#include <asm/regdef.h>
.text
.globl _start
_start:
    nop
    addi.d sp, sp, -32
    b umain
    nop

