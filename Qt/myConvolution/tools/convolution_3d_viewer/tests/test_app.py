from __future__ import annotations

import json
import math
import os
from pathlib import Path
import struct
import sys
import threading
import unittest
from unittest.mock import MagicMock, patch
from urllib.error import HTTPError
from urllib.request import Request, urlopen

import numpy as np


TOOL_DIR = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOL_DIR))

import app  # noqa: E402


def encode_regular(values: np.ndarray, family: str, byte_order: str) -> bytes:
    endian = "<" if byte_order == "le" else ">"
    flat = np.asarray(values, dtype=np.float64).ravel(order="C")
    header = struct.pack(f"{endian}I", flat.size)
    if family == "double":
        payload = flat.astype(f"{endian}f8").tobytes()
    elif family == "real":
        payload = flat.astype(f"{endian}f4").tobytes()
    elif family == "smallint":
        raw = np.array([int(value * 30_000.0) for value in flat], dtype=np.int16)
        payload = raw.astype(f"{endian}i2").tobytes()
    else:
        raise AssertionError(family)
    return header + payload


def encode_packed(values: np.ndarray, bits: int, byte_order: str) -> bytes:
    endian = "<" if byte_order == "le" else ">"
    flat = np.asarray(values, dtype=np.float64).ravel(order="C")
    maximum = (1 << bits) - 1
    codes = [int(min(1.0, max(0.0, float(value))) * maximum) for value in flat]

    stream = 0
    for index, code in enumerate(codes):
        stream |= code << (index * bits)
    word_count = (len(codes) * bits + 31) // 32
    words = [(stream >> (word_index * 32)) & 0xFFFFFFFF for word_index in range(word_count)]
    return struct.pack(f"{endian}I", len(codes)) + b"".join(
        struct.pack(f"{endian}I", word) for word in words
    )


class DecoderTests(unittest.TestCase):
    def setUp(self) -> None:
        self.source = np.array(
            [
                [0.0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75],
                [0.875, 1.0, 0.9375, 0.8125, 0.6875, 0.5625, 0.4375],
                [0.3125, 0.1875, 0.0625, 0.03125, 0.2222, 0.7777, 0.9999],
            ],
            dtype=np.float64,
        )

    def test_regular_formats_both_byte_orders(self) -> None:
        for family in ("double", "real", "smallint"):
            for byte_order in ("le", "be"):
                with self.subTest(family=family, byte_order=byte_order):
                    blob = encode_regular(self.source, family, byte_order)
                    decoded = app.decode_convolution(blob, f"{family}_{byte_order}", 7, 3)
                    if family == "double":
                        expected = self.source
                        tolerance = 1e-14
                    elif family == "real":
                        expected = self.source.astype(np.float32).astype(np.float64)
                        tolerance = 1e-7
                    else:
                        expected = np.trunc(self.source * 30_000.0) / 30_000.0
                        tolerance = 1e-14
                    np.testing.assert_allclose(decoded, expected, rtol=0, atol=tolerance)

    def test_all_packed_widths_and_byte_orders(self) -> None:
        for bits in range(2, 9):
            maximum = (1 << bits) - 1
            expected = np.trunc(np.clip(self.source, 0.0, 1.0) * maximum) / maximum
            for byte_order in ("le", "be"):
                with self.subTest(bits=bits, byte_order=byte_order):
                    blob = encode_packed(self.source, bits, byte_order)
                    decoded = app.decode_convolution(blob, f"pa_{bits}b_{byte_order}", 7, 3)
                    np.testing.assert_allclose(decoded, expected, rtol=0, atol=1e-14)

    def test_rejects_header_shape_mismatch(self) -> None:
        blob = encode_regular(self.source, "double", "le")
        with self.assertRaisesRegex(app.ViewerError, "count_h×count_v"):
            app.decode_convolution(blob, "double_le", 8, 3)

    def test_rejects_truncated_payload(self) -> None:
        blob = encode_packed(self.source, 7, "be")[:-1]
        with self.assertRaisesRegex(app.ViewerError, "Размер BYTEA"):
            app.decode_convolution(blob, "pa_7b_be", 7, 3)

    def test_rejects_unknown_type(self) -> None:
        with self.assertRaisesRegex(app.ViewerError, "Неизвестный data_type"):
            app.decode_convolution(b"\0\0\0\0", "mystery_le", 1, 1)


