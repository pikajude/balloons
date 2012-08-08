#include "logger.h"

void logger(color c, const char *msg) {
    printf(c < 0 ? "\033[%d;1m%s\033[0m" : "\033[%dm%s\033[0m", abs(c), msg);
}
