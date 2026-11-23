#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
mkdir -p "$tmp"/{mplc,core,share}
touch "$tmp"/{mplc/api.h,core/main.h,share/scada_types.h}
g++ -I"$tmp" -include tests/scada_fixture.h -std=c++17 -Wall -Wextra -Wpedantic tests/sa02m_io.cpp examples/mplc_cyntron/{gpio_helper,CA_02m,cyntron_protocol,external_rtc}.cpp \
    -Wl,--wrap=open,--wrap=close,--wrap=ioctl,--wrap=write,--wrap=read -o "$tmp/sa02m-io"
"$tmp/sa02m-io"
