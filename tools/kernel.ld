OUTPUT_ARCH(loongarch64)
ENTRY(kernel_entry)
SECTIONS
{
    . = 0x9000000000000000;
    .text      :
    {
      . = ALIGN(8);
      wrs_kernel_text_start = .; _wrs_kernel_text_start = .;
      *(.startup)
      *(.text) 
      *(.text.*)
      *(.gnu.linkonce.t*)
      *(.mips16.fn.*) 
      *(.mips16.call.*) /* for MIPS */
      *(.rodata) *(.rodata.*) *(.gnu.linkonce.r*) *(.rodata1)
      . = ALIGN(8192);
      *(.ramexv)
    }

    . = ALIGN(16);
    wrs_kernel_text_end = .; _wrs_kernel_text_end = .;
    etext = .; _etext = .;

    .stab : {
        . = ALIGN(8);
        PROVIDE(__STAB_BEGIN__ = .);
        *(.stab)
        PROVIDE(__STAB_END__ = .);
        BYTE(0)     /* Force the linker to allocate space
                  for this section */
        . = ALIGN(8);
    }

    .stabstr : {
        . = ALIGN(8);
        PROVIDE(__STABSTR_BEGIN__ = .);
        *(.stabstr)
        PROVIDE(__STABSTR_END__ = .);
        BYTE(0)     /* Force the linker to allocate space
                  for this section */
        . = ALIGN(8);
    }

    .data ALIGN(8)   : 
    {
      wrs_kernel_data_start = .; _wrs_kernel_data_start = .;
      *(.data)
      *(.data.*)
      *(.gnu.linkonce.d*)
      *(.data1)
      *(.eh_frame)
      *(.gcc_except_table)
      . = ALIGN(16);
      _gp = . + 0x7ff0;  /* set gp for MIPS startup code */
        /* got*, dynamic, sdata*, lit[48], and sbss should follow _gp */
      *(.got.plt)
      *(.got)
      *(.dynamic)
      *(.got2)
      *(.sdata) *(.sdata.*) *(.lit8) *(.lit4)
        . = ALIGN(32);
    }

    . = ALIGN(32);
    edata = .; _edata = .;
    wrs_kernel_data_end = .; _wrs_kernel_data_end = .;

    .bss  ALIGN(8)  :
    {
      wrs_kernel_bss_start = .; _wrs_kernel_bss_start = .;
      *(.sbss) *(.scommon) *(.dynbss) *(.bss) *(COMMON)
      . = ALIGN(32);
    }

    . = ALIGN(32);
    end = .; _end = .;
    wrs_kernel_bss_end = .; _wrs_kernel_bss_end = .;

}
