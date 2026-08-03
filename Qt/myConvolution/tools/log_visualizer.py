#!/usr/bin/env python3
"""Standalone browser visualizer for generator log.txt files.

The script uses only the Python standard library and does not depend on, import,
or modify myConvolution.  Pass a log path explicitly or let the script select
the newest log.txt under the project's build directories.
"""

from __future__ import annotations

import argparse
from collections import OrderedDict
from dataclasses import dataclass, field
import json
import math
from pathlib import Path
import re
import secrets
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import threading
import webbrowser


PROJECT_ROOT = Path(__file__).resolve().parent.parent
LOG_LINE_RE = re.compile(
    r"^post=(.*?)\tlevel=([^\t]+)\ttimestamp=QDateTime\((.*?)\)"
    r"\tconvH=std::vector\((.*)\)\s*$"
)
TIMESTAMP_RE = re.compile(
    r"^(\d{4}-\d{2}-\d{2}) (\d{2}:\d{2}:\d{2}(?:\.\d+)?) UTC(?: Qt::UTC)?$"
)


def json_bytes(payload: object) -> bytes:
    return json.dumps(
        payload,
        ensure_ascii=False,
        allow_nan=False,
        separators=(",", ":"),
    ).encode("utf-8")


def timestamp_to_iso(value: str) -> str:
    match = TIMESTAMP_RE.fullmatch(value.strip())
    if match is None:
        return value.strip()
    return f"{match.group(1)}T{match.group(2)}Z"


def parse_finite_float(value: str) -> float:
    result = float(value)
    if not math.isfinite(result):
        raise ValueError("non-finite number")
    return result


def parse_log_line(line: str) -> dict[str, object] | None:
    match = LOG_LINE_RE.fullmatch(line.rstrip("\r\n"))
    if match is None:
        return None

    post_name = match.group(1).strip()
    if not post_name:
        raise ValueError("empty post name")

    convolution = [
        parse_finite_float(item.strip())
        for item in match.group(4).split(",")
        if item.strip()
    ]
    if not convolution:
        raise ValueError("empty convH")

    timestamp_raw = match.group(3).strip()
    return {
        "post": post_name,
        "level": parse_finite_float(match.group(2).strip()),
        "timestamp": timestamp_to_iso(timestamp_raw),
        "timestampRaw": timestamp_raw,
        "convH": convolution,
    }


@dataclass
class PostData:
    readings: list[dict[str, object]] = field(default_factory=list)
    latest: dict[str, object] | None = None


class LogSnapshot:
    """Incrementally parses appended complete lines and caches the result."""

    def __init__(self, path: Path):
        self.path = path.resolve(strict=False)
        self._lock = threading.Lock()
        self._identity: tuple[int, int] | None = None
        self._offset = 0
        self._posts: OrderedDict[str, PostData] = OrderedDict()
        self._ignored_lines = 0
        self._malformed_lines = 0

    def reset(self) -> None:
        with self._lock:
            self._reset_unlocked()

    def _reset_unlocked(self) -> None:
        self._identity = None
        self._offset = 0
        self._posts.clear()
        self._ignored_lines = 0
        self._malformed_lines = 0

    def _consume_line(self, line: str) -> None:
        try:
            record = parse_log_line(line)
        except (TypeError, ValueError):
            self._malformed_lines += 1
            return

        if record is None:
            self._ignored_lines += 1
            return

        post_name = str(record["post"])
        post = self._posts.setdefault(post_name, PostData())
        post.readings.append(
            {
                "timestamp": record["timestamp"],
                "timestampRaw": record["timestampRaw"],
                "level": record["level"],
            }
        )
        post.latest = record

    def _refresh_unlocked(self) -> tuple[bool, int, int]:
        try:
            stat = self.path.stat()
        except FileNotFoundError:
            self._reset_unlocked()
            return False, 0, 0

        identity = (stat.st_dev, stat.st_ino)
        if self._identity != identity or stat.st_size < self._offset:
            self._reset_unlocked()
            self._identity = identity

        with self.path.open("rb") as handle:
            handle.seek(self._offset)
            chunk = handle.read()

        if chunk:
            last_newline = chunk.rfind(b"\n")
            if last_newline >= 0:
                complete = chunk[: last_newline + 1]
                self._offset += last_newline + 1
                text = complete.decode("utf-8", errors="replace")
                for line in text.splitlines():
                    self._consume_line(line)

        return True, stat.st_size, stat.st_mtime_ns

    def snapshot(self) -> dict[str, object]:
        with self._lock:
            try:
                exists, size, modified_ns = self._refresh_unlocked()
            except OSError as error:
                return {
                    "ok": False,
                    "exists": self.path.exists(),
                    "path": str(self.path),
                    "error": str(error),
                    "posts": [],
                }

            posts = []
            total_records = 0
            for name, post in self._posts.items():
                if post.latest is None:
                    continue
                total_records += len(post.readings)
                posts.append(
                    {
                        "name": name,
                        "readings": post.readings,
                        "latest": {
                            "timestamp": post.latest["timestamp"],
                            "timestampRaw": post.latest["timestampRaw"],
                            "level": post.latest["level"],
                            "convH": post.latest["convH"],
                        },
                    }
                )

            return {
                "ok": True,
                "exists": exists,
                "path": str(self.path),
                "size": size,
                "modifiedNs": modified_ns,
                "totalRecords": total_records,
                "ignoredLines": self._ignored_lines,
                "malformedLines": self._malformed_lines,
                "posts": posts,
            }

    def delete(self) -> dict[str, object]:
        with self._lock:
            try:
                self.path.unlink()
            except FileNotFoundError:
                self._reset_unlocked()
                return {"ok": True, "deleted": False, "message": "Файл уже отсутствует."}
            except OSError as error:
                return {"ok": False, "deleted": False, "message": str(error)}

            self._reset_unlocked()
            return {"ok": True, "deleted": True, "message": "log.txt удалён."}


