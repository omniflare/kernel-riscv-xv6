#ifndef SHELL_H
#define SHELL_H

// Shell command interface
void shell_ls(const char *args);
void shell_cat(const char *args);
void shell_touch(const char *args);
void shell_mkdir(const char *args);
void shell_cd(const char *args);
void shell_sh(const char *args);
void shell_pwd(void);
void shell_rm(const char *args);
void shell_write(const char *args);
void shell_echo(const char *args);

#endif
