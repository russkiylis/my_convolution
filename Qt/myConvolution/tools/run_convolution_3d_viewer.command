#!/bin/zsh
set -eu

script_dir="${0:A:h}"
tool_dir="$script_dir/convolution_3d_viewer"
viewer_venv="$tool_dir/.venv"

if [[ ! -x "$viewer_venv/bin/python" ]]; then
    print "Первый запуск: создаю отдельное окружение 3D-просмотрщика..."
    python3 -m venv "$viewer_venv"
fi

if ! "$viewer_venv/bin/python" -c "import numpy, plotly, psycopg" >/dev/null 2>&1; then
    print "Устанавливаю локальные зависимости 3D-просмотрщика..."
    if command -v uv >/dev/null 2>&1; then
        UV_CACHE_DIR="$viewer_venv/.uv-cache" \
            uv pip install --python "$viewer_venv/bin/python" -r "$tool_dir/requirements.txt"
    else
        "$viewer_venv/bin/python" -m pip install --no-cache-dir -r "$tool_dir/requirements.txt"
    fi
fi

exec "$viewer_venv/bin/python" "$tool_dir/app.py" "$@"
