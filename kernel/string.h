#ifndef STRING_H
#define STRING_H

int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, unsigned long n);
unsigned long strlen(const char *s);
void strcpy(char *dst, const char *src);
void strncpy(char *dst, const char *src, unsigned long n);


#endif