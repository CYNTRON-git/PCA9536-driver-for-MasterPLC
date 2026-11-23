"""Build the checked-out system driver against the verified 35699 input kit."""
import argparse
import hashlib
import importlib.util
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[1]
helper = ROOT / "tools/vendor_baseline_35699.py"
spec = importlib.util.spec_from_file_location("vendor_baseline", helper)
vendor = importlib.util.module_from_spec(spec)
spec.loader.exec_module(vendor)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--inputs", type=Path, required=True)
    parser.add_argument("--toolchain", type=Path, required=True)
    parser.add_argument("--work-dir", type=Path, required=True)
    args = parser.parse_args()
    work = args.work_dir.resolve()
    tc = args.toolchain.resolve()
    inputs = args.inputs.resolve()
    if vendor.inside(work, inputs) or vendor.inside(work, ROOT):
        parser.error("Use a fresh build directory outside the source and input trees")
    work.mkdir(parents=True, exist_ok=False)
    result = {"status": "FAILED", "sdk": "1.3.10.35699", "hardware_validation": False}
    try:
        api, runtime = vendor.prepare(inputs, work)
        target = api / "examples/mplc_cyntron"
        shutil.rmtree(target)  # Fresh staging tree only, includes obsolete vendor helpers.
        shutil.copytree(ROOT / "examples/mplc_cyntron", target)
        common = api / "examples/common/ca02m_hw.h"
        common.parent.mkdir(exist_ok=True)
        shutil.copy2(ROOT / "examples/common/ca02m_hw.h", common)
        sources = {str(p.relative_to(api)): hashlib.sha256(p.read_bytes()).hexdigest()
                   for p in [*sorted(target.iterdir()), common] if p.is_file()}
        vendor.save_json(work / "driver-sources.json", sources)
        env = dict(PATH=f"{tc}/bin:/usr/bin:/bin", HOME=str(work), LC_ALL="C", SHELL="/bin/bash")
        result["toolchain"] = vendor.preflight(tc, work, env)
        env.update(MPS_TOOLCHAIN_BIN=str(tc / "bin"), MPLC_RUNTIME=str(runtime),
                   CXXFLAGS="-std=gnu++17 " + vendor.CPU, CFLAGS=vendor.CPU,
                   NAME="mplc_cyntron", JOBS="1")
        cwd = api / "platform/linux/api"
        vendor.run_build(["/bin/bash", cwd / "makedrv.sh", "linux-armv7hf"],
                         cwd, env, work / "build.log", 900)
        artifact = cwd / "mplc_cyntron.so"
        result["abi"] = vendor.verify(artifact, tc, work, env)
        exports = (work / "abi-exports.txt").read_text()
        for name in ("CA_02m", "CyntronProtocol", "DisposeAddin"):
            if name not in exports:
                raise RuntimeError(f"Missing export for {name}")
        result["status"] = "PASS_CANDIDATE_COMPILE_LINK_ABI"
    finally:
        vendor.save_json(work / "result.json", result)


if __name__ == "__main__":
    main()
