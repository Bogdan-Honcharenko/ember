; Recursive fib: A = argument/result, B = scratch.
LOADI A, 6
CALL fib
OUTN
HALT
fib:
    LOADI B, 0
    CMP A, B
    JZ base
    LOADI B, 1
    CMP A, B
    JZ base
    PUSH A
    DEC A
    CALL fib
    POP B
    PUSH A
    MOV A, B
    DEC A
    DEC A
    CALL fib
    POP B
    ADD A, B
base:
    RET
