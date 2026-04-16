#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/platform/linux/api"
DEFAULT_PLATFORM="linux-armv7hf"
DEFAULT_DEPS_DIR="/opt/mplc4"

PLATFORM="$DEFAULT_PLATFORM"
DEPS_DIR="${MPLC_LIB_SO_DIR:-$DEFAULT_DEPS_DIR}"
OUTPUT_DIR="${OUTPUT_DIR:-$SCRIPT_DIR/dist}"

usage() {
    cat <<'EOF'
Usage:
  ./build-driver.sh [platform] [--deps-dir PATH] [--output-dir PATH]
  ./build-driver.sh --help

Arguments:
  platform            Target platform for makedrv.sh.
                      Default: linux-armv7hf

Options:
  --deps-dir PATH     Directory with MasterPLC runtime libraries.
                      Default: /opt/mplc4 or env MPLC_LIB_SO_DIR
  --output-dir PATH   Directory for collected build artifacts.
                      Default: ./dist

Examples:
  ./build-driver.sh
  ./build-driver.sh linux-armv7hf
  ./build-driver.sh kvantor --deps-dir /mnt/sdk/mplc4
EOF
}

if [[ "${1:-}" == "--help" || "${1:-}" == "-h" ]]; then
    usage
    exit 0
fi

if [[ $# -gt 0 && "${1:-}" != --* ]]; then
    PLATFORM="$1"
    shift
fi

while [[ $# -gt 0 ]]; do
    case "$1" in
        --deps-dir)
            DEPS_DIR="$2"
            shift 2
            ;;
        --output-dir)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage >&2
            exit 1
            ;;
    esac
done

REQUIRED_LIBS=(
    masterplc.so
    mplc_archive.so
    mplcshare.so
    opcua.so
    liblua.so
    mplc_events.so
)

echo "==> Platform:   $PLATFORM"
echo "==> Deps dir:   $DEPS_DIR"
echo "==> Output dir: $OUTPUT_DIR"

mkdir -p "$BUILD_DIR/mplc_lib_so" "$OUTPUT_DIR/$PLATFORM"

for lib in "${REQUIRED_LIBS[@]}"; do
    if [[ ! -f "$DEPS_DIR/$lib" ]]; then
        echo "Required library not found: $DEPS_DIR/$lib" >&2
        echo "Specify another directory via --deps-dir or MPLC_LIB_SO_DIR." >&2
        exit 1
    fi
    cp -f "$DEPS_DIR/$lib" "$BUILD_DIR/mplc_lib_so/$lib"
done

pushd "$BUILD_DIR" > /dev/null
bash ./makedrv.sh "$PLATFORM"

shopt -s nullglob
for artifact in ./*.so ./*.map ./log.txt; do
    cp -f "$artifact" "$OUTPUT_DIR/$PLATFORM/"
done
shopt -u nullglob
popd > /dev/null

echo "==> Build completed"
echo "Artifacts:"
echo "  $OUTPUT_DIR/$PLATFORM"
