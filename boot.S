.section .note.GNU-stack,"",@progbits
.section .text
.align 4
.long 0x1BADB002
.long 0
.long -(0x1BADB002)

.global _start
_start:
    call kernel_main
    cli
    hlt