class SurfaceTests(unittest.TestCase):
    def test_axis_count_uses_generator_floor_contract(self) -> None:
        app._validate_axis(0.0, 359.0, 2.0, 179, "H")
        with self.assertRaisesRegex(app.ViewerError, "floor"):
            app._validate_axis(0.0, 359.0, 2.0, 180, "H")

    def test_pooling_preserves_global_peak(self) -> None:
        z = np.zeros((91, 3600), dtype=np.float64)
        z[37, 1742] = 1.0
        x = np.arange(3600, dtype=np.float64) * 0.1
        y = np.arange(91, dtype=np.float64) - 45.0
        reduced, shown_x, shown_y, seam = app.downsample_surface(
            z, x, y, 480, 180, 360.0
        )
        self.assertTrue(seam)
        self.assertEqual(reduced.shape, (91, 481))
        self.assertEqual(shown_x.shape, (481,))
        self.assertEqual(shown_y.shape, (91,))
        self.assertTrue(math.isclose(float(np.max(reduced)), 1.0))

    def test_demo_covers_posts_and_two_formats(self) -> None:
        settings = app.ViewerSettings(app.DEFAULT_QUERY_PATH, 480, 180, False)
        payload = app.load_demo(settings)
        self.assertTrue(payload["ok"])
        self.assertEqual(payload["summary"]["posts"], 3)
        self.assertEqual(payload["summary"]["types"], 2)
        self.assertEqual(payload["summary"]["records"], 18)
        self.assertFalse(payload["errors"])
        self.assertNotIn("surfaces", payload)

        records = payload["records"]
        history = [
            record
            for record in records
            if record["post"] == "Север" and record["dataType"] == "pa_4b_le"
        ]
        self.assertEqual(len(history), 3)
        self.assertEqual([record["historyPosition"] for record in history], [1, 2, 3])
        self.assertTrue(all(record["historyCount"] == 3 for record in history))
        self.assertTrue(all(record["convBytes"] > 0 for record in history))
        self.assertTrue(all("z" not in record for record in records))


