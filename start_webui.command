#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

echo "============================================"
echo "  RC Engine Sound ESP32 Configurator"
echo "============================================"
echo ""

# --- 1. Find Python 3 ---
PY=""
if [ -x ".venv/bin/python3" ]; then
  PY=".venv/bin/python3"
elif [ -x ".venv/bin/python" ]; then
  PY=".venv/bin/python"
elif command -v python3 >/dev/null 2>&1; then
  PY="python3"
elif command -v python >/dev/null 2>&1; then
  PY="python"
fi

if [ -z "$PY" ]; then
  echo ""
  echo "  ERROR: Python 3 is not installed."
  echo ""
  echo "  Install it with Homebrew:"
  echo "    brew install python3"
  echo ""
  echo "  Or download from: https://www.python.org/downloads/"
  echo ""
  read -p "Press Enter to exit..."
  exit 1
fi

echo "  Found Python: $PY"

# --- 2. Auto-install pyserial if missing ---
if ! "$PY" -c "import serial" 2>/dev/null; then
  echo "  Installing pyserial (needed for USB connection)..."
  "$PY" -m pip install pyserial --quiet 2>/dev/null || {
    echo "  WARNING: Could not install pyserial. Flash-over-USB may not work."
    echo "  You can still build and download the .bin file."
  }
fi

# --- 3. Make sure Arduino IDE can be found ---
if ! command -v arduino-cli >/dev/null 2>&1; then
  CLI_FOUND=false
  for p in "/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli" \
           "$HOME/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli"; do
    if [ -x "$p" ]; then
      CLI_FOUND=true
      break
    fi
  done
  if [ "$CLI_FOUND" = false ]; then
    echo ""
    echo "  NOTE: Arduino IDE not found."
    echo "  You can still browse and change settings, but to compile & flash"
    echo "  you need Arduino IDE 2.x from: https://www.arduino.cc/en/software"
    echo ""
  fi
fi

# --- 4. Start the server and open browser ---
echo ""
echo "  Starting server..."
open "http://localhost:8080" 2>/dev/null &
exec "$PY" configure.py