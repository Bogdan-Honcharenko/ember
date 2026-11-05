from pathlib import Path
import subprocess, tempfile, os
exe='./build/ember'
env=dict(os.environ, ASAN_OPTIONS='detect_leaks=1:halt_on_error=1', UBSAN_OPTIONS='halt_on_error=1')
def run(args, text=None):
    p=subprocess.run([exe]+args,input=text,text=True,capture_output=True,env=env,timeout=20)
    assert 'Sanitizer' not in p.stderr and 'runtime error:' not in p.stderr, p.stderr
    return p
with tempfile.TemporaryDirectory() as d:
    file=Path(d)/'test.asm'
    def source(s, option='--asm'):
        file.write_text(s)
        return run([option,str(file)]) if option else run([str(file)])
    for name, expected in [('hello','HELLO'),('fib','8'),('fib10','55'),('search','2')]:
        p=run(['programs/'+name+'.asm']); assert p.returncode==0 and p.stdout.strip()==expected,(name,p.stdout,p.stderr)
        Path('checks/lab-08-'+name+'-output.txt').write_text(p.stdout+'\n' if not p.stdout.endswith('\n') else p.stdout)
    p=source('LOADI A, 0b1010\nLOADI B, 0x10\nJMP end\nNOP\nend: HALT\n')
    assert p.returncode==0 and '20 0a 21 10 30 08 00 01 00' in p.stdout,p.stdout
    for bad in ['0x','0b','0xGG','0b102','12abc','1_0']:
        p=source('; comment\nLOADI A, '+bad+'\n'); assert p.returncode!=0 and 'line 2' in p.stderr,(bad,p.stderr)
    for s in ['LOADI A, 256','LOADI A, 999999999999999999999999999','JMP 4096','JMP missing','x: NOP\nx: HALT','LOADI C, 1','LOADI A 1','HALT 1','WHAT','LOAD A, [2','LOADI A, -1']:
        p=source(s); assert p.returncode!=0 and 'line ' in p.stderr,(s,p.stderr)
    assert source('NOP\n'*2048).returncode==0
    assert source('NOP\n'*2049).returncode!=0
    p=source('start: NOP\r\n; comment\r\nJMP start'); assert '01 30 00 00' in p.stdout
    p=source('LOADI A, 1 ; hi\n', '--lex'); assert p.returncode==0 and 'Number 1' in p.stdout
    p=source('loop: JMP loop', ''); assert p.returncode!=0 and 'step limit' in p.stderr
    p=source('RET', ''); assert p.returncode!=0
    p=run(['/missing/ember-test.asm']); assert p.returncode!=0
    p=run(['--bad',str(file)]); assert p.returncode!=0
    file.write_text('LOADI A, 300')
    p=run([], 'set 100 77\nasm '+str(file)+'\nget 100\nquit\n')
    assert '77  0x4d' in p.stdout and 'line 1' in p.stderr,(p.stdout,p.stderr)
    search=Path('programs/search.asm').read_text().replace('LOADI B, 65','LOADI B, 66')
    p=source(search,''); assert p.returncode==0 and p.stdout.strip()=='255',p.stdout
    p=run(['programs/pixels.asm']); assert p.returncode==0
    rows=p.stdout.splitlines(); assert len(rows)==32 and sum(r.count('#') for r in rows)==2,p.stdout
    Path('checks/lab-08-screen.txt').write_text(p.stdout)
for name in ['hello','search','fib']:
    for option,suffix in [('--asm','bytes'),('--lex','tokens')]:
        p=run([option,'programs/'+name+'.asm']); assert p.returncode==0
        Path('checks/lab-08-'+name+'-'+suffix+'.txt').write_text(p.stdout)
p=run([], 'asm programs/hello.asm\nrun\ndump\nquit\n'); assert p.returncode==0
Path('checks/lab-08-dump.txt').write_text(p.stdout)
print('Lab 08: examples, labels, number errors, bounds, CLI, atomic load, search miss and screen PASS')
