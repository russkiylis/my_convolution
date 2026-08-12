#!/usr/bin/env python3
"""Standalone PostgreSQL -> interactive 3D convolution viewer.

The tool deliberately does not import or execute myConvolution.  It knows only
the documented database row contract and serves a loopback-only browser UI.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from datetime import date, datetime
from decimal import Decimal
import gzip
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
import math
import os
from pathlib import Path
import re
import secrets
import struct
import threading
import webbrowser
from urllib.parse import parse_qs, urlparse

import numpy as np
import psycopg
from psycopg.rows import dict_row
from plotly.offline import get_plotlyjs


TOOL_DIR = Path(__file__).resolve().parent
DEFAULT_QUERY_PATH = TOOL_DIR / "query.sql"
INDEX_PATH = TOOL_DIR / "index.html"
DEFAULT_DB_USER = "russkiylis"
DEFAULT_DB_PASSWORD = "1337"

DATA_TYPE_RE = re.compile(
    r"^(double|real|smallint|pa_([2-8])b)_(le|be)$"
)
DECIMAL_ID_RE = re.compile(r"^(?:0|-[1-9][0-9]*|[1-9][0-9]*)$")
POSTGRES_BIGINT_MIN = -(1 << 63)
POSTGRES_BIGINT_MAX = (1 << 63) - 1
MAX_RECORDS = 200
MAX_CATALOG_RECORDS = 20_000
MAX_ELEMENTS_PER_RECORD = 5_000_000
MAX_REQUEST_BYTES = 32_768
SSL_MODES = {"disable", "allow", "prefer", "require", "verify-ca", "verify-full"}

SURFACE_BY_ID_QUERY = """
SELECT
    result.id,
    result.result_timestamp,
    result.sysname,
    result.azimuth::double precision AS azimuth,
    result.elevation::double precision AS elevation,
    result.power::double precision AS power,
    result.frequency::double precision AS frequency,
    result.latitude::double precision AS latitude,
    result.longitude::double precision AS longitude,
    conv.data_type::text AS data_type,
    conv.quality::double precision AS quality,
    conv.min_angle_h::double precision AS min_angle_h,
    conv.max_angle_h::double precision AS max_angle_h,
    conv.step_h::double precision AS step_h,
    conv.count_h::integer AS count_h,
    conv.min_angle_v::double precision AS min_angle_v,
    conv.max_angle_v::double precision AS max_angle_v,
    conv.step_v::double precision AS step_v,
    conv.count_v::integer AS count_v,
    conv.conv
FROM public.df_result AS result
JOIN public.df_conv AS conv ON conv.id = result.id
WHERE result.id = %s
  AND conv.conv IS NOT NULL
  AND conv.count_h > 0
  AND conv.count_v > 0
