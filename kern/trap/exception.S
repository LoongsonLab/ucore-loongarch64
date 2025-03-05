#include <asm/regdef.h>
#include <asm/loongisa_csr.h>
#include <memlayout.h>

.extern current
.extern loongarch_trap

.section .text
.global exception_handler
.global exception_return
.global tlbrefill_handler
.global tlbrefill_redirector


.align 6
.type tlbrefill_handler, @function
tlbrefill_handler:
    // Save t0 to KS0
    csrwr   t0, LISA_CSR_KS0
    // Save RFPRMD to KS1 and clear RFPRMD
    li.d    t0, 0
    csrwr   t0, LISA_CSR_RFPRMD
    csrwr   t0, LISA_CSR_KS1        //KS1 saved RFPRMD
    // Save BADV to KS4
    csrrd   t0, LISA_CSR_RFBADV
    csrwr   t0, LISA_CSR_KS4
    // get tlbrefill_redirector pointer from KS3 as we already set it in kern/init/init.c
    csrrd   t0, LISA_CSR_KS3
    csrwr   t0, LISA_CSR_RFEPC
    // Save RFEPC(ERA) to KS2
    addi.d  t0, t0, -1      //istlbr==0
    csrwr   t0, LISA_CSR_KS2
    ertn

.type tlbrefill_redirector, @function
tlbrefill_redirector:

    // return PRMD from KS1(RFPRMD)
    csrrd   t0, LISA_CSR_KS1
    csrwr   t0, LISA_CSR_PRMD
    // return EPC(ERA) from KS2
    csrrd   t0, LISA_CSR_KS2
    csrwr   t0, LISA_CSR_EPC
    //set ESTAT = tlbrefill exception(ecode=0x3f)(la64)
    li.d    t0, 0x3f0000    
    csrwr   t0, LISA_CSR_KS5
    // return t0 from ks0
    csrrd   t0, LISA_CSR_KS0
    
    b tlbexception_handler

.align 4
.type tlbexception_handler, @function
tlbexception_handler:
    // Save t0 and t1
    csrwr   t0, LISA_CSR_KS0
    csrwr   t1, LISA_CSR_KS1
    // Save previous stack pointer in t1
    move    t1, sp
    csrwr   t1, LISA_CSR_KS2
    //t1 saved the vaual of KS2,KS2 saved sp
    /*
        Warning: csrwr will bring the old csr register value into rd, 
        not only just write rd to csr register,
        so you may see the rd changed.
        It's documented in the manual from loongarch.
    */
    // check if user mode
    csrrd   t0, LISA_CSR_PRMD  
    andi    t0, t0, 3
    beq     t0, zero, 1f

    
    /* Coming from user mode - load kernel stack into sp */
    la      t0, current // current pointer
    ld.d    t0, t0, 0 // proc struct
    ld.d    t0, t0, 0x10 // kstack pointer
    addi.d  t1, zero, 1
    slli.d  t1, t1, 13 // KSTACKSIZE=8192=pow(2,13)
    add.d   sp, t0, t1
    csrrd   t1, LISA_CSR_KS2
  
1:
    //saved EXST to t0 for save EXST to sp later
    csrrd   t0, LISA_CSR_KS5        //KS5 saved ESTAT
    //return KS2
    csrrd   t1, LISA_CSR_KS2
    b common_exception

.align 4
.type exception_handler, @function
exception_handler:
    // Save t0 and t1
    csrwr   t0, LISA_CSR_KS0
    csrwr   t1, LISA_CSR_KS1
    // Save previous stack pointer in t1
    move    t1, sp
    csrwr   t1, LISA_CSR_KS2
    //t1 saved the vaual of KS2,KS2 saved sp
    // Save BADV to KS4
    csrrd   t0, LISA_CSR_BADV
    csrwr   t0, LISA_CSR_KS4
    /*
        Warning: csrwr will bring the old csr register value into rd, 
        not only just write rd to csr register,
        so you may see the rd changed.
        It's documented in the manual from loongarch.
    */
    // check if user mode
    csrrd   t0, LISA_CSR_PRMD  
    andi    t0, t0, 3
    beq     t0, zero, 1f

    
    /* Coming from user mode - load kernel stack into sp */
    la      t0, current // current pointer
    ld.d    t0, t0, 0 // proc struct
    ld.d    t0, t0, 0x10 // kstack pointer
    addi.d  t1, zero, 1
    slli.d  t1, t1, 13 // KSTACKSIZE=8192=pow(2,13)
    add.d   sp, t0, t1
    csrrd   t1, LISA_CSR_KS2
  
1:
    //saved EXST to t0 for save EXST to sp later
    csrrd   t0, LISA_CSR_EXST
    //return KS2
    csrrd   t1, LISA_CSR_KS2
    
    b common_exception

