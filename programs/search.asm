; First 65 in [3, 1, 65, 65, 0, 0, 0, 0]; index or 255.
LOADI A, 3
STORE [0x800], A
LOADI A, 1
STORE [0x801], A
LOADI A, 65
STORE [0x802], A
STORE [0x803], A
LOADH H, 0x800
loop:
    LOAD A, [H]
    LOADI B, 65
    CMP A, B
    JZ found
    INCH
    HLOW
    LOADI B, 8
    CMP A, B
    JNZ loop
    LOADI A, 255
    OUTN
    HALT
found:
    HLOW
    OUTN
    HALT
