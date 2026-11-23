#!/usr/bin/env bash
set -euo pipefail
repo=$(cd "$(dirname "$0")" && pwd)
workspace=${CYNTRON_BUILD_ROOT:-/Users/cyntron/cyntron-build}
out=${1:?Usage: build-sa02m.sh NEW_OUTPUT_DIR}
mkdir -p "$(dirname "$out")"
mkdir "$out"
out=$(cd "$out" && pwd)
source "$workspace/env/base.sh"
exec "$workspace/run-task" --profile background -- docker --context colima-cyntron run --rm \
    --pull=never --platform linux/amd64 --network none --read-only --cpus 1 --memory 2g \
    --pids-limit 128 --tmpfs /tmp:rw,size=256m \
    --mount type=volume,source=cyntron-sa-bsp-work,target=/kit,readonly \
    --mount "type=bind,source=$repo,target=/src,readonly" \
    --mount "type=bind,source=$out,target=/output" \
    sha256:9af56be350d47f08b71df02abac56118fdc3bfa886adc26c64eb612fb3d50e7a \
    timeout 900 python3 -I /src/tools/build_sa02m.py --inputs /kit/inputs/sa-mplc-inputs \
    --toolchain /kit/toolchains/arm-mpssoft-linux-gnueabihf --work-dir /output/work