class LazyCatalogTests(unittest.TestCase):
    def setUp(self) -> None:
        self.settings = app.ViewerSettings(app.DEFAULT_QUERY_PATH, 120, 90, False)
        self.demo_rows = app._demo_rows()

    def tearDown(self) -> None:
        server = getattr(self, "server", None)
        if server is not None:
            server.server_close()

    def make_server(self) -> app.ViewerHTTPServer:
        self.server = app.ViewerHTTPServer(
            ("127.0.0.1", 0),
            app.ViewerHandler,
            token="test-token",
            settings=self.settings,
        )
        return self.server

    def test_catalog_validation_never_reads_conv_blob(self) -> None:
        class BlobGuard(dict[str, object]):
            def __getitem__(self, key: str) -> object:
                if key == "conv":
                    raise AssertionError("catalog attempted to read BYTEA")
                return super().__getitem__(key)

        row = BlobGuard(self.demo_rows[0])
        records, errors = app.build_catalog([row])
        self.assertFalse(errors)
        self.assertEqual(len(records), 1)
        self.assertNotIn("conv", records[0])
        self.assertNotIn("z", records[0])

    def test_demo_surface_is_decoded_only_for_allowed_catalog_id(self) -> None:
        server = self.make_server()
        response = app.load_demo(self.settings, self.demo_rows)
        session_id = server.activate_demo(
            self.demo_rows,
            response["records"],
            server.begin_catalog_load(),
        )

        selected_id = int(response["records"][0]["id"])
        source, surface = server.surface(selected_id, session_id)
        self.assertEqual(source, "demo")
        self.assertEqual(surface["id"], str(selected_id))
        self.assertIn("z", surface)
        self.assertIn("stats", surface)

        with self.assertRaisesRegex(app.ViewerError, "загруженном каталоге"):
            server.surface(-999_999, session_id)

    def test_live_surface_query_is_parameterized_and_catalog_scoped(self) -> None:
        server = self.make_server()
        catalog_record = app.build_catalog([self.demo_rows[0]])[0][0]
        connection = {"host": "db", "password": "private"}
        session_id = server.activate_postgresql(
            connection,
            [catalog_record],
            server.begin_catalog_load(),
        )

        expected_surface = {"id": catalog_record["id"], "z": [[1.0]]}
        with patch.object(
            app,
            "fetch_surface_from_database",
            return_value=expected_surface,
        ) as fetch:
            source, surface = server.surface(int(catalog_record["id"]), session_id)
            self.assertEqual(source, "postgresql")
            self.assertEqual(surface, expected_surface)
            fetch.assert_called_once()
            call = fetch.call_args
            self.assertEqual(call.args[1], int(catalog_record["id"]))
            self.assertEqual(call.args[3], catalog_record)

            with self.assertRaisesRegex(app.ViewerError, "загруженном каталоге"):
                server.surface(123_456, session_id)
            self.assertEqual(fetch.call_count, 1)

        self.assertEqual(server._demo_rows_by_id, {})
        self.assertFalse(any("conv" in record for record in server._catalog_by_id.values()))

    def test_stale_session_is_rejected_before_and_after_lazy_fetch(self) -> None:
        server = self.make_server()
        response = app.load_demo(self.settings, self.demo_rows)
        old_session_id = server.activate_demo(
            self.demo_rows,
            response["records"],
            server.begin_catalog_load(),
        )
        selected_id = int(response["records"][0]["id"])

        replacement_session_id = server.activate_demo(
            self.demo_rows,
            response["records"],
            server.begin_catalog_load(),
        )
        self.assertNotEqual(old_session_id, replacement_session_id)
        with self.assertRaisesRegex(app.ViewerError, "устарел"):
            server.surface(selected_id, old_session_id)

        old_session_id = server.activate_postgresql(
            {"host": "db"},
            [response["records"][0]],
            server.begin_catalog_load(),
        )

        def reload_while_fetching(*_args: object) -> dict[str, object]:
            server.activate_demo(
                self.demo_rows,
                response["records"],
                server.begin_catalog_load(),
            )
            return {"id": selected_id, "z": [[1.0]]}

        with patch.object(
            app,
            "fetch_surface_from_database",
            side_effect=reload_while_fetching,
        ):
            with self.assertRaisesRegex(app.ViewerError, "устарел"):
                server.surface(selected_id, old_session_id)

    def test_empty_activation_preserves_previous_catalog(self) -> None:
        server = self.make_server()
        response = app.load_demo(self.settings, self.demo_rows)
        session_id = server.activate_demo(
            self.demo_rows,
            response["records"],
            server.begin_catalog_load(),
        )
        selected_id = int(response["records"][0]["id"])

        with self.assertRaisesRegex(app.ViewerError, "ни одной"):
            server.activate_postgresql(
                {"host": "db"},
                [],
                server.begin_catalog_load(),
            )

        source, surface = server.surface(selected_id, session_id)
        self.assertEqual(source, "demo")
        self.assertEqual(surface["id"], str(selected_id))

    def test_older_catalog_completion_cannot_replace_newer_load(self) -> None:
        server = self.make_server()
        response = app.load_demo(self.settings, self.demo_rows)
        older_ticket = server.begin_catalog_load()
        newer_ticket = server.begin_catalog_load()
        newer_session = server.activate_demo(
            self.demo_rows,
            response["records"],
            newer_ticket,
        )

        with self.assertRaisesRegex(app.ViewerError, "более новый"):
            server.activate_postgresql(
                {"host": "slow-old-database"},
                [response["records"][0]],
                older_ticket,
            )

        selected_id = int(response["records"][0]["id"])
        source, surface = server.surface(selected_id, newer_session)
        self.assertEqual(source, "demo")
        self.assertEqual(surface["id"], str(selected_id))

    def test_bigint_id_roundtrips_as_string_and_fetches_as_python_int(self) -> None:
        big_id = 9_007_199_254_740_993
        row = dict(self.demo_rows[0])
        row["id"] = big_id
        records, errors = app.build_catalog([row])
        self.assertFalse(errors)
        self.assertEqual(records[0]["id"], "9007199254740993")
        self.assertEqual(json.loads(json.dumps(records))[0]["id"], str(big_id))

        server = self.make_server()
        session_id = server.activate_postgresql(
            {"host": "db"},
            records,
            server.begin_catalog_load(),
        )
        self.assertIn(big_id, server._catalog_by_id)

        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        host, port = server.server_address[:2]
        request = Request(
            f"http://{host}:{port}/api/surface?token=test-token",
            data=json.dumps(
                {"id": str(big_id), "sessionId": session_id}
            ).encode("utf-8"),
            headers={"Content-Type": "application/json"},
            method="POST",
        )
        try:
            with patch.object(
                app,
                "fetch_surface_from_database",
                return_value={"id": "incorrect", "z": [[1.0]]},
            ) as fetch:
                with urlopen(request, timeout=5) as response:
                    detail = json.loads(response.read())
        finally:
            server.shutdown()
            thread.join(timeout=5)

        surface = detail["surface"]
        self.assertEqual(detail["source"], "postgresql")
        self.assertEqual(surface["id"], str(big_id))
        self.assertEqual(fetch.call_args.args[1], big_id)
        self.assertIs(type(fetch.call_args.args[1]), int)
        self.assertEqual(json.loads(json.dumps(surface))["id"], str(big_id))

    def test_record_id_parser_accepts_canonical_strings_and_json_ints_only(self) -> None:
        self.assertEqual(app._parse_record_id("-17"), -17)
        self.assertEqual(app._parse_record_id("9007199254740993"), 9_007_199_254_740_993)
        self.assertEqual(app._parse_record_id(42), 42)

        invalid_values: list[object] = [
            True,
            1.0,
            "",
            "01",
            "-0",
            "+1",
            " 1",
            "1 ",
            "1.0",
            "--1",
            str(1 << 63),
            str(-(1 << 63) - 1),
        ]
        for value in invalid_values:
            with self.subTest(value=value):
                with self.assertRaises(app.ViewerError):
                    app._parse_record_id(value)

    def test_read_only_executor_fetches_only_limit_plus_one(self) -> None:
        connect_context = MagicMock()
        connection = MagicMock()
        transaction_context = MagicMock()
        cursor_context = MagicMock()
        cursor = MagicMock()
        connect_context.__enter__.return_value = connection
        connection.transaction.return_value = transaction_context
        connection.cursor.return_value = cursor_context
        cursor_context.__enter__.return_value = cursor
        cursor.fetchmany.return_value = [{"id": 1}, {"id": 2}]

        with patch.object(app.psycopg, "connect", return_value=connect_context):
            with self.assertRaisesRegex(app.ViewerError, "больше 1"):
                app._execute_read_only({}, "SELECT 1", max_rows=1)

        cursor.fetchmany.assert_called_once_with(2)

    def test_catalog_and_detail_apply_their_fetch_limits(self) -> None:
        with patch.object(
            app,
            "_execute_read_only",
            return_value=[self.demo_rows[0]],
        ) as execute:
            response, _connection = app.load_database_session({}, self.settings)
        self.assertEqual(response["summary"]["records"], 1)
        self.assertEqual(execute.call_args.kwargs["max_rows"], app.MAX_CATALOG_RECORDS)

        with patch.object(app, "_execute_read_only", return_value=[]) as execute:
            with self.assertRaisesRegex(app.ViewerError, "не найдена"):
                app.fetch_surface_from_database({}, 123, self.settings)
        self.assertEqual(execute.call_args.kwargs["max_rows"], 1)

    def test_detail_sql_uses_primary_key_placeholder(self) -> None:
        self.assertIn("WHERE result.id = %s", app.SURFACE_BY_ID_QUERY)
        self.assertIn("conv.conv", app.SURFACE_BY_ID_QUERY)
        self.assertNotIn(str(self.demo_rows[0]["id"]), app.SURFACE_BY_ID_QUERY)

    def test_http_demo_catalog_and_lazy_surface_endpoints(self) -> None:
        server = self.make_server()
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        host, port = server.server_address[:2]
        base_url = f"http://{host}:{port}"
        try:
            with urlopen(f"{base_url}/api/demo?token=test-token", timeout=5) as response:
                catalog = json.loads(response.read())
            self.assertEqual(catalog["summary"]["records"], 18)
            self.assertNotIn("surfaces", catalog)
            self.assertIsInstance(catalog["sessionId"], str)
            self.assertTrue(catalog["sessionId"])

            selected_id = catalog["records"][0]["id"]
            request = Request(
                f"{base_url}/api/surface?token=test-token",
                data=json.dumps(
                    {"id": selected_id, "sessionId": catalog["sessionId"]}
                ).encode("utf-8"),
                headers={"Content-Type": "application/json"},
                method="POST",
            )
            with urlopen(request, timeout=5) as response:
                detail = json.loads(response.read())
            self.assertTrue(detail["ok"])
            self.assertEqual(detail["sessionId"], catalog["sessionId"])
            self.assertEqual(detail["surface"]["id"], selected_id)
            self.assertIn("z", detail["surface"])

            invalid_request = Request(
                f"{base_url}/api/surface?token=test-token",
                data=json.dumps(
                    {"id": 1.5, "sessionId": catalog["sessionId"]}
                ).encode("utf-8"),
                headers={"Content-Type": "application/json"},
                method="POST",
            )
            with self.assertRaises(HTTPError) as invalid_response:
                urlopen(invalid_request, timeout=5)
            self.assertEqual(invalid_response.exception.code, 400)
            invalid_response.exception.close()
        finally:
            server.shutdown()
            thread.join(timeout=5)

    def test_query_sql_lists_all_history_without_returning_blob_column(self) -> None:
        query = app.DEFAULT_QUERY_PATH.read_text(encoding="utf-8")
        self.assertIn("row_number() OVER", query)
        self.assertIn("history_count", query)
        self.assertIn("octet_length(conv.conv)", query)
        self.assertNotIn("pg_column_size", query)
        self.assertIn("result.result_timestamp DESC NULLS LAST", query)
        self.assertNotIn("WHERE row_position", query)
        self.assertNotRegex(query, r"(?m)^\s*conv\.conv\s*,?\s*$")


class ConnectionDefaultsTests(unittest.TestCase):
    def test_requested_credentials_are_used_by_default(self) -> None:
        with patch.dict(os.environ, {}, clear=True):
            defaults = app.connection_defaults()

        self.assertEqual(defaults["user"], "russkiylis")
        self.assertEqual(defaults["password"], "1337")
        self.assertFalse(defaults["passwordFromEnvironment"])

    def test_environment_credentials_override_builtins(self) -> None:
        with patch.dict(
            os.environ,
            {"PGUSER": "from-environment", "PGPASSWORD": "secret-from-environment"},
            clear=True,
        ):
            defaults = app.connection_defaults()

        self.assertEqual(defaults["user"], "from-environment")
        self.assertEqual(defaults["password"], "")
        self.assertTrue(defaults["passwordFromEnvironment"])


if __name__ == "__main__":
    unittest.main()