APP_HTML = r"""<!doctype html>
<html lang="ru">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Визуализатор генератора</title>
  <style>
    :root {
      color-scheme: light dark;
      --page: #f4f6f9;
      --surface: #ffffff;
      --surface-soft: #f8fafc;
      --text: #172033;
      --muted: #667085;
      --border: #d7dce5;
      --grid: #e5e9f0;
      --primary: #3168d5;
      --danger: #c73535;
      --danger-hover: #a92525;
      --button: #e8edf5;
      --button-hover: #dce4ef;
      --shadow: 0 10px 30px rgba(29, 41, 57, 0.08);
    }
    @media (prefers-color-scheme: dark) {
      :root {
        --page: #11151d;
        --surface: #1a202b;
        --surface-soft: #151b24;
        --text: #edf2f7;
        --muted: #a7b0c0;
        --border: #343d4c;
        --grid: #2b3442;
        --primary: #77a3ff;
        --danger: #c94b4b;
        --danger-hover: #dd6262;
        --button: #283140;
        --button-hover: #333e50;
        --shadow: 0 10px 30px rgba(0, 0, 0, 0.24);
      }
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      background: var(--page);
      color: var(--text);
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
    }
    main { width: min(1440px, 100%); margin: 0 auto; padding: 24px; }
    header {
      display: flex;
      align-items: flex-start;
      justify-content: space-between;
      gap: 20px;
      margin-bottom: 18px;
    }
    h1 { margin: 0 0 6px; font-size: clamp(23px, 3vw, 34px); font-weight: 650; }
    h2 { margin: 0; font-size: 19px; font-weight: 650; }
    h3 { margin: 0; font-size: 16px; font-weight: 650; }
    p { margin: 0; }
    .path { color: var(--muted); overflow-wrap: anywhere; font-size: 13px; }
    .toolbar { display: flex; gap: 10px; flex-wrap: wrap; justify-content: flex-end; }
    button {
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 9px 13px;
      background: var(--button);
      color: var(--text);
      font: inherit;
      font-weight: 600;
      cursor: pointer;
    }
    button:hover { background: var(--button-hover); }
    button:focus-visible { outline: 3px solid color-mix(in srgb, var(--primary), transparent 55%); outline-offset: 2px; }
    button:disabled { opacity: 0.45; cursor: not-allowed; }
    button.danger { background: var(--danger); border-color: var(--danger); color: white; }
    button.danger:hover:not(:disabled) { background: var(--danger-hover); }
    .status-row {
      display: flex;
      gap: 8px 20px;
      flex-wrap: wrap;
      color: var(--muted);
      font-size: 13px;
      margin-bottom: 18px;
    }
    .status-row strong { color: var(--text); }
    .notice {
      padding: 14px 16px;
      border: 1px solid var(--border);
      border-radius: 10px;
      background: var(--surface);
      color: var(--muted);
      margin-bottom: 18px;
    }
    .notice.error { color: var(--danger); }
    .overview, .post-card {
      border: 1px solid var(--border);
      border-radius: 12px;
      background: var(--surface);
      box-shadow: var(--shadow);
    }
    .overview { padding: 18px; margin-bottom: 18px; }
    .section-head { display: flex; justify-content: space-between; align-items: baseline; gap: 12px; margin-bottom: 12px; }
    .section-head span, .chart-value { color: var(--muted); font-size: 13px; }
    .legend { display: flex; gap: 8px 18px; flex-wrap: wrap; margin: 10px 0 0; font-size: 13px; color: var(--muted); }
    .legend-item { display: inline-flex; align-items: center; gap: 6px; }
    .legend-line { width: 19px; height: 3px; border-radius: 2px; }
    canvas { display: block; width: 100%; height: 240px; background: var(--surface-soft); border-radius: 8px; }
    .posts { display: grid; grid-template-columns: repeat(auto-fit, minmax(min(520px, 100%), 1fr)); gap: 18px; }
    .post-card { padding: 18px; min-width: 0; }
    .post-meta { display: flex; gap: 8px 18px; flex-wrap: wrap; margin: 8px 0 14px; color: var(--muted); font-size: 13px; }
    .post-meta strong { color: var(--text); }
    .chart-block + .chart-block { margin-top: 18px; }
    .chart-title { display: flex; justify-content: space-between; gap: 12px; align-items: baseline; margin-bottom: 7px; }
    .sr-only { position: absolute; width: 1px; height: 1px; padding: 0; margin: -1px; overflow: hidden; clip: rect(0, 0, 0, 0); white-space: nowrap; border: 0; }
    @media (max-width: 700px) {
      main { padding: 16px; }
      header { flex-direction: column; }
      .toolbar { width: 100%; justify-content: flex-start; }
      canvas { height: 210px; }
    }
  </style>
</head>
<body>
<main>
  <header>
    <div>
      <h1>Данные генератора</h1>
      <p id="logPath" class="path">Определение пути…</p>
    </div>
    <div class="toolbar" aria-label="Действия с журналом">
      <button id="refreshButton" type="button">Обновить</button>
      <button id="deleteButton" class="danger" type="button" disabled>Удалить log.txt</button>
    </div>
  </header>

  <div id="statusRow" class="status-row" aria-live="polite"></div>
  <div id="notice" class="notice">Чтение данных…</div>

  <section id="overview" class="overview" hidden aria-labelledby="overviewTitle">
    <div class="section-head">
      <h2 id="overviewTitle">Уровень всех постов</h2>
      <span id="overviewValue">Наведите курсор на график</span>
    </div>
    <canvas id="overviewCanvas" role="img" aria-label="Изменение уровня всех постов во времени"></canvas>
    <div id="overviewLegend" class="legend"></div>
  </section>

  <section id="posts" class="posts" aria-label="Графики постов"></section>
</main>

<script>
(() => {
  "use strict";
  const TOKEN = "__TOKEN__";
  const palette = ["#3478e5", "#e27828", "#2d9b68", "#a25bd6", "#d24f7a", "#2d9db0", "#8b7a2b", "#7b8798"];
  const state = { data: null, charts: [], loading: false };
  const elements = {
    logPath: document.getElementById("logPath"),
    refresh: document.getElementById("refreshButton"),
    remove: document.getElementById("deleteButton"),
    status: document.getElementById("statusRow"),
    notice: document.getElementById("notice"),
    overview: document.getElementById("overview"),
    overviewCanvas: document.getElementById("overviewCanvas"),
    overviewLegend: document.getElementById("overviewLegend"),
    overviewValue: document.getElementById("overviewValue"),
    posts: document.getElementById("posts")
  };

  function css(name) {
    return getComputedStyle(document.documentElement).getPropertyValue(name).trim();
  }

  function formatBytes(value) {
    if (!Number.isFinite(value)) return "—";
    const units = ["Б", "КБ", "МБ", "ГБ"];
    let size = value;
    let unit = 0;
    while (size >= 1024 && unit < units.length - 1) { size /= 1024; unit += 1; }
    return `${size.toLocaleString("ru-RU", { maximumFractionDigits: unit ? 1 : 0 })} ${units[unit]}`;
  }

  function formatTime(value) {
    const date = new Date(value);
    if (Number.isNaN(date.valueOf())) return String(value).replace(" UTC Qt::UTC", " UTC");
    return date.toLocaleTimeString("ru-RU", { hour: "2-digit", minute: "2-digit", second: "2-digit", fractionalSecondDigits: 3, timeZone: "UTC" }) + " UTC";
  }

  function colorFor(index) { return palette[index % palette.length]; }

  function extent(values, equalPad = null) {
    let min = Infinity;
    let max = -Infinity;
    values.forEach(value => {
      if (Number.isFinite(value)) { min = Math.min(min, value); max = Math.max(max, value); }
    });
    if (!Number.isFinite(min)) return [0, 1];
    if (min === max) {
      const pad = equalPad ?? Math.max(Math.abs(min) * 0.08, 0.5);
      return [min - pad, max + pad];
    }
    const pad = (max - min) * 0.08;
    return [min - pad, max + pad];
  }

  function fitCanvas(canvas) {
    const rect = canvas.getBoundingClientRect();
    const ratio = Math.max(1, window.devicePixelRatio || 1);
    const width = Math.max(1, Math.round(rect.width));
    const height = Math.max(180, Math.round(rect.height));
    const pixelWidth = Math.round(width * ratio);
    const pixelHeight = Math.round(height * ratio);
    if (canvas.width !== pixelWidth || canvas.height !== pixelHeight) {
      canvas.width = pixelWidth;
      canvas.height = pixelHeight;
    }
    const context = canvas.getContext("2d");
    context.setTransform(ratio, 0, 0, ratio, 0, 0);
    return { context, width, height };
  }

  function drawLineChart(canvas, series, options = {}) {
    const { context: ctx, width, height } = fitCanvas(canvas);
    const margin = { left: 54, right: 18, top: 18, bottom: 38 };
    const plotWidth = Math.max(1, width - margin.left - margin.right);
    const plotHeight = Math.max(1, height - margin.top - margin.bottom);
    const allPoints = series.flatMap(item => item.points);
    const xValues = allPoints.map(point => point[0]);
    const yValues = allPoints.map(point => point[1]);
    let xMin;
    let xMax;
    if (options.xFromZero) {
      const finiteX = xValues.filter(Number.isFinite);
      xMin = 0;
      xMax = finiteX.length ? Math.max(...finiteX) : 1;
      if (xMax <= xMin) xMax = xMin + 1;
    } else {
      [xMin, xMax] = extent(xValues, options.timeX ? 1000 : 0.5);
    }
    const [yMin, yMax] = options.fixedY || extent(yValues);
    const xScale = value => margin.left + ((value - xMin) / Math.max(xMax - xMin, 1e-12)) * plotWidth;
    const yScale = value => margin.top + (1 - (value - yMin) / Math.max(yMax - yMin, 1e-12)) * plotHeight;

    ctx.clearRect(0, 0, width, height);
    ctx.fillStyle = css("--surface-soft");
    ctx.fillRect(0, 0, width, height);
    ctx.font = "12px -apple-system, BlinkMacSystemFont, Segoe UI, sans-serif";
    ctx.textBaseline = "middle";

    for (let i = 0; i <= 4; i += 1) {
      const y = margin.top + (plotHeight * i) / 4;
      const value = yMax - ((yMax - yMin) * i) / 4;
      ctx.strokeStyle = css("--grid");
      ctx.lineWidth = 1;
      ctx.beginPath(); ctx.moveTo(margin.left, y); ctx.lineTo(width - margin.right, y); ctx.stroke();
      ctx.fillStyle = css("--muted");
      ctx.textAlign = "right";
      ctx.fillText(value.toFixed(options.yDigits ?? 2), margin.left - 8, y);
    }

    const tickCount = options.timeX ? (width < 800 ? 2 : 4) : (width < 520 ? 2 : 4);
    for (let i = 0; i <= tickCount; i += 1) {
      const ratio = i / tickCount;
      const x = margin.left + plotWidth * ratio;
      const value = xMin + (xMax - xMin) * ratio;
      ctx.fillStyle = css("--muted");
      ctx.textAlign = i === 0 ? "left" : i === tickCount ? "right" : "center";
      ctx.fillText(options.formatX ? options.formatX(value) : Math.round(value).toLocaleString("ru-RU"), x, height - 17);
    }

    series.forEach(item => {
      if (!item.points.length) return;
      ctx.strokeStyle = item.color;
      ctx.lineWidth = 1.8;
      ctx.lineJoin = "round";
      ctx.lineCap = "round";
      ctx.beginPath();
      item.points.forEach((point, index) => {
        const x = xScale(point[0]);
        const y = yScale(point[1]);
        if (index === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
      });
      if (item.points.length === 1) {
        ctx.arc(xScale(item.points[0][0]), yScale(item.points[0][1]), 3, 0, Math.PI * 2);
      }
      ctx.stroke();
    });

    canvas._chartInfo = { series, margin, plotWidth, xMin, xMax, valueElement: options.valueElement, valueLabel: options.valueLabel || "" };
  }

  function installReadout(canvas) {
    canvas.addEventListener("mousemove", event => {
      const info = canvas._chartInfo;
      if (!info || !info.valueElement || !info.series.length) return;
      const rect = canvas.getBoundingClientRect();
      const localX = Math.max(info.margin.left, Math.min(rect.width - info.margin.right, event.clientX - rect.left));
      const targetX = info.xMin + ((localX - info.margin.left) / Math.max(info.plotWidth, 1)) * (info.xMax - info.xMin);
      let closest = null;
      info.series.forEach(series => series.points.forEach(point => {
        const distance = Math.abs(point[0] - targetX);
        if (closest === null || distance < closest.distance) closest = { distance, point, name: series.name };
      }));
      if (closest) {
        info.valueElement.textContent = `${closest.name}: ${closest.point[1].toFixed(4)}${info.valueLabel}`;
      }
    });
  }

  function makeElement(tag, className, text) {
    const element = document.createElement(tag);
    if (className) element.className = className;
    if (text !== undefined) element.textContent = text;
    return element;
  }

  function labeledValue(label, value) {
    const item = makeElement("span");
    item.append(document.createTextNode(`${label}: `), makeElement("strong", "", value));
    return item;
  }

  function render(data) {
    state.data = data;
    state.charts = [];
    elements.logPath.textContent = data.path || "Путь не определён";
    elements.remove.disabled = !data.exists;
    elements.posts.replaceChildren();
    elements.overviewLegend.replaceChildren();

    if (!data.ok) {
      elements.notice.hidden = false;
      elements.notice.className = "notice error";
      elements.notice.textContent = `Не удалось прочитать log.txt: ${data.error || "неизвестная ошибка"}`;
      elements.overview.hidden = true;
      elements.status.replaceChildren();
      return;
    }

    elements.status.replaceChildren(
      labeledValue("Файл", data.exists ? formatBytes(data.size) : "не найден"),
      labeledValue("Постов", String(data.posts.length)),
      labeledValue("Записей", String(data.totalRecords))
    );
    if (data.malformedLines) {
      elements.status.append(labeledValue("Повреждённых строк", String(data.malformedLines)));
    }

    if (!data.exists) {
      elements.notice.hidden = false;
      elements.notice.className = "notice";
      elements.notice.textContent = "log.txt пока отсутствует. Запустите генератор или укажите другой файл при запуске визуализатора.";
      elements.overview.hidden = true;
      return;
    }

    if (!data.posts.length) {
      elements.notice.hidden = false;
      elements.notice.className = "notice";
      elements.notice.textContent = "В файле пока нет строк генератора подходящего формата.";
      elements.overview.hidden = true;
      return;
    }

    elements.notice.hidden = true;
    elements.overview.hidden = false;
    const overviewSeries = data.posts.map((post, index) => ({
      name: post.name,
      color: colorFor(index),
      points: post.readings.map((reading, readingIndex) => {
        const parsed = Date.parse(reading.timestamp);
        return [Number.isFinite(parsed) ? parsed : readingIndex, reading.level];
      })
    }));
    data.posts.forEach((post, index) => {
      const item = makeElement("span", "legend-item");
      const line = makeElement("span", "legend-line");
      line.style.background = colorFor(index);
      item.append(line, document.createTextNode(post.name));
      elements.overviewLegend.append(item);
    });
    const overviewDraw = () => drawLineChart(elements.overviewCanvas, overviewSeries, {
      timeX: true,
      formatX: value => formatTime(new Date(value).toISOString()),
      valueElement: elements.overviewValue,
      valueLabel: " дБ"
    });
    state.charts.push(overviewDraw);
    installReadout(elements.overviewCanvas);

    data.posts.forEach((post, index) => {
      const color = colorFor(index);
      const card = makeElement("article", "post-card");
      const heading = makeElement("div", "section-head");
      heading.append(makeElement("h2", "", post.name), makeElement("span", "", `${post.readings.length} записей`));
      card.append(heading);

      const meta = makeElement("div", "post-meta");
      meta.append(
        labeledValue("Последний уровень", `${post.latest.level.toFixed(4)} дБ`),
        labeledValue("Время", formatTime(post.latest.timestamp)),
        labeledValue("Отсчётов convH", post.latest.convH.length.toLocaleString("ru-RU"))
      );
      card.append(meta);

      const levelBlock = makeElement("div", "chart-block");
      const levelTitle = makeElement("div", "chart-title");
      const levelValue = makeElement("span", "chart-value", "Наведите курсор на график");
      levelTitle.append(makeElement("h3", "", "Уровень во времени"), levelValue);
      const levelCanvas = document.createElement("canvas");
      levelCanvas.setAttribute("role", "img");
      levelCanvas.setAttribute("aria-label", `Уровень поста ${post.name} во времени`);
      levelBlock.append(levelTitle, levelCanvas);
      card.append(levelBlock);

      const convBlock = makeElement("div", "chart-block");
      const convTitle = makeElement("div", "chart-title");
      const convValue = makeElement("span", "chart-value", "Наведите курсор на график");
      convTitle.append(makeElement("h3", "", "Последняя горизонтальная свёртка"), convValue);
      const convCanvas = document.createElement("canvas");
      convCanvas.setAttribute("role", "img");
      convCanvas.setAttribute("aria-label", `Последняя горизонтальная свёртка поста ${post.name}`);
      convBlock.append(convTitle, convCanvas);
      card.append(convBlock);
      elements.posts.append(card);

      const levelSeries = [{
        name: post.name,
        color,
        points: post.readings.map((reading, readingIndex) => {
          const parsed = Date.parse(reading.timestamp);
          return [Number.isFinite(parsed) ? parsed : readingIndex, reading.level];
        })
      }];
      const convSeries = [{ name: post.name, color, points: post.latest.convH.map((value, pointIndex) => [pointIndex, value]) }];
      const levelDraw = () => drawLineChart(levelCanvas, levelSeries, {
        timeX: true,
        formatX: value => formatTime(new Date(value).toISOString()),
        valueElement: levelValue,
        valueLabel: " дБ"
      });
      const convDraw = () => drawLineChart(convCanvas, convSeries, {
        xFromZero: true,
        fixedY: [0, 1],
        yDigits: 2,
        valueElement: convValue
      });
      state.charts.push(levelDraw, convDraw);
      installReadout(levelCanvas);
      installReadout(convCanvas);
    });

    requestAnimationFrame(() => state.charts.forEach(draw => draw()));
  }

  async function loadData(showBusy = true) {
    if (state.loading) return;
    state.loading = true;
    if (showBusy) elements.refresh.disabled = true;
    try {
      const response = await fetch("/api/data", { cache: "no-store" });
      if (!response.ok) throw new Error(`HTTP ${response.status}`);
      const data = await response.json();
      const unchanged = !showBusy && state.data &&
        state.data.path === data.path &&
        state.data.exists === data.exists &&
        state.data.modifiedNs === data.modifiedNs &&
        state.data.size === data.size;
      if (!unchanged) render(data);
    } catch (error) {
      elements.notice.hidden = false;
      elements.notice.className = "notice error";
      elements.notice.textContent = `Ошибка обновления: ${error.message}`;
    } finally {
      state.loading = false;
      elements.refresh.disabled = false;
    }
  }

  elements.refresh.addEventListener("click", () => loadData(true));
  elements.remove.addEventListener("click", async () => {
    const path = state.data?.path || "log.txt";
    const confirmed = window.confirm(`Удалить файл?\n\n${path}\n\nЕсли генератор сейчас пишет в файл, сначала остановите его.`);
    if (!confirmed) return;
    elements.remove.disabled = true;
    try {
      const response = await fetch("/api/delete", {
        method: "POST",
        headers: { "X-Visualizer-Token": TOKEN }
      });
      const result = await response.json();
      if (!response.ok || !result.ok) throw new Error(result.message || `HTTP ${response.status}`);
      await loadData(false);
    } catch (error) {
      window.alert(`Не удалось удалить log.txt: ${error.message}`);
      elements.remove.disabled = !state.data?.exists;
    }
  });

  let resizeTimer = 0;
  window.addEventListener("resize", () => {
    window.clearTimeout(resizeTimer);
    resizeTimer = window.setTimeout(() => state.charts.forEach(draw => draw()), 100);
  });
  window.matchMedia("(prefers-color-scheme: dark)").addEventListener("change", () => state.charts.forEach(draw => draw()));

  loadData(true);
  window.setInterval(() => loadData(false), 2500);
})();
</script>
</body>
</html>
"""


