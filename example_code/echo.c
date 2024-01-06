#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
  if (argc >= 2) {
    /* First argument printed without leading space */
    fputs(argv[1], stdout);
    for (int i = 2; i < argc; ++i) {
      /* Subsequent arguments padded on left with a leading space */
      putchar(' ');
      fputs(argv[i], stdout);
    }
  }
  putchar('\n');
  return EXIT_SUCCESS;
}
