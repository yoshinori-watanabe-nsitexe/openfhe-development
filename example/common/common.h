
#include <stdio.h>
#include <string.h>
#include "./../common/encoding.h"

#define Null  0
#define true  1
#define false 0

#define fbs1_addr_base  (0x002000000lu)
#define fbs2_addr_base  (0x00c000000lu)
#define rbs_addr_base   (0x008000000lu)
#define dbs_addr_base   (0x000000400lu)
#define musl_addr_base  (0x028000000lu)
#define ctrls_addr_base (0x020000000lu)
#define bram_addr_base  (0x040000000lu)
#define pcie_addr_base  (0x060000000lu)
#define pcie_ctrl_addr  (0x020008000lu)
#define dram_addr_base  (0x080000000lu)
#define ddr4_ctrl_addr  (0x020009000lu)
#define uart0_addr_base (0x020010000lu)
#define uart1_addr_base (0x020011000lu)
#define brom_addr_base  (0x000001000lu)
#define dbg_addr_base   (0x000000400lu)   //
#define clint_addr_base (0x002000000lu)   //
#define cmu_addr_base   (0x002200000lu)   //
#define plic_addr_base  (0x00c000000lu)   //
#define save_addr_base  (0x040004000lu)

#define INTSW_ADDR      (clint_addr_base+0x0000)
#define CMPTMR_ADDR     (clint_addr_base+0x4000)
#define TIMER_ADDR      (clint_addr_base+0xbff8)
#define INTEXT_ADDR     (0x002100000)
#define CSR_MSTATUS     0x300
#define CSR_MIE         0x304
#define CSR_MTVEC       0x305
#define CSR_MEPC        0x341
#define CSR_MCAUSE      0x342
#define CSR_MTVAL       0x343
#define CSR_MIP         0x344
#define MCAUSE_EXT      0x0b
#define MCAUSE_TMR      0x07
#define MCAUSE_SW       0x03
#define MIE_MEIE        (0x1<<MCAUSE_EXT)
#define MIE_MTIE        (0x1<<MCAUSE_TMR)
#define MIE_MSIE        (0x1<<MCAUSE_SW)

void intpin_set(int addr, int hart) {
  volatile unsigned int *acc_addr;

  acc_addr  = (volatile unsigned int *)((unsigned long)addr+4*hart);
  *acc_addr = 0x1;
}

void intpin_clr(int addr, int hart) {
  volatile unsigned int *acc_addr;

  acc_addr  = (volatile unsigned int *)((unsigned long)addr+4*hart);
  *acc_addr = 0x0;
}

static inline uint64_t get_hartid()
{
    register uint64_t hartid;
    asm volatile("csrr %0, mhartid;\n\t" : "=&r"(hartid));
    return hartid;
}

//******************************************
// Function: handle_trap
//******************************************
uintptr_t  handle_trap(uintptr_t cause, uintptr_t epc, uintptr_t regs[32]) {
  volatile unsigned int hartid;
  
  *(unsigned long *)(0xffff0000 | 8*CSR_MCAUSE)  = cause;
  *(unsigned long *)(0xffff0000 | 8*CSR_MTVEC)   = read_csr(mtvec);
  *(unsigned long *)(0xffff0000 | 8*CSR_MEPC)    = read_csr(mepc);
  *(unsigned long *)(0xffff0000 | 8*CSR_MTVAL)   = read_csr(mtval);
  *(unsigned long *)(0xffff0000 | 8*CSR_MSTATUS) = read_csr(mstatus);
  *(unsigned long *)(0xffff0000 | 8*CSR_MIE)     = read_csr(mie);
  *(unsigned long *)(0xffff0000 | 8*CSR_MIP)     = read_csr(mip);

  asm volatile("csrr %0, mhartid;\n\t" : "=&r"(hartid));
  if ((cause & 0x0000ffff) == MCAUSE_EXT) {
    intpin_clr(INTEXT_ADDR, hartid);
  } else if ((cause & 0x0000ffff) == MCAUSE_TMR) {
    *(volatile unsigned long *)((unsigned long)CMPTMR_ADDR+8*hartid) = 0x7fffffffffffffff;
  } else if ((cause & 0x0000ffff) == MCAUSE_SW) {
    intpin_clr(INTSW_ADDR, hartid);
  }
  
  if((cause & 0x8000000000000000) != 0x8000000000000000) {
    return (epc + 4);
  } else {
    return (epc);
  }
}

static inline void wait_3cycle(uint64_t wait_cyc)
{
    asm volatile(
        "   beqz    %[wcyc], 2f             ;\n\t"
        "   mv      x15,     %[wcyc]        ;\n\t"
        "1:                                 ;\n\t"
        "   nop                             ;\n\t"
        "   addiw   x15,     x15,     -1    ;\n\t"
        "   bnez    x15,     1b             ;\n\t"
        "2:                                 ;\n\t"
        :
        : [wcyc] "r" (wait_cyc)
        : "x15"
    );
}

static inline void set_flag(volatile uint64_t * flag_addr, uint64_t wdata)
{
    asm volatile (
        "1:                                  \n\t"
        "   lr.d    x15, (%[addr])          ;\n\t"
        "   or      x15, x15, %[wdata]      ;\n\t"
        "   sc.d    x15, x15, (%[addr])     ;\n\t"
        "   bnez    x15, 1b                 ;\n\t"
        :
        : [addr] "r" (flag_addr), [wdata] "r" (wdata)
        : "x15"
    );
}

static inline void set_intval(uint64_t intval_offset)
{
    asm volatile (
        "   auipc   x15,  0x0               ;\n\t"
        "   add     x15,  x15, %[intvl]     ;\n\t"
        "   jalr    zero, 10(x15)           ;\n\t"
        "   nop                             ;\n\t"
        "   nop                             ;\n\t"
        "   nop                             ;\n\t"
        "   nop                             ;\n\t"
        :
        : [intvl] "r" (intval_offset)
        : "x15"
    );
}

void thread_entry(int cid, int nc)
{
}

