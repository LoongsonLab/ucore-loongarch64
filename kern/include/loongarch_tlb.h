#ifndef _LOONGARCH_TLB_H
#define _LOONGARCH_TLB_H

#include <asm/loongisa_csr.h>
#include <glue_pgmap.h>

#define LOONGARCH_TLB_ENTRYL_V (1<<0)
#define LOONGARCH_TLB_ENTRYL_D (1<<1)
#define LOONGARCH_TLB_ENTRYL_G (1<<6)
#define LOONGARCH_TLB_MAT_CO (1<<4)
#define LOONGARCH_TLB_ENTRYH_VPPN_MASK (~0x1FFF)
#define LOONGARCH_TLB_PLV3 (3<<2)

static inline void write_one_tlb(int index, unsigned long long hi, unsigned long long low0, unsigned long long low1)
{
    write_csr_tlbidx(index | 12 << 24);
    // write_csr_stlbps(0xc);
    write_csr_entryhi(hi);
    write_csr_entrylo0(low0);
    write_csr_entrylo1(low1);
    write_csr_entryrhi(hi);
    write_csr_entryrlo0(low0);
    write_csr_entryrlo1(low1);
    // printhex(hi);
    // kprintf("\n");
    // printhex(low0);
    // kprintf("\n");
    // printhex(low1);
    // kprintf("\n");
    __asm__ __volatile__("tlbwr");
}

static inline void tlb_replace_random(unsigned long long hi, unsigned long long low0, unsigned long long low1)
{
    write_csr_tlbidx(12 << 24);
    // printhex(hi);
    // kprintf("\n");
    // printhex(low0);
    // kprintf("\n");
    // printhex(low1);
    // kprintf("\n");
    // write_csr_stlbps(0xc);
    write_csr_entryhi(hi | 12);
    write_csr_entrylo0(low0);
    write_csr_entrylo1(low1);
    write_csr_entryrhi(hi | 12);
    write_csr_entryrlo0(low0);
    write_csr_entryrlo1(low1);
    __asm__ __volatile__("tlbfill");

}

// void tlb_refill(uint32_t badaddr, pte_t *pte);

// uint32_t pte2tlblow(pte_t pte);

#endif