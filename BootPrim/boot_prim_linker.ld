/*
******************************************************************************
**
** @file        : LinkerScript.ld
** @author      : STM32 VS Code Extension (customized by it32bit)
**
**  Abstract    : Linker script for STM32F407G-DISC1 (STM32F407VGTx)
**                - 1024KB FLASH
**                - 64KB CCMRAM
**                - 128KB RAM
**
**  BootPrim region: Sector 0 (0x08000000–0x08003FFF)
**
******************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Stack and heap configuration */
_estack = ORIGIN(RAM) + LENGTH(RAM);
_Min_Heap_Size = 0x200;
_Min_Stack_Size = 0x400;

/* Memory layout */
MEMORY
{
  CCMRAM (xrw): ORIGIN = 0x10000000, LENGTH = 64K
  RAM    (xrw): ORIGIN = 0x20000000, LENGTH = 128K
  FLASH  (rx) : ORIGIN = 0x08000000, LENGTH = 16K /* Sector 0: BootPrim */
}

/* Symbol for end of BootPrim region */
__boot_prim_end__ = ORIGIN(FLASH) + LENGTH(FLASH);

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)
    KEEP(*(.init))
    KEEP(*(.fini))
    . = ALIGN(4);
    _etext = .;
  } >FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  .firmware_version :
  {
    . = ALIGN(4);
    KEEP(*(.firmware_version))
    . = ALIGN(4);
  } >FLASH

  .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN(__init_array_end = .);
  } >FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN(__fini_array_end = .);
  } >FLASH

  /* Shared RAM section at the beginning of RAM for bootloader - app communication*/
  .shared_ram (NOLOAD) :
  {
    __shared_ram_start = ORIGIN(RAM);
    . = __shared_ram_start;
    KEEP(*(.shared_ram))
    . = ALIGN(8);
    __shared_ram_end = .;
  } >RAM
  _sidata = LOADADDR(.data);

  .data :
  {
    . = __shared_ram_end;
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT> FLASH

  /* RAM-executed flash functions */
  .ramfunc :
  {
    . = ALIGN(4);
    _sramfunc = .;
    KEEP(*(.ramfunc))
    KEEP(*(.ramfunc*))
    . = ALIGN(4);
    _eramfunc = .;
  } >RAM AT> FLASH

  _sidata_ramfunc = LOADADDR(.ramfunc);

  .bss :
  {
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } >RAM

  ._user_heap_stack :
  {
    PROVIDE(end = .);
    PROVIDE(_end = .);
    . = . + _Min_Heap_Size + _Min_Stack_Size;
  } >RAM

  /* Reserved regions for system-wide access (NOLOAD) */
  .config_region (NOLOAD) :
  {
    __config_start__ = 0x0800C000; /* Sector 3 */
  } >FLASH

  .error_log_region (NOLOAD) :
  {
    __error_log_start__ = 0x08008000; /* Sector 2 */
  } >FLASH

  .cert_private_region (NOLOAD) :
  {
    __cert_private_start__ = 0x08004000; /* Sector 1 */
  } >FLASH

  .cert_public_region (NOLOAD) :
  {
    __cert_public_start__ = 0x080F8400; /* Sector 11 */
  } >FLASH

  .metadata_cert_region (NOLOAD) :
  {
    __metadata_start__ = 0x0807FC00; /* End of app region */
    __cert_start__     = 0x0807FE00;
  } >FLASH

  .update_metadata_cert_region (NOLOAD) :
  {
    __new_metadata_start__ = 0x080FFC00; /* End of update app */
    __new_cert_start__     = 0x080FFE00;
  } >FLASH

  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
