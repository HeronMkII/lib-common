#include <uart/log.h>

// TODO: should this be volatile
uint8_t buf[LOG_BUFFER_SIZE];

// UART must be initialized before calling print
inline int print(char *str, ...) {
    va_list ptr;
    va_start(ptr, str);

    int ret = vsprintf((char *)buf, str, ptr);
    va_end(ptr);

    send_uart(buf, strlen((char*)buf));
    return ret;
}