def make_handler(snapshot: LogSnapshot, token: str) -> type[BaseHTTPRequestHandler]:
    class VisualizerHandler(BaseHTTPRequestHandler):
        def send_payload(self, status: int, payload: object) -> None:
            body = json_bytes(payload)
            self.send_response(status)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(body)

        def do_GET(self) -> None:  # noqa: N802 - BaseHTTPRequestHandler API
            path = self.path.partition("?")[0]
            if path == "/":
                body = APP_HTML.replace("__TOKEN__", token).encode("utf-8")
                self.send_response(200)
                self.send_header("Content-Type", "text/html; charset=utf-8")
                self.send_header("Content-Length", str(len(body)))
                self.send_header("Cache-Control", "no-store")
                self.send_header("X-Content-Type-Options", "nosniff")
                self.send_header("Content-Security-Policy", "default-src 'self'; script-src 'unsafe-inline'; style-src 'unsafe-inline'; connect-src 'self'; img-src 'self' data:; base-uri 'none'; frame-ancestors 'none'")
                self.end_headers()
                self.wfile.write(body)
                return
            if path == "/api/data":
                self.send_payload(200, snapshot.snapshot())
                return
            self.send_error(404)

        def do_POST(self) -> None:  # noqa: N802 - BaseHTTPRequestHandler API
            path = self.path.partition("?")[0]
            if path != "/api/delete":
                self.send_error(404)
                return
            if self.headers.get("X-Visualizer-Token") != token:
                self.send_payload(403, {"ok": False, "message": "Неверный защитный токен."})
                return
            result = snapshot.delete()
            self.send_payload(200 if result["ok"] else 500, result)

        def log_message(self, format: str, *args: object) -> None:
            return

    return VisualizerHandler


