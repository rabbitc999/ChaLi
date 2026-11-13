.syntax unified
.thumb
.section .vectors,"a",%progbits
.word stack_top, reset
.rept 9
.word 0
.endr
.word vPortSVCHandler, 0, 0, xPortPendSVHandler, xPortSysTickHandler
