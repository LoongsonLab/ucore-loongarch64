#include <defs.h>
#include <loongarch.h>
#include <asm/loongisa_csr.h>
#include <picirq.h>
#include <trap.h>
#include <defs.h>
#include <loongarch.h>
#include <asm/loongisa_csr.h>
#include <picirq.h>
#include <trap.h>
#include <kdebug.h>
#include <pmm.h>
#include <vmm.h>
#include <proc.h>
#include <sched.h>
#include <ide.h>
#include <fs.h>

void setup_exception_vector()
{
    extern unsigned char exception_handler[];
    extern unsigned char tlbrefill_handler[];
    extern unsigned char tlbrefill_redirector[];
    unsigned long long isTLBR=0x1;
    __csrwr_d(exception_handler, LISA_CSR_EBASE);
    __csrwr_d(((unsigned long long)tlbrefill_redirector|isTLBR), LISA_CSR_KS3);
    __csrwr_d((unsigned long long)tlbrefill_handler & 0x0fffffffffffffff, LISA_CSR_RFBASE);
}

void check_tlb() {
    kprintf("tlbidx = %08x\n",read_csr_tlbidx());
    kprintf("stlbps = %08x\n",read_csr_stlbps());
    kprintf("entryrhi = %08x\n",read_csr_entryrhi());
}

void __noreturn
kern_init(void) {
    setup_exception_vector();
    tlb_invalidate_all();
    pic_init();                 // init interrupt controller
    cons_init();                // init the console
    clock_init();               // init clock interrupt
    const char *message = "(THU.CST) os is loading ...\n\n";
    kprintf(message);
    print_kerninfo();
#ifdef LAB2_EX1
    pmm_init();                 // init physical memory management
#else
    // For LAB1
    kprintf("LAB1 Check - Please press your keyboard manually and see what happend.\n");
    intr_enable();
    while(1) asm volatile ("\tidle 0\n");
#endif
#ifdef LAB3_EX1
    vmm_init();                 // init virtual memory management
#else
    // For LAB2
    kprintf("LAB2 Check Pass!\n");
    intr_enable();
    while(1) asm volatile ("\tidle 0\n");
#endif
#ifndef LAB4_EX1
    // For LAB3
    kprintf("LAB3 Check Pass!\n");
    intr_enable();
    while(1) asm volatile ("\tidle 0\n");
#endif
    sched_init();
    proc_init();                // init process table
#ifdef LAB8_EX2
    check_initrd();
    ide_init();
    fs_init();
#endif
    intr_enable();              // enable irq interrupt
    cpu_idle();
}