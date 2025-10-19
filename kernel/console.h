#ifndef CONSOLE_H
#define CONSOLE_H

void console_init();
void console_putc(char c);
void console_puts(const char *s);
int console_getc();

#endif
