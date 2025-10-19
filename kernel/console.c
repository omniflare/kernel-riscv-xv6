#include "console.h"

#define UART0 0x10000000L    // QEMU virt UART base address
#define UART0_LSR (UART0 + 5)   // Line Status Register
#define UART_LSR_RX_READY 0x01  // Receiver data ready

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

int console_getc(){
    while ((*(volatile unsigned char *)UART0_LSR & UART_LSR_RX_READY) == 0);
    return *(volatile unsigned char *)(UART0 + 0);
}
