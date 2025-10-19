#include "console.h"
#include "string.h"
#include "types.h"

#define CMD_BUF_SIZE 64

void execute_command(const char *cmd);

int main() {

  char buf[CMD_BUF_SIZE];
  int idx = 0;

  console_puts("Tiny RISC-V Kernel\nType 'help' for commands.\n> ");
  // console_init();
  // console_puts("Hello from kernel");
  //

  while (1) {
    int c = console_getc();
    if (c == '\r' || c == '\n') {
      console_putc('\n');
      buf[idx] = '\0';
      execute_command(buf);
      idx = 0;
      console_puts("> ");
    } else if (c == 127 || c == '\b') {
      if (idx > 0) {
        idx--;
        console_puts("\b \b");
      }
    } else if (idx < CMD_BUF_SIZE - 1) {
      buf[idx++] = (char)c;
      console_putc(c);
    }
  }
}

void execute_command(const char *cmd) {
    if (strcmp(cmd, "help") == 0) {
            console_puts("Commands:\n");
            console_puts("  hello  - prints greeting\n");
            console_puts("  clear  - clears screen\n");
            console_puts("  echo X - prints X\n");
            console_puts("  reboot - resets QEMU\n");
        } else if (strcmp(cmd, "hello") == 0) {
            console_puts("Hello, user!\n");
        } else if (strcmp(cmd, "clear") == 0) {
            // ANSI escape sequence for clear
            console_puts("\033[2J\033[H");
        } else if (strncmp(cmd, "echo ", 5) == 0) {
            console_puts(cmd + 5);
            console_putc('\n');
        } else if (cmd[0] != '\0') {
            console_puts("Unknown command. Type 'help'.\n");
        }
}
