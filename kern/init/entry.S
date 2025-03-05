#include <asm/regdef.h>
#include <asm/loongisa_csr.h>

.global kernel_entry
.extern kern_init

.extern edata
.extern end

.section .startup

#define KSTACKSIZE (4096*2)

kernel_entry:
  b reset
  nop
    .align 4
reset:
    la t0, 1f
    jirl zero, t0, 0
1:
    la sp, bootstacktop
#zero bss
    la t0, edata
    la t1, end
2:
    st.d zero, t0, 0
    addi.w t0, t0, 4
    slt t3, t0, t1
    bgtz t3, 2b
    addi.d sp, sp, -16
    # Config direct window and set PG
    li.d    t0, 0x9000000000000011
    csrwr   t0, 0x180
    // DMWIN0: 0x9000000000000000-0x9fffffffffffffff->0x000000000000-0xffffffffffff Cached
    li.d    t0, 0x8000000000000001
    csrwr   t0, LISA_CSR_DMWIN1
    // DMWIN1: 0x8000000000000000-0x8fffffffffffffff->0x000000000000-0xffffffffffff Uncached (serial port is here)
    # Enable PG
    li.d    t0, 0xb0    # PLV=0, IE=0, PG
    csrwr   t0, LISA_CSR_CRMD
     # pgsize
    li.w    t0, 0xc    
    csrwr   t0, LISA_CSR_FTLBPGSIZE
    b kern_init

.section .data
    .global bootstack
bootstack:
    .space KSTACKSIZE
    .global bootstacktop
bootstacktop:
    .space 64