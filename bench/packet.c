#include "packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <wchar.h>

typedef unsigned long long ullong;

ullong ms(void) {
  struct timeval time;
  gettimeofday(&time, NULL);
  return ((unsigned long long)time.tv_sec * 1000000) + time.tv_usec;
}

wchar_t *str(void) {
  return L"foobar baz\nqux=qax\n\nbutts";
}

int main() {
  ullong t = ms();
  for(int i = 0; i < 500000; i++) {
    pkt_parse(str());
  }
  printf("Parsed 500,000 packets in %fs.\n", ((double)(ms() - t)) / 1000000);
  return 0;
}
