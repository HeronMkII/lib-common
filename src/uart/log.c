#include <uart/log.h>

char buf[LOG_BUFFER_SIZE];
// UART must be initialized before calling print
inline int print(char *str, ...) {
    va_list ptr;
    va_start(ptr, str);

    int ret = vsprintf(buf, str, ptr);
    va_end(ptr);

    send_uart(buf);
    return ret;
}
