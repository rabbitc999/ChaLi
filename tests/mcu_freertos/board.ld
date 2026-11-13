ENTRY(reset)
MEMORY {
    FLASH (rx) : ORIGIN = 0, LENGTH = 4M
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 4M
}
SECTIONS {
    .vectors : { KEEP(*(.vectors)) } > FLASH
    .text : { *(.text*) *(chali_code) *(.rodata*) } > FLASH
    .ARM.exidx : { *(.ARM.exidx*) } > FLASH
    .data : { data_start = .; *(.data*) data_end = .; } > RAM AT > FLASH
    data_load = LOADADDR(.data);
    .bss (NOLOAD) : { bss_start = .; *(.bss*) *(COMMON) bss_end = .; } > RAM
    stack_top = ORIGIN(RAM) + LENGTH(RAM);
}