""".strip()


class ViewerError(RuntimeError):
    """A validation or user-facing data error."""


@dataclass(frozen=True)
class ViewerSettings:
    query_path: Path
    max_h_samples: int
    max_v_samples: int
    demo_on_start: bool


def _finite_float(value: object, field: str, *, nullable: bool = True) -> float | None:
    if value is None:
        if nullable:
            return None
        raise ViewerError(f"Поле {field} не должно быть NULL")
    try:
        result = float(value)
    except (TypeError, ValueError, OverflowError) as error:
        raise ViewerError(f"Поле {field} не является числом") from error
    if not math.isfinite(result):
        raise ViewerError(f"Поле {field} содержит NaN/Inf")
    return result


def _positive_int(value: object, field: str) -> int:
    try:
        result = int(value)
    except (TypeError, ValueError, OverflowError) as error:
        raise ViewerError(f"Поле {field} не является целым числом") from error
    if result <= 0:
        raise ViewerError(f"Поле {field} должно быть больше нуля")
    return result


def _parse_record_id(value: object) -> int:
    """Parse a PostgreSQL BIGINT id without accepting lossy JSON numbers."""

    if type(value) is int:
        record_id = value
    elif type(value) is str and DECIMAL_ID_RE.fullmatch(value) is not None:
        record_id = int(value)
    else:
        raise ViewerError("Поле id должно быть целым числом или десятичной строкой")

    if not POSTGRES_BIGINT_MIN <= record_id <= POSTGRES_BIGINT_MAX:
        raise ViewerError("Поле id выходит за диапазон PostgreSQL BIGINT")
    return record_id


def _iso_timestamp(value: object) -> str:
    if isinstance(value, (datetime, date)):
        return value.isoformat()
    return str(value or "")


def _validate_axis(
    minimum: float,
    maximum: float,
    step: float,
    count: int,
    axis_name: str,
) -> None:
    if step <= 0:
        raise ViewerError(f"Шаг оси {axis_name} должен быть положительным")
    if maximum <= minimum:
        raise ViewerError(f"Диапазон оси {axis_name} задан неверно")

    calculated = (maximum - minimum) / step
    tolerance = 1e-9 * max(1.0, abs(calculated), float(count))
    expected_count = math.floor(calculated + tolerance)
    if expected_count != count:
        raise ViewerError(
            f"Размер оси {axis_name} ({count}) не согласован с диапазоном и шагом "
            f"(floor({calculated:g}) = {expected_count})"
        )


def _post_name(value: object) -> str:
    if value is None:
        return "Без имени (NULL)"
    name = str(value).strip()
    return name or "Без имени (пусто)"


def _record_payload(
    row: dict[str, object],
    *,
    require_history: bool = True,
) -> dict[str, object]:
    """Validate and serialize one lightweight catalog row without its BYTEA."""

    try:
        record_id = _parse_record_id(row["id"])
    except KeyError as error:
        raise ViewerError("В строке отсутствует поле id") from error

    try:
        data_type = str(row["data_type"])
    except KeyError as error:
        raise ViewerError("В строке отсутствует поле data_type") from error
    if DATA_TYPE_RE.fullmatch(data_type) is None:
        raise ViewerError(f"Неизвестный data_type: {data_type!r}")

    try:
        count_h = _positive_int(row["count_h"], "count_h")
        count_v = _positive_int(row["count_v"], "count_v")
    except KeyError as error:
        raise ViewerError(f"В строке отсутствует поле {error.args[0]}") from error
    element_count = count_h * count_v
    if element_count > MAX_ELEMENTS_PER_RECORD:
        raise ViewerError(
            f"Сетка {count_h}×{count_v} превышает лимит "
            f"{MAX_ELEMENTS_PER_RECORD:,} точек"
        )

    try:
        min_h = _finite_float(row["min_angle_h"], "min_angle_h", nullable=False)
        max_h = _finite_float(row["max_angle_h"], "max_angle_h", nullable=False)
        step_h = _finite_float(row["step_h"], "step_h", nullable=False)
        min_v = _finite_float(row["min_angle_v"], "min_angle_v", nullable=False)
        max_v = _finite_float(row["max_angle_v"], "max_angle_v", nullable=False)
        step_v = _finite_float(row["step_v"], "step_v", nullable=False)
    except KeyError as error:
        raise ViewerError(f"В строке отсутствует поле {error.args[0]}") from error
    assert min_h is not None and max_h is not None and step_h is not None
    assert min_v is not None and max_v is not None and step_v is not None

    _validate_axis(min_h, max_h, step_h, count_h, "H")
    _validate_axis(min_v, max_v, step_v, count_v, "V")

    history: dict[str, int] = {}
    if require_history:
        try:
            conv_bytes = _positive_int(row["conv_bytes"], "conv_bytes")
            history_position = _positive_int(row["history_position"], "history_position")
            history_count = _positive_int(row["history_count"], "history_count")
        except KeyError as error:
            raise ViewerError(f"В строке отсутствует поле {error.args[0]}") from error
        if conv_bytes < 4:
            raise ViewerError("conv_bytes меньше четырёхбайтового заголовка")
        if history_position > history_count:
            raise ViewerError(
                "history_position не может быть больше history_count"
            )
        history = {
            "convBytes": conv_bytes,
            "historyPosition": history_position,
            "historyCount": history_count,
        }

    return {
        # PostgreSQL BIGINT exceeds JavaScript's exact integer range.  Keep the
        # canonical decimal representation throughout every JSON response.
        "id": str(record_id),
        "post": _post_name(row.get("sysname")),
        "timestamp": _iso_timestamp(row.get("result_timestamp")),
        "dataType": data_type,
        "quality": _finite_float(row.get("quality"), "quality"),
        "azimuth": _finite_float(row.get("azimuth"), "azimuth"),
        "elevation": _finite_float(row.get("elevation"), "elevation"),
        "power": _finite_float(row.get("power"), "power"),
        "frequency": _finite_float(row.get("frequency"), "frequency"),
        "latitude": _finite_float(row.get("latitude"), "latitude"),
        "longitude": _finite_float(row.get("longitude"), "longitude"),
        **history,
        "grid": {
            "countH": count_h,
            "countV": count_v,
            "minH": min_h,
            "maxH": max_h,
            "stepH": step_h,
            "minV": min_v,
            "maxV": max_v,
            "stepV": step_v,
        },
    }


def decode_convolution(
    blob_value: object,
    data_type: str,
    count_h: int,
    count_v: int,
) -> np.ndarray:
    """Decode the exact QDataStream/PackedArray format used in df_conv.conv."""

    match = DATA_TYPE_RE.fullmatch(data_type)
    if match is None:
        raise ViewerError(f"Неизвестный data_type: {data_type!r}")

    if blob_value is None:
        raise ViewerError("Поле conv равно NULL")
    try:
        blob = bytes(blob_value)
    except (TypeError, ValueError) as error:
        raise ViewerError("Поле conv не является BYTEA") from error
    if len(blob) < 4:
        raise ViewerError("BYTEA короче четырёхбайтового заголовка")

    element_count = count_h * count_v
    if element_count > MAX_ELEMENTS_PER_RECORD:
        raise ViewerError(
            f"Сетка {count_h}×{count_v} превышает лимит "
            f"{MAX_ELEMENTS_PER_RECORD:,} точек"
        )

    family, bit_group, byte_order = match.groups()
    endian = "<" if byte_order == "le" else ">"
    encoded_count = struct.unpack_from(f"{endian}I", blob, 0)[0]
    if encoded_count != element_count:
        raise ViewerError(
            f"В BYTEA записано {encoded_count} элементов, "
            f"а count_h×count_v = {element_count}"
        )

    payload = memoryview(blob)[4:]
    if family == "double":
        expected_bytes = element_count * 8
        dtype = np.dtype(f"{endian}f8")
        scale = None
    elif family == "real":
        expected_bytes = element_count * 4
        dtype = np.dtype(f"{endian}f4")
        scale = None
    elif family == "smallint":
        expected_bytes = element_count * 2
        dtype = np.dtype(f"{endian}i2")
        scale = 30_000.0
    else:
        bits = int(bit_group)
        word_count = (element_count * bits + 31) // 32
        expected_bytes = word_count * 4
        if len(payload) != expected_bytes:
            raise ViewerError(
                f"Размер BYTEA для {data_type} равен {len(blob)}, "
                f"ожидалось {expected_bytes + 4}"
            )

        words = np.frombuffer(payload, dtype=np.dtype(f"{endian}u4"))
        words = words.astype(np.uint64, copy=False)
        padded_words = np.concatenate((words, np.zeros(1, dtype=np.uint64)))

        bit_positions = np.arange(element_count, dtype=np.uint64) * bits
        word_indices = (bit_positions >> np.uint64(5)).astype(np.intp)
        shifts = bit_positions & np.uint64(31)
        codes = padded_words[word_indices] >> shifts

        crosses_word = shifts + bits > 32
        if np.any(crosses_word):
            crossing_indices = word_indices[crosses_word]
            crossing_shifts = shifts[crosses_word]
            codes[crosses_word] |= (
                padded_words[crossing_indices + 1] << (32 - crossing_shifts)
            )

        mask = (1 << bits) - 1
        values = (codes & mask).astype(np.float64) / float(mask)
        return values.reshape((count_v, count_h), order="C")

    if len(payload) != expected_bytes:
        raise ViewerError(
            f"Размер BYTEA для {data_type} равен {len(blob)}, "
            f"ожидалось {expected_bytes + 4}"
        )

    values = np.frombuffer(payload, dtype=dtype, count=element_count)
    values = values.astype(np.float64, copy=False)
    if scale is not None:
        values = values / scale
    if not np.all(np.isfinite(values)):
        raise ViewerError("После декодирования обнаружены NaN/Inf")
    return values.reshape((count_v, count_h), order="C")


def _pool_axis(values: np.ndarray, target: int, axis: int) -> tuple[np.ndarray, np.ndarray]:
    size = values.shape[axis]
    if target >= size:
        return values, np.arange(size, dtype=np.intp)

    starts = np.floor(np.arange(target, dtype=np.float64) * size / target).astype(np.intp)
    reduced = np.maximum.reduceat(values, starts, axis=axis)
    ends = np.concatenate((starts[1:], np.array([size], dtype=np.intp)))
    centers = ((starts + ends - 1) // 2).astype(np.intp)
    return reduced, centers


def downsample_surface(
    z: np.ndarray,
    x: np.ndarray,
    y: np.ndarray,
    max_h_samples: int,
    max_v_samples: int,
    max_angle_h: float,
) -> tuple[np.ndarray, np.ndarray, np.ndarray, bool]:
    """Peak-preserving 2D max pooling for a responsive Plotly surface."""

    reduced_h, h_indices = _pool_axis(z, max_h_samples, axis=1)
    reduced, v_indices = _pool_axis(reduced_h, max_v_samples, axis=0)
    shown_x = x[h_indices]
    shown_y = y[v_indices]

    seam_closed = False
    horizontal_span = max_angle_h - float(x[0])
    if shown_x.size > 1 and math.isclose(horizontal_span, 360.0, abs_tol=1e-6):
        shown_x = np.concatenate((shown_x, np.array([max_angle_h])))
        reduced = np.concatenate((reduced, reduced[:, :1]), axis=1)
        seam_closed = True

    return reduced, shown_x, shown_y, seam_closed


def _surface_payload(
    row: dict[str, object],
    settings: ViewerSettings,
) -> dict[str, object]:
    record = _record_payload(row, require_history=False)
    data_type = str(record["dataType"])
    grid = record["grid"]
    assert isinstance(grid, dict)
    count_h = int(grid["countH"])
    count_v = int(grid["countV"])
    min_h = float(grid["minH"])
    max_h = float(grid["maxH"])
    step_h = float(grid["stepH"])
    min_v = float(grid["minV"])
    max_v = float(grid["maxV"])
    step_v = float(grid["stepV"])

    z = decode_convolution(row["conv"], data_type, count_h, count_v)
    x = min_h + np.arange(count_h, dtype=np.float64) * step_h
    y = min_v + np.arange(count_v, dtype=np.float64) * step_v

    flat_peak_index = int(np.argmax(z))
    peak_v_index, peak_h_index = np.unravel_index(flat_peak_index, z.shape)
    value_min = float(np.min(z))
    value_max = float(z[peak_v_index, peak_h_index])
    value_mean = float(np.mean(z))

    shown_z, shown_x, shown_y, seam_closed = downsample_surface(
        z,
        x,
        y,
        settings.max_h_samples,
        settings.max_v_samples,
        max_h,
    )

    return {
        **record,
        "grid": {
            **grid,
            "shownH": int(shown_x.size),
            "shownV": int(shown_y.size),
            "seamClosed": seam_closed,
        },
        "stats": {
            "min": value_min,
            "max": value_max,
            "mean": value_mean,
            "peakH": float(x[peak_h_index]),
            "peakV": float(y[peak_v_index]),
        },
        "x": np.round(shown_x, 6).tolist(),
        "y": np.round(shown_y, 6).tolist(),
        "z": np.round(shown_z, 6).tolist(),
    }


def build_surfaces(
    rows: list[dict[str, object]],
    settings: ViewerSettings,
) -> tuple[list[dict[str, object]], list[dict[str, object]]]:
    if len(rows) > MAX_RECORDS:
        raise ViewerError(
            f"Запрос вернул {len(rows)} строк; безопасный лимит — {MAX_RECORDS}. "
            "Ограничьте query.sql."
        )

    surfaces: list[dict[str, object]] = []
    errors: list[dict[str, object]] = []
    for row in rows:
        row_id = row.get("id", "?")
        try:
            surfaces.append(_surface_payload(row, settings))
        except (KeyError, TypeError, ValueError, OverflowError, ViewerError) as error:
            errors.append({"id": str(row_id), "message": str(error)})

    surfaces.sort(key=lambda item: (str(item["post"]).casefold(), str(item["dataType"])))
    return surfaces, errors


def build_catalog(
    rows: list[dict[str, object]],
) -> tuple[list[dict[str, object]], list[dict[str, object]]]:
    """Build lightweight history metadata while never reading the conv column."""

    if len(rows) > MAX_CATALOG_RECORDS:
        raise ViewerError(
            f"Запрос вернул {len(rows)} строк; лимит каталога — "
            f"{MAX_CATALOG_RECORDS:,}. Ограничьте query.sql."
        )

    records: list[dict[str, object]] = []
    errors: list[dict[str, object]] = []
    seen_ids: set[int] = set()
    for row in rows:
        row_id = row.get("id", "?")
        try:
            record = _record_payload(row)
            record_id = int(record["id"])
            if record_id in seen_ids:
                raise ViewerError(f"Запрос повторно вернул id={record_id}")
            seen_ids.add(record_id)
            records.append(record)
        except (KeyError, TypeError, ValueError, OverflowError, ViewerError) as error:
            errors.append({"id": str(row_id), "message": str(error)})

    return records, errors


def _demo_rows() -> list[dict[str, object]]:
    rng = np.random.default_rng(20260811)
    count_h = 360
    count_v = 91
    x = np.arange(count_h, dtype=np.float64)
    y = np.linspace(-45.0, 45.0, count_v)
    xx, yy = np.meshgrid(x, y)

    def circular_delta(values: np.ndarray, center: float) -> np.ndarray:
        return (values - center + 180.0) % 360.0 - 180.0

    def make_surface(peaks: list[tuple[float, float, float, float, float]]) -> np.ndarray:
        surface = np.full_like(xx, 0.018)
        for h_center, v_center, amplitude, h_width, v_width in peaks:
            dh = circular_delta(xx, h_center)
            surface += amplitude * np.exp(
                -0.5 * ((dh / h_width) ** 2 + ((yy - v_center) / v_width) ** 2)
            )
        surface += rng.normal(0.0, 0.006, size=surface.shape)
        surface = np.clip(surface, 0.0, None)
        return surface / float(np.max(surface))

    definitions = [
        ("Север", [(62, 8, 1.0, 13, 7), (245, -17, 0.55, 21, 10)]),
        ("Восток", [(128, -5, 1.0, 10, 6), (312, 22, 0.7, 17, 8)]),
        ("Резерв", [(350, 3, 1.0, 9, 5), (178, -28, 0.45, 24, 9)]),
    ]
    formats = ("double_le", "pa_4b_le")
    rows: list[dict[str, object]] = []
    row_id = -1
    for post_index, (post, peaks) in enumerate(definitions):
        base = make_surface(peaks)
        for data_type in formats:
            for history_index in range(3):
                display = np.roll(base, shift=history_index * 2, axis=1)
                if data_type.startswith("pa_4b"):
                    display = np.trunc(np.clip(display, 0.0, 1.0) * 15.0) / 15.0
                endian = "<"
                header = struct.pack(f"{endian}I", display.size)
                if data_type == "double_le":
                    blob = header + display.astype("<f8").tobytes(order="C")
                else:
                    flat_codes = np.trunc(display.ravel(order="C") * 15.0).astype(np.uint32)
                    word_count = (flat_codes.size * 4 + 31) // 32
                    words = np.zeros(word_count, dtype=np.uint32)
                    for index, code in enumerate(flat_codes):
                        bit_position = index * 4
                        words[bit_position // 32] |= code << (bit_position % 32)
                    blob = header + words.astype("<u4").tobytes()

                rows.append(
                    {
                        "id": row_id,
                        "result_timestamp": datetime(
                            2026,
                            8,
                            11,
                            12,
                            40 + post_index - history_index * 10,
                            0,
                        ),
                        "sysname": post,
                        "azimuth": peaks[0][0] + history_index * 2,
                        "elevation": peaks[0][1],
                        "power": -38.5 + post_index - history_index * 0.6,
                        "frequency": 433_920_000.0 + post_index * 25_000.0,
                        "latitude": 55.75 + post_index * 0.01,
                        "longitude": 37.61 + post_index * 0.01,
                        "data_type": data_type,
                        "quality": 0.92 - post_index * 0.04 - history_index * 0.025,
                        "min_angle_h": 0.0,
                        "max_angle_h": 360.0,
                        "step_h": 1.0,
                        "count_h": count_h,
                        "min_angle_v": -45.0,
                        "max_angle_v": 46.0,
                        "step_v": 1.0,
                        "count_v": count_v,
                        "conv_bytes": len(blob),
                        "history_position": history_index + 1,
                        "history_count": 3,
                        "conv": blob,
                    }
                )
                row_id -= 1
    return rows


def _connection_kwargs(payload: dict[str, object]) -> dict[str, object]:
    host = str(payload.get("host") or os.getenv("PGHOST") or "127.0.0.1").strip()
    database = str(payload.get("database") or os.getenv("PGDATABASE") or "my_convolution").strip()
    user = str(payload.get("user") or os.getenv("PGUSER") or DEFAULT_DB_USER).strip()
    password = str(payload.get("password") or os.getenv("PGPASSWORD") or DEFAULT_DB_PASSWORD)
    sslmode = str(payload.get("sslmode") or os.getenv("PGSSLMODE") or "prefer").strip()

    try:
        port = int(payload.get("port") or os.getenv("PGPORT") or 5432)
    except (TypeError, ValueError) as error:
        raise ViewerError("Порт PostgreSQL должен быть целым числом") from error

    if not host or not database or not user:
        raise ViewerError("Хост, база данных и пользователь обязательны")
    if not 1 <= port <= 65_535:
        raise ViewerError("Порт PostgreSQL должен быть в диапазоне 1..65535")
    if sslmode not in SSL_MODES:
        raise ViewerError("Неизвестный режим SSL")

    connect_kwargs: dict[str, object] = {
        "host": host,
        "port": port,
        "dbname": database,
        "user": user,
        "sslmode": sslmode,
        "connect_timeout": 5,
        "application_name": "convolution_3d_viewer",
        "options": "-c default_transaction_read_only=on -c statement_timeout=60000",
        "row_factory": dict_row,
    }
    if password:
        connect_kwargs["password"] = password
    return connect_kwargs


def _read_query(path: Path) -> str:
    try:
        query = path.read_text(encoding="utf-8").strip()
    except OSError as error:
        raise ViewerError(f"Не удалось прочитать {path.name}: {error}") from error
    if not query:
        raise ViewerError(f"{path.name} пуст")
    return query


def _execute_read_only(
    connect_kwargs: dict[str, object],
    query: str,
    parameters: tuple[object, ...] = (),
    *,
    max_rows: int,
) -> list[dict[str, object]]:
    """Execute exactly one statement inside an already-pinned read-only snapshot."""

    if max_rows <= 0:
        raise ValueError("max_rows must be positive")

    try:
        with psycopg.connect(**connect_kwargs) as connection:
            with connection.transaction():
                with connection.cursor() as cursor:
                    # Fix the transaction mode before running any user-configurable SQL.
                    # The harmless SELECT establishes the transaction snapshot, so the
                    # following statement cannot switch the transaction back to READ WRITE.
                    cursor.execute("SET TRANSACTION READ ONLY")
                    cursor.execute("SELECT 1", (), prepare=True)

                    # The extended protocol accepts exactly one statement.  This prevents
                    # query.sql/--query from ending the protected transaction and appending
                    # another command in a new transaction.
                    cursor.execute(query, parameters, prepare=True)
                    rows = cursor.fetchmany(max_rows + 1)
                    if len(rows) > max_rows:
                        raise ViewerError(
                            f"Запрос вернул больше {max_rows:,} строк; "
                            "уточните условия выборки"
                        )
                    return rows
    except psycopg.Error as error:
        primary = getattr(error.diag, "message_primary", None)
        raise ViewerError(f"PostgreSQL: {primary or str(error)}") from error


def _catalog_response(
    rows: list[dict[str, object]],
    source: str,
) -> dict[str, object]:
    records, errors = build_catalog(rows)
    return {
        "ok": True,
        "source": source,
        "records": records,
        "errors": errors,
        "summary": {
            "records": len(records),
            "posts": len({item["post"] for item in records}),
            "types": len({item["dataType"] for item in records}),
            "skipped": len(errors),
        },
    }


def load_database_session(
    payload: dict[str, object],
    settings: ViewerSettings,
) -> tuple[dict[str, object], dict[str, object]]:
    """Load catalog metadata and return its private connection configuration."""

    connect_kwargs = _connection_kwargs(payload)
    query = _read_query(settings.query_path)
    rows = _execute_read_only(
        connect_kwargs,
        query,
        max_rows=MAX_CATALOG_RECORDS,
    )
    return _catalog_response(rows, "postgresql"), connect_kwargs


def load_from_database(
    payload: dict[str, object],
    settings: ViewerSettings,
) -> dict[str, object]:
    """Compatibility wrapper used by tests and non-HTTP callers."""

    response, _connect_kwargs_private = load_database_session(payload, settings)
    return response


def fetch_surface_from_database(
    connect_kwargs: dict[str, object],
    record_id: int,
    settings: ViewerSettings,
    catalog_record: dict[str, object] | None = None,
) -> dict[str, object]:
    rows = _execute_read_only(
        connect_kwargs,
        SURFACE_BY_ID_QUERY,
        (record_id,),
        max_rows=1,
    )
    if not rows:
        raise ViewerError(f"Свёртка id={record_id} больше не найдена в базе данных")
    if len(rows) != 1:
        raise ViewerError(f"Для id={record_id} база данных вернула несколько строк")

    surface = _surface_payload(rows[0], settings)
    if catalog_record is not None:
        for key in ("convBytes", "historyPosition", "historyCount"):
            if key in catalog_record:
                surface[key] = catalog_record[key]
    return surface


def load_demo(
    settings: ViewerSettings,
    rows: list[dict[str, object]] | None = None,
) -> dict[str, object]:
    del settings  # kept in the public signature for backwards compatibility
    return _catalog_response(rows if rows is not None else _demo_rows(), "demo")


def connection_defaults() -> dict[str, object]:
    environment_password = os.getenv("PGPASSWORD")
    return {
        "host": os.getenv("PGHOST") or "127.0.0.1",
        "port": int(os.getenv("PGPORT") or 5432),
        "database": os.getenv("PGDATABASE") or "my_convolution",
        "user": os.getenv("PGUSER") or DEFAULT_DB_USER,
        # Keep an environment-supplied password in the Python process.  The
        # requested built-in default is safe to prefill because it already lives
        # in this local source file.
        "password": "" if environment_password is not None else DEFAULT_DB_PASSWORD,
        "sslmode": os.getenv("PGSSLMODE") or "prefer",
        "passwordFromEnvironment": environment_password is not None,
    }


class ViewerHTTPServer(ThreadingHTTPServer):
    daemon_threads = True
    allow_reuse_address = True

    def __init__(
        self,
        server_address: tuple[str, int],
        handler_class: type[BaseHTTPRequestHandler],
        *,
        token: str,
        settings: ViewerSettings,
    ) -> None:
        super().__init__(server_address, handler_class)
        self.token = token
        self.settings = settings
        self._plotly_gzip: bytes | None = None
        self._state_lock = threading.RLock()
        self._load_generation = 0
        self._session_id: str | None = None
        self._source: str | None = None
        self._catalog_by_id: dict[int, dict[str, object]] = {}
        self._connection: dict[str, object] | None = None
        self._demo_rows_by_id: dict[int, dict[str, object]] = {}

    def plotly_gzip(self) -> bytes:
        if self._plotly_gzip is None:
            self._plotly_gzip = gzip.compress(get_plotlyjs().encode("utf-8"), compresslevel=6)
        return self._plotly_gzip

    def begin_catalog_load(self) -> int:
        """Reserve the only ticket allowed to publish the next catalog."""

        with self._state_lock:
            self._load_generation += 1
            return self._load_generation

    def _validate_load_ticket_locked(self, load_ticket: int) -> None:
        if type(load_ticket) is not int or load_ticket != self._load_generation:
            raise ViewerError("Загрузка каталога устарела; используется более новый запрос")

    def activate_postgresql(
        self,
        connect_kwargs: dict[str, object],
        records: list[dict[str, object]],
        load_ticket: int,
    ) -> str:
        """Atomically replace the active catalog without retaining any BYTEAs."""

        if not records:
            raise ViewerError("Запрос не вернул ни одной пригодной свёртки")
        catalog = {int(record["id"]): dict(record) for record in records}
        with self._state_lock:
            self._validate_load_ticket_locked(load_ticket)
            session_id = secrets.token_urlsafe(24)
            self._source = "postgresql"
            self._session_id = session_id
            self._catalog_by_id = catalog
            self._connection = dict(connect_kwargs)
            self._demo_rows_by_id = {}
        return session_id

    def activate_demo(
        self,
        rows: list[dict[str, object]],
        records: list[dict[str, object]],
        load_ticket: int,
    ) -> str:
        if not records:
            raise ViewerError("Демо-набор не содержит пригодных свёрток")
        catalog = {int(record["id"]): dict(record) for record in records}
        allowed_ids = set(catalog)
        demo_rows = {
            int(row["id"]): row
            for row in rows
            if int(row["id"]) in allowed_ids
        }
        with self._state_lock:
            self._validate_load_ticket_locked(load_ticket)
            session_id = secrets.token_urlsafe(24)
            self._source = "demo"
            self._session_id = session_id
            self._catalog_by_id = catalog
            self._connection = None
            self._demo_rows_by_id = demo_rows
        return session_id

    def _validate_session_locked(self, session_id: str) -> None:
        current_session_id = self._session_id
        if current_session_id is None:
            raise ViewerError("Сначала загрузите каталог свёрток")
        if not secrets.compare_digest(session_id, current_session_id):
            raise ViewerError("Каталог свёрток устарел; повторите выбор")

    def surface(
        self,
        record_id: int,
        session_id: str,
    ) -> tuple[str, dict[str, object]]:
        """Decode one selected catalog record; never hold the state lock during I/O."""

        with self._state_lock:
            self._validate_session_locked(session_id)
            source = self._source
            catalog_record = self._catalog_by_id.get(record_id)
            connection = dict(self._connection) if self._connection is not None else None
            demo_row = self._demo_rows_by_id.get(record_id)

        if catalog_record is None:
            raise ViewerError(f"Свёртка id={record_id} отсутствует в загруженном каталоге")

        if source == "demo":
            if demo_row is None:
                raise ViewerError(f"Демо-свёртка id={record_id} не найдена")
            surface = _surface_payload(demo_row, self.settings)
            for key in ("convBytes", "historyPosition", "historyCount"):
                surface[key] = catalog_record[key]
        elif source == "postgresql" and connection is not None:
            surface = fetch_surface_from_database(
                connection,
                record_id,
                self.settings,
                catalog_record,
            )
        else:
            raise ViewerError("Источник данных просмотрщика повреждён; загрузите каталог заново")

        surface["id"] = str(record_id)

        # A database fetch or demo decode can overlap with a new /api/load.  Do
        # not leak the now-stale result into the replacement catalog's UI.
        with self._state_lock:
            self._validate_session_locked(session_id)
        return source, surface


class ViewerHandler(BaseHTTPRequestHandler):
    server: ViewerHTTPServer

    def log_message(self, fmt: str, *args: object) -> None:
        return

    def _token_is_valid(self) -> bool:
        parsed = urlparse(self.path)
        query = parse_qs(parsed.query)
        supplied = query.get("token", [""])[0]
        return secrets.compare_digest(supplied, self.server.token)

    def _common_headers(self) -> None:
        self.send_header("X-Content-Type-Options", "nosniff")
        self.send_header("Referrer-Policy", "no-referrer")
        self.send_header("X-Frame-Options", "DENY")

    def _send_bytes(
        self,
        status: int,
        body: bytes,
        content_type: str,
        *,
        content_encoding: str | None = None,
        cache_control: str = "no-store",
    ) -> None:
        self.send_response(status)
        self._common_headers()
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", cache_control)
        if content_encoding:
            self.send_header("Content-Encoding", content_encoding)
        self.end_headers()
        self.wfile.write(body)

    def _send_json(self, status: int, payload: object) -> None:
        body = json.dumps(
            payload,
            ensure_ascii=False,
            allow_nan=False,
            separators=(",", ":"),
            default=lambda value: float(value) if isinstance(value, Decimal) else str(value),
        ).encode("utf-8")
        accept_encoding = self.headers.get("Accept-Encoding", "")
        if len(body) > 8_192 and "gzip" in accept_encoding:
            body = gzip.compress(body, compresslevel=5)
            self._send_bytes(
                status,
                body,
                "application/json; charset=utf-8",
                content_encoding="gzip",
            )
            return
        self._send_bytes(status, body, "application/json; charset=utf-8")

    def _read_json_body(self) -> dict[str, object]:
        try:
            length = int(self.headers.get("Content-Length", "0"))
        except ValueError as error:
            raise ViewerError("Некорректный Content-Length") from error
        if length <= 0 or length > MAX_REQUEST_BYTES:
            raise ViewerError("Некорректный размер запроса")
        try:
            payload = json.loads(self.rfile.read(length).decode("utf-8"))
        except (UnicodeDecodeError, json.JSONDecodeError) as error:
            raise ViewerError("Ожидался JSON-запрос") from error
        if not isinstance(payload, dict):
            raise ViewerError("Ожидался JSON-объект")
        return payload

    def do_GET(self) -> None:  # noqa: N802 - BaseHTTPRequestHandler API
        if not self._token_is_valid():
            self._send_json(403, {"ok": False, "error": "Недействительный токен приложения"})
            return

        path = urlparse(self.path).path
        if path == "/":
            try:
                template = INDEX_PATH.read_text(encoding="utf-8")
            except OSError as error:
                self._send_json(500, {"ok": False, "error": str(error)})
                return
            html = template.replace("__APP_TOKEN__", self.server.token)
            self.send_response(200)
            self._common_headers()
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Security-Policy", "default-src 'self'; script-src 'self' 'unsafe-inline'; style-src 'self' 'unsafe-inline'; img-src 'self' data: blob:; connect-src 'self'; font-src 'self'")
            body = html.encode("utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(body)
            return

        if path == "/plotly.min.js":
            self._send_bytes(
                200,
                self.server.plotly_gzip(),
                "application/javascript; charset=utf-8",
                content_encoding="gzip",
                cache_control="private, max-age=86400",
            )
            return

        if path == "/api/defaults":
            self._send_json(
                200,
                {
                    "ok": True,
                    "defaults": connection_defaults(),
                    "demoOnStart": self.server.settings.demo_on_start,
                    "queryName": self.server.settings.query_path.name,
                },
            )
            return

        if path == "/api/demo":
            try:
                load_ticket = self.server.begin_catalog_load()
                rows = _demo_rows()
                result = load_demo(self.server.settings, rows)
                records = result["records"]
                assert isinstance(records, list)
                session_id = self.server.activate_demo(rows, records, load_ticket)
                result["sessionId"] = session_id
            except (ViewerError, AssertionError) as error:
                self._send_json(500, {"ok": False, "error": str(error)})
                return
            self._send_json(200, result)
            return

        self._send_json(404, {"ok": False, "error": "Не найдено"})

    def do_POST(self) -> None:  # noqa: N802 - BaseHTTPRequestHandler API
        if not self._token_is_valid():
            self._send_json(403, {"ok": False, "error": "Недействительный токен приложения"})
            return

        path = urlparse(self.path).path
        if path not in {"/api/load", "/api/surface"}:
            self._send_json(404, {"ok": False, "error": "Не найдено"})
            return

        try:
            payload = self._read_json_body()
            if path == "/api/load":
                load_ticket = self.server.begin_catalog_load()
                result, connect_kwargs = load_database_session(
                    payload,
                    self.server.settings,
                )
                records = result["records"]
                assert isinstance(records, list)
                session_id = self.server.activate_postgresql(
                    connect_kwargs,
                    records,
                    load_ticket,
                )
                result["sessionId"] = session_id
            else:
                record_id = _parse_record_id(payload.get("id"))
                raw_session_id = payload.get("sessionId")
                if type(raw_session_id) is not str or not raw_session_id:
                    raise ViewerError("Поле sessionId должно быть непустой строкой")
                session_id = raw_session_id
                source, surface = self.server.surface(record_id, session_id)
                result = {
                    "ok": True,
                    "source": source,
                    "sessionId": session_id,
                    "surface": surface,
                }
        except ViewerError as error:
            self._send_json(400, {"ok": False, "error": str(error)})
            return
        except Exception as error:  # keep the local UI alive on unexpected row issues
            self._send_json(500, {"ok": False, "error": f"Внутренняя ошибка: {error}"})
            return
        self._send_json(200, result)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Автономный 3D-просмотрщик свёрток из PostgreSQL"
    )
    parser.add_argument("--host", default="127.0.0.1", help="loopback-адрес web-сервера")
    parser.add_argument("--port", type=int, default=0, help="порт web-сервера (0 = выбрать автоматически)")
    parser.add_argument("--query", type=Path, default=DEFAULT_QUERY_PATH, help="путь к SELECT-запросу")
    parser.add_argument("--max-h", type=int, default=480, help="максимум отсчётов H на графике")
    parser.add_argument("--max-v", type=int, default=180, help="максимум отсчётов V на графике")
    parser.add_argument("--demo", action="store_true", help="автоматически открыть демо-данные")
    parser.add_argument("--no-browser", action="store_true", help="не открывать браузер автоматически")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if args.host not in {"127.0.0.1", "localhost", "::1"}:
        raise SystemExit("Из соображений безопасности разрешён только loopback-адрес")
    if not 0 <= args.port <= 65_535:
        raise SystemExit("Порт должен быть в диапазоне 0..65535")
    if args.max_h < 16 or args.max_v < 16:
        raise SystemExit("--max-h и --max-v должны быть не меньше 16")

    settings = ViewerSettings(
        query_path=args.query.resolve(),
        max_h_samples=args.max_h,
        max_v_samples=args.max_v,
        demo_on_start=args.demo,
    )
    token = secrets.token_urlsafe(24)
    server = ViewerHTTPServer(
        (args.host, args.port),
        ViewerHandler,
        token=token,
        settings=settings,
    )
    actual_host, actual_port = server.server_address[:2]
    display_host = "127.0.0.1" if actual_host in {"0.0.0.0", "::"} else actual_host
    url = f"http://{display_host}:{actual_port}/?token={token}"

    print("3D-просмотрщик свёрток запущен локально:")
    print(url)
    print("Остановить: Ctrl+C")
    if not args.no_browser:
        threading.Timer(0.35, lambda: webbrowser.open(url)).start()

    try:
        server.serve_forever(poll_interval=0.25)
    except KeyboardInterrupt:
        print("\nПросмотрщик остановлен.")
    finally:
        server.server_close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
