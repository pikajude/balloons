#include "logger.h"

void logger(color c, const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    printf(c < 0 ? "\033[%d;1m" : "\033[%dm", abs(c));
    vprintf(msg, args);
    va_end(args);
    printf("\033[0m");
}
