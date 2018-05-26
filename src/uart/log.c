#include <uart/uart.h>

uint8_t print_buf[PRINT_BUF_SIZE];

// UART must be initialized before calling print
inline int print(char *str, ...) {
    va_list ptr;
    va_start(ptr, str);

    int ret = vsprintf((char *)print_buf, str, ptr);
    va_end(ptr);

    send_uart(print_buf, strlen((char*)print_buf));
    return ret;
}

inline int uprintf(char *str, ...) {
    va_list ptr;
    va_start(ptr, str);

    int ret = vsprintf((char *)print_buf, str, ptr);
    va_end(ptr);

    send_uart(print_buf, strlen((char*)print_buf));
    return ret;
}

