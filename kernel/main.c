#include "console.h"

int main () {
    console_init();
    console_puts("Hello from kernel");
    
    while (1) {
    // hang forever
    }
}