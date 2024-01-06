#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char *argv[])
{
  int i = 1; /* Notice we start at 1. argv[0] is the name of the program.
                e.g. `cmd arg1 arg2...` will result in argv[0] == "cmd",
                                                       argv[1] == "arg1",
                                                       argv[2] == "arg2",
                                                       and so on... */
  char const *filename;
  FILE *fp;

  /* When a command is run in the shell, the standard streams (input,
   * output, and error) are inherited from the shell, which may reassign
   * these streams when provided with redirection operators. For example,
   *
   * `cat file` -> argv = {"cat", "file"}... explicit filename provided
   * `cat <file` -> argv = {"cat"}... shell redirects stdin to point to file,
   *                                  cat reads from stdin
   */
  if (argc < 2) {
    /* If no FILE, read from standard input */
    filename = "-"; /* Customary reported name for stdin */
    fp = stdin;
    goto dfl_stdin;
  }

  for (; i < argc; ++i) {
    filename = argv[i];

    if (strcmp(filename, "-") == 0) {
      /* If FILE is "-", read from standard input */
      fp = stdin;
    } else {
      /* Open the file */
      fp = fopen(filename, "r");

      /* Error checking standard library methods is _mandatory_. Read the
       * man page sections RETURN VALUE and ERRORS for _every_ function
       * you use.
       *
       * Access directly in vim, with `:Man fopen`, etc.
       */
      if (!fp) err(EXIT_FAILURE, "%s", filename);
    }
  dfl_stdin:;
    char buf[BUFSIZ]; /* BUFSIZE macro is a "pretty good" size for a buffer
                         meant to be used for fast i/o. Typically a small
                         multiple of the system's page size (which is typically
                         4kB)--4096, 8092, etc... */
    for (;;) {
      size_t nr = fread(buf, 1, sizeof buf, fp);
      if (nr < sizeof buf && ferror(stdin)) err(EXIT_FAILURE, "%s", argv[1]);
      if (nr == 0) break; /* end of file, empty buffer */

      size_t nw = fwrite(buf, 1, nr, stdout);
      if (nw < nr) err(EXIT_FAILURE, "stdout");

      if (nr < sizeof buf) break; /* end of file, partial buffer */
    }
    if (fp != stdin)
      fclose(fp); /* Opening a file allocates resources (file descriptor, FILE
                     object, etc...). All opened files must be closed to free
                     those resources. */
  }
  fflush(stdout);
  if (ferror(stdout)) err(EXIT_FAILURE, "stdout");
  return EXIT_SUCCESS;
}
