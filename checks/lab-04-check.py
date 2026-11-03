import pathlib
import subprocess

root = pathlib.Path(__file__).resolve().parent.parent
exe = root / "build/ember"
def run(commands):
    result = subprocess.run([str(exe)], input=commands, text=True, capture_output=True, timeout=10)
    assert result.returncode == 0, result.stderr
    assert not result.stderr, result.stderr
    return result.stdout

cases = {"lab-04": "3 2 1 ", "lab-04-search": "2 ember>", "lab-04-miss": "255 ember>", "lab-04-limit": "step limit reached (100000)", "lab-04-unknown": "unknown opcode 0xff", "lab-04-trace": "PC=8 H=0 A=0"}
for name, expected in cases.items():
    text = run((root / "checks" / (name + ".txt")).read_text())
    assert expected in text, text
    (root / "checks" / (name + "-output.txt")).write_text(text)

base = (root / "checks/lab-04-miss.txt").read_text().split("find ")[0]
for offset in range(8):
    text = run(base + f"set {2048+offset} 65\nrun\nregs\nquit\n")
    assert f"{offset} ember>" in text and f"H={2048+offset}" in text, text

for args in ["-1 5 65", "5 4 65", "0 4097 0", "0 8 256", "0 8 -1", "x 8 65", "0 8", "0 8 65 extra"]:
    assert "usage: find" in run("find " + args + "\nquit\n")
assert "not found" in run("find 4096 4096 0\nquit\n")
assert "found at 4095" in run("set 4095 65\nfind 4095 4096 65\nquit\n")
print("LAB04 REPL CHECKS PASSED: countdown, search at all 8 positions, miss, invalid inputs, limit")
