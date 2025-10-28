#include "console.h"
#include "string.h"
#include "types.h"
#include "fs.h"
#include "shell.h"

#define CMD_BUF_SIZE 128

void execute_command(const char *cmd);

static inline void mmio_write(uint64_t addr, uint64_t value) {
  *(volatile uint64_t *)addr = value;
}
extern void _start(void); //from entry.s

int main() {

  char buf[CMD_BUF_SIZE];
  int idx = 0;

  // Initialize filesystem
  fs_init();

  console_puts("Tiny RISC-V Kernel with Filesystem\n");
  console_puts("Type 'help' for commands.\n> ");
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
  // Parse command and arguments
  char command[64];
  const char *args = cmd;
  int i = 0;
  
  // Extract command (first word)
  while (*args && *args != ' ' && i < 63) {
    command[i++] = *args++;
  }
  command[i] = '\0';
  
  // Skip spaces to get to arguments
  while (*args == ' ') args++;
  
  // Execute commands
  if (strcmp(command, "help") == 0) {
    console_puts("Commands:\n");
    console_puts("  help         - prints this help\n");
    console_puts("  hello        - prints greeting\n");
    console_puts("  clear        - clears screen\n");
    console_puts("  ls           - list files\n");
    console_puts("  cat FILE     - display file contents\n");
    console_puts("  touch FILE   - create empty file\n");
    console_puts("  mkdir DIR    - create directory\n");
    console_puts("  cd DIR       - change directory\n");
    console_puts("  pwd          - print working directory\n");
    console_puts("  rm FILE      - remove file/directory\n");
    console_puts("  write FILE TEXT - write text to file\n");
    console_puts("  echo TEXT > FILE  - write text to file\n");
    console_puts("  echo TEXT >> FILE - append text to file\n");
    console_puts("  sh FILE      - execute shell script\n");
    console_puts("  reboot       - resets QEMU\n");
  } else if (strcmp(command, "hello") == 0) {
    console_puts("Hello, user!\n");
  } else if (strcmp(command, "clear") == 0) {
    console_puts("\033[2J\033[H");
  } else if (strcmp(command, "ls") == 0) {
    shell_ls(args);
  } else if (strcmp(command, "cat") == 0) {
    shell_cat(args);
  } else if (strcmp(command, "touch") == 0) {
    shell_touch(args);
  } else if (strcmp(command, "mkdir") == 0) {
    shell_mkdir(args);
  } else if (strcmp(command, "cd") == 0) {
    shell_cd(args);
  } else if (strcmp(command, "pwd") == 0) {
    shell_pwd();
  } else if (strcmp(command, "rm") == 0) {
    shell_rm(args);
  } else if (strcmp(command, "write") == 0) {
    shell_write(args);
  } else if (strcmp(command, "echo") == 0) {
    shell_echo(args);
  } else if (strcmp(command, "sh") == 0) {
    shell_sh(args);
  } else if (strcmp(command, "reboot") == 0) {
    console_puts("Rebooting...\n");
    void (*restart)(void) = _start;
    restart();
  } else if (command[0] != '\0') {
    console_puts("Unknown command. Type 'help'.\n");
  }
}