common_exception:
   /*
    * At this point:
    *      Interrupts are off. (The processor did this for us.)
    *      t0 contains the exception status(like exception cause on MIPS).
    *      t1 contains the old stack pointer.
    *      sp points into the kernel stack.
    *      ks4 contains the BADV
    *      ks5 contains the ESTAT
    *      All other registers are untouched.
    */
   
   /*
    * Allocate stack space for 35 words to hold the trap frame,
    * plus four more words for a minimal argument block.
    */
    addi.d  sp, sp, -304
    st.d    s8, sp, 288
    st.d    s7, sp, 280
    st.d    s6, sp, 272
    st.d    s5, sp, 264
    st.d    s4, sp, 256
    st.d    s3, sp, 248
    st.d    s2, sp, 240
    st.d    s1, sp, 232
    st.d    s0, sp, 224
    st.d    fp, sp, 216
    st.d    reserved_reg, sp, 208
    st.d    t8, sp, 200
    st.d    t7, sp, 192
    st.d    t6, sp, 184
    st.d    t5, sp, 176
    st.d    t4, sp, 168
    st.d    t3, sp, 160
    st.d    t2, sp, 152
    //st.w    t1, sp, 144
    //st.w    t0, sp, 136
    st.d    a7, sp, 128
    st.d    a6, sp, 120
    st.d    a5, sp, 112
    st.d    a4, sp, 104
    st.d    a3, sp, 96
    st.d    a2, sp, 88
    st.d    a1, sp, 80
    st.d    a0, sp, 72
    st.d    t1, sp, 64  // replace sp with real sp, now use t1 for free
    st.d    tp, sp, 56
    // save real t0 and t1 after real sp (stored in t1 previously) stored
    csrrd   t1, LISA_CSR_KS1
    st.d    t1, sp, 144
    csrrd   t1, LISA_CSR_KS0
    st.d    t1, sp, 136
    
    // replace with real value
    // save tf_era after t0 and t1 saved
    csrrd   t1, LISA_CSR_EPC
    st.d    t1, sp, 296

   /*
    * Save remaining exception context information.
    */

    // save ra (note: not in pushregs, it's tf_ra)
    st.d    ra, sp, 48
    // save prmd
    csrrd   t1, LISA_CSR_PRMD
    st.w    t1, sp, 44
    // save estat
    st.w    t0, sp, 40
    // now use t0 for free
    // store badv
    csrrd   t0, LISA_CSR_KS4
    st.d    t0, sp, 32
    st.d    zero, sp, 24
    // support nested interrupt

    // IE and PLV will automatically set to 0 when trap occur

    // set trapframe as function argument
    addi.d  a0, sp, 32
    li.w    t0, 0xb0    # PLV=0, IE=0, PG=1
    csrwr   t0, LISA_CSR_CRMD
    la.abs  t0, loongarch_trap
    jirl    ra, t0, 0
    //bl loongarch_trap



exception_return:
    // restore prmd
    ld.d    t0, sp, 44
    li.d    t1, 7
    csrxchg t0, t1, LISA_CSR_PRMD
    csrrd   t0, LISA_CSR_PRMD
    // restore era no k0 and k1 for la64, so must do first
    ld.d    t0, sp, 296
    csrwr   t0, LISA_CSR_EPC
    // restore general registers
    ld.d    ra, sp, 48
    ld.d    tp, sp, 56
    //ld.d    sp, sp, 64 (do it finally)
    ld.d    a0, sp, 72
    ld.d    a1, sp, 80
    ld.d    a2, sp, 88
    ld.d    a3, sp, 96
    ld.d    a4, sp, 104
    ld.d    a5, sp, 112
    ld.d    a6, sp, 120
    ld.d    a7, sp, 128
    ld.d    t0, sp, 136
    ld.d    t1, sp, 144
    ld.d    t2, sp, 152
    ld.d    t3, sp, 160
    ld.d    t4, sp, 168
    ld.d    t5, sp, 176
    ld.d    t6, sp, 184
    ld.d    t7, sp, 192
    ld.d    t8, sp, 200
    ld.d    reserved_reg, sp, 208
    ld.d    fp, sp, 216
    ld.d    s0, sp, 224
    ld.d    s1, sp, 232
    ld.d    s2, sp, 240
    ld.d    s3, sp, 248
    ld.d    s4, sp, 256
    ld.d    s5, sp, 264
    ld.d    s6, sp, 272
    ld.d    s7, sp, 280
    ld.d    s8, sp, 288
    // rstore sp
    ld.d    sp, sp, 64
    ertn

    .end exception_return
    .end common_exception




