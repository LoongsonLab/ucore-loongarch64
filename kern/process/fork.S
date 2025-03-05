#include <asm/regdef.h>
#include <asm/loongisa_csr.h>
#include <memlayout.h>


.extern exception_return

.section .text 

.global forkrets
.type forkrets, @function
forkrets:
  addi.d sp, a0, -32
  b exception_return
.end forkrets
