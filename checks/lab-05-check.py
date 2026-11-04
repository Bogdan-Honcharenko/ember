from pathlib import Path
import subprocess
import re
root=Path(__file__).resolve().parent.parent
def run(text):
    p=subprocess.run([str(root/"build/ember")],input=text,text=True,capture_output=True,timeout=10)
    assert p.returncode==0 and not p.stderr,p.stderr
    return p.stdout
for name in ["lab-05","lab-05-frame","lab-05-store","lab-05-string"]:
    result=run((root/"checks"/(name+".txt")).read_text())
    (root/"checks"/(name+"-output.txt")).write_text(result)
    rows=re.findall(r"[#.]{64}",result)
    if name=="lab-05":
        assert len(rows)==64
        for start in [0,32]:
            assert sum(r.count("#") for r in rows[start:start+32])==3
            assert rows[start][0]=="#" and rows[start+2][13]=="#" and rows[start+31][63]=="#"
    elif name=="lab-05-frame":
        assert len(rows)==32 and rows[0]=="#"*64 and rows[31]=="#"*64
        assert all(r=="#"+"."*62+"#" for r in rows[1:31])
    elif name=="lab-05-store":
        assert len(rows)==32 and rows[0]=="#"+"."*63
        assert sum(r.count("#") for r in rows)==1
    else:
        assert "HELLOember>" in result and "found at 2050" in result
for args in ["-1 0","64 0","0 32","9999999999 0","1","a 2"]:
    assert "usage: plot" in run("plot "+args+"\nquit\n")
for args in ["-1 5","8 4","0 4097","x 4"]:
    assert "usage: findnz" in run("findnz "+args+"\nquit\n")
assert "not found" in run("cls\nfindnz 0xA00 0xB00\nquit\n")
assert "found at 2577" in run("cls\nplot 13 2\nfindnz 0xA00 0xB00\nquit\n")
print("LAB05 REPL CHECKS PASSED: screens, STORE, strings, search, invalid inputs")
