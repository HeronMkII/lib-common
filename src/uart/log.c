#include <uart/log.h>

// TODO: should this be volatile
uint8_t log_buf[LOG_BUF_SIZE];

// UART must be initialized before calling print
inline int print(char *str, ...) {
    va_list ptr;
    va_start(ptr, str);

    int ret = vsprintf((char *)log_buf, str, ptr);
    va_end(ptr);

    send_uart(log_buf, strlen((char*)log_buf));
    return ret;
}
