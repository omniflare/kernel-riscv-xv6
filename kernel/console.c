#include "console.h"

#define UART0 0x10000000L    // QEMU virt UART base address

static inline void uart_putc(char c ){
    *(volatile unsigned char *)(UART0 + 0) = c;
}

void console_init() {

}

void console_putc(char c) {
    if (c == '\n'){
        uart_putc('\r');
    }
    uart_putc(c);
}

void console_puts(const char* s){
    while (*s) {
        console_putc(*s ++);
    }
}
