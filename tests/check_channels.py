"""Compare source dispatch and both ELF name tables to the supplied channel contract."""
import argparse
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("candidate", type=Path)
parser.add_argument("shipped", type=Path)
args = parser.parse_args()
names = {}
rows = []
for item in json.loads((ROOT / "docs/channel-contract.json").read_bytes()):
    proto = item["protocol"]
    names[proto] = set()
    for channel in item["channels"]:
        name = channel["name"]
        assert name not in names[proto]
        names[proto].add(name)
        port, = channel["ports"]
        rows.append(dict(protocol=proto, name=name, portType=port["type"],
                         direction="input" if proto == "CyntronProtocol" else "output"))
src = ROOT / "examples/mplc_cyntron"
assert len(names["CyntronProtocol"]) == 17 and len(names["CA_02m"]) == 5
diag = (src / "cyntron_protocol.cpp").read_text()
outputs = (src / "CA_02m.cpp").read_text()
assert set(re.findall(r'name == "([A-Z_]+)"', diag)) == names["CyntronProtocol"]
assert set(re.findall(r'(?:enabled|chan\.find)\("([A-Z_]+)"\)', outputs)) == names["CA_02m"]
assert "InVar.Update(" in diag and "OutVar.Get<bool>()" in outputs
for proto, source in (("CyntronProtocol", diag), ("CA_02m", outputs)):
    assert f"MPLC_PROTOCOL_TYPE({proto}," in source
for path in (args.shipped, args.candidate):
    elf = path.read_bytes()
    assert elf.startswith(b"\x7fELF")
    for row in rows:
        assert row["name"].encode() in elf, (path, row["name"])
print(json.dumps(dict(status="PASS_STATIC_CHANNEL_CONTRACT", channels=rows), indent=2))