def discover_log(project_root: Path) -> Path:
    candidates = [project_root / "log.txt"]
    candidates.extend(project_root.glob("build/*/myConvolution.app/Contents/MacOS/log.txt"))
    candidates.extend(project_root.glob("build/*/log.txt"))
    existing = [path for path in candidates if path.is_file()]
    if existing:
        return max(existing, key=lambda path: path.stat().st_mtime_ns)
    return project_root / "log.txt"


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Показывает графики всех постов из log.txt и позволяет удалить этот файл."
    )
    parser.add_argument(
        "log",
        nargs="?",
        type=Path,
        help="Путь к log.txt. По умолчанию выбирается самый свежий файл из build/*.",
    )
    parser.add_argument("--host", default="127.0.0.1", help=argparse.SUPPRESS)
    parser.add_argument("--port", type=int, default=0, help="Порт сервера; 0 выбирает свободный порт.")
    parser.add_argument("--no-browser", action="store_true", help="Не открывать браузер автоматически.")
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    log_path = arguments.log.expanduser() if arguments.log else discover_log(PROJECT_ROOT)
    if not log_path.is_absolute():
        log_path = (Path.cwd() / log_path).resolve(strict=False)

    snapshot = LogSnapshot(log_path)
    token = secrets.token_urlsafe(24)
    server = ThreadingHTTPServer(
        (arguments.host, arguments.port),
        make_handler(snapshot, token),
    )
    address, port = server.server_address[:2]
    browser_host = "127.0.0.1" if address in {"0.0.0.0", "::"} else address
    url = f"http://{browser_host}:{port}/"

    print(f"Визуализатор: {url}")
    print(f"Файл: {snapshot.path}")
    print("Остановка: Ctrl+C")
    if not arguments.no_browser:
        threading.Timer(0.25, lambda: webbrowser.open(url)).start()

    try:
        server.serve_forever(poll_interval=0.25)
    except KeyboardInterrupt:
        print("\nВизуализатор остановлен.")
    finally:
        server.server_close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
