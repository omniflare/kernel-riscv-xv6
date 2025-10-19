#include "string.h"

int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char *)a - *(const unsigned char *)b;
}

int strncmp(const char *a, const char *b, unsigned long n) {
    while (n && *a && (*a == *b)) {
        a++;
        b++;
        n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char *)a - *(const unsigned char *)b;
}

unsigned long strlen(const char *s) {
    unsigned long len = 0;
    while (*s++) len++;
    return len;
}

void strcpy(char *dst, const char *src) {
    while ((*dst++ = *src++))
        ;
}

void strncpy(char *dst, const char *src, unsigned long n) {
    while (n && (*dst++ = *src++))
        n--;
    while (n--)
        *dst++ = '\0';
}
