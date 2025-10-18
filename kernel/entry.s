    .section .text
    .globl _start

_start:
    # Set up stack pointer
    la sp, stack_top

    # Call main()
    call main

hang:
    j hang  # Infinite loop if main() returns

    .section .bss
    .align 4
stack:
    .space 4096 * 4    # 16KB stack
stack_top:
