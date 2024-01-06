Base64 
======

In this assignment, you will write a simple base64 encoding utility to familiarize yourself with the basics of C programming. You will be using, exclusively, a small subset of the C standard library to accomplish this task. You will practice:

* Parsing command-line arguments
* Using the standard io library features
* Handling errors
* Manipulating data and working with arrays

Learning Outcomes
-----------------

* How to write a C program to solve a problem? (Module 1 MLO 4)
* How can programs invoke OS services using system calls? (Module 1 MLO 5)
* How do you interact with the user in C programs? (Module 1 MLO 6)
* How are C programs transformed into an executable form? (Module 1 MLO 7)

Specification
-------------

NAME
   ``base64`` - Base64 encode data and print to standard output

SYNOPSIS
   ``base64 [FILE]``

DESCRIPTION
   Base64 encode ``FILE``, or standard input, and output to standard output.

   With no ``FILE``, or if ``FILE`` is ``-``, read from standard input.

   Encoded lines are wrapped every 76 characters.

   The data are encoded according to the standard algorithm and standard base64 alphabet described in `RFC 4648 <https://datatracker.ietf.org/doc/html/rfc4648>`_.

STDIN
   The standard input shall be used only if no ``FILE`` operand is specified, or if the ``FILE`` operand is ``-``.

STDOUT
   The standard output shall contain the base64 encoded output, wrapped to 76 characters.

STDERR
   The standard error shall be used only for error reporting. All errors must be reported to standard error.

EXIT STATUS
   0, if successful.

   >0, if an error occurs.

Additional Requirements
-----------------------

* Your program may not use Variable Length Arrays (VLAs), which are declared with a non-constant expression as the array size (e.g. ``char input[n]``). The compiler option ``-Werror=vla`` will prevent compilation with VLAs.
* Your program must compile against the c99 standard, using the compiler option ``-std=c99``.

In other words,

``gcc -std=c99 -Werror=vla -o base64 ...``

The provided makefile compiles with the appropriate options, and you do not need to modify it,

.. code-block:: console

   $ make -B release
   gcc  -std=c99 -Wall -Werror=vla -Werror -O3 -c -o build/release/base64.o src/base64.c
   gcc  -o build/release/base64 build/release/base64.o
   $ make -B debug
   gcc  -std=c99 -Wall -Werror=vla -g -c -o build/debug/base64.o src/base64.c
   gcc  -o build/debug/base64 build/debug/base64.o  

Gradescope will compile your program using its own copy of the makefile; you need only to upload your *.c source files and nothing else.

Additionally, your program must run in **constant space complexity** while accepting input of any arbitrary length. In other words, you may not dynamically allocate memory. The idea is that your program will process input in small chunks, until reaching end of input. Do not attempt to buffer the entire input before encoding it. If found to violate this requirement, you will receive a 0.

When exiting due to an error, your program must print an informative error message to the standard error stream, ``stderr`` and exit with a non-zero exit status. You are encouraged to use the convenience function ``err(int exit_val, char const *fmt, ...)`` exposed by the ``<err.h>`` header file, which both exits the program and prints an informative error message in a single operation. On the same note, you must not print to ``stderr`` and must exit with 0 or ``EXIT_SUCCESS`` if you don't encounter any errors.

Your program must not have any memory leaks; since you are prohibited from dynamically allocating space for input, the only place you should worry about encountering a memory leak is with opened files. See the advice below for ensuring you meet this requirement.

Explanation
-----------

Suppose you wish to transmit an arbitrary stream of data over a medium that only supports plaintext data, such as email or physical print. A simple method would be to convert each byte of data into an eight-character string of '1' and '0' characters representing its binary value--base2.

Take, for example, the message ``"foo"``; as an ASCII encoded string, this consists of byte values: ``{102, 111, 111}``, which are represented in binary as s: ``{0b01100110, 0b01101111, 0b01101111}``

In a base2 scheme, this would be encoded and transmitted as the string 
``"011001100110111101101111"``.

You're probably also familiar with hexadecimal (base16), which would encode each nibble (half-byte) as a single character: ``"666f6f"``.

Another popular representation is octal (base8), which encodes groups of tribbles (three bits) as a single character. However, unlike base2 and base16, base8 has an issue:

* LCM(log2(2), 8)/8  = LCM(1, 8)/8 = 8/8 = 1
* LCM(log2(8), 8)/8 = LCM(3, 8)/8 = 24/8 = 3
* LCM(log2(16), 8)/8 = LCM(4, 8)/8 = 8/8 = 1

Notice that the least common multiple of 3 (the number of bits in a tribble) and 8 (the number of bits in a byte) is 24; this means, if we want to process only whole tribbles, we need an input that is a multiple of three bytes. The solution to this is to pad the input with additional null bytes until it is a multiple of 3 bytes. Then, after performing the encoding, any extra bytes that resulted solely from the padding bytes are simply replaced with a padding character, such as ``'='``.

As an example, three zero bytes (``{0, 0, 0}``) would encode as ``"00000000"`` in base8, two zero bytes (``{0, 0}``) would encode as ``"000000=="``, and one zero byte (``{0}``) would encode as ``"000====="``.

Notice that, the larger the base, the fewer characters needed to encode; for example, a hypothetical base256 scheme would take exactly the same amount of space--but there's a problem. We only have 96 printable characters in the ASCII character set. The largest power of 2 less than 96 is 64, so it turns out that base64 is the most compact of all the baseN encoding methods†. For this reason, base64 encoding is the standard used for a variety of important protocols, such as transmitting encryption keys as plaintext. Email attachments are also base64 encoded, as is raw image data embedded in web pages, for other examples where this tends to pop up.

† Technically speaking, base96 is the most compact printable encoding, but log2(96) is an irrational number, which implies that this is a very computationally expensive operation (perhaps you can prove to yourself why that is)! By the way, this is why most calculators use binary-encoded decimal, where each decimal digit is represented by a 4-bit value (0-10). It's less compact than a pure binary representation, but the transformation between BCD and base10 is trivial whereas the transformation between binary base2 and decimal base10 requires expensive division--again, because log2(10) is an irrational number.
Base64 has the same problem as base8, because,

* LCM(log2(64), 8)/8 = LCM(6, 8)/8 = 25/8 = 3

For this reason, Base64 also has to read input in chunks of three bytes at a time, and use padding when an input is not a multiple of three bytes in length.

General Advice
--------------

In this program, you will be working with arbitrary input data as raw bytes, using bitwise arithmetic operations, as described in the base64 specification.

The basic structure of your program should be,


* select input stream
* loop:

  * read input bytes
  * convert input bytes to integer indices
  * convert integer indices to base64 alphabet characters
  * write output
  * handle line wrapping

* cleanup

Skeleton code has been provided for you in the ``src/`` directory. Additional example programs are provided in the ``example_code/`` directory. 

The provided makefile has several targets: all, release, debug, examples, and test. These should be self explanatory; usage is like so, if you are unfamiliar:

.. code-block:: console

   $ make      # 'all' is implicit here
   $ make all
   $ make release
   $ make debug
   etc...

Add executable scripts to the tests/ directory, or modify the existing test script to write your own tests as you work on your program.

Common Issues
^^^^^^^^^^^^^

* Always error check function calls. Don't assume a request to open a file or read/write data was successful.
* You do not need to manipulate the filename argument, if there is one. You can pass it straight to a function like ``fopen()``.
* Thoroughly read the man pages for the functions you are using. They're all in section 3, so ``$ man 3 funcname``... 

The most common mistakes

* Only testing with text input / not testing with random data
* Only testing small inputs, not being strategic/thoughtful about input size
* Ignoring edge cases
* Wrong exit code, printing errors to stdout, printing output to stderr
* Not catching errors

Library Methods
---------------

Comparing Strings
^^^^^^^^^^^^^^^^^

The ``strcmp()`` function returns the **difference** between the first different character between two strings. Thus, ``strcmp(a, b)`` returns **0** when ``a`` and ``b`` point to the same string.

Hint:

.. code-block:: c

   if (strcmp(filename, "-") == 0) {
     puts("The file name IS \"-\"");
   } else {
     puts("The file name IS NOT \"-\"");
   }

Selecting Input
^^^^^^^^^^^^^^^

I recomend writing your program to process input from ``stdin``. In the preamble of your ``main()`` function, you may reopen ``stdin`` to point at a provided FILE argument using the ``freopen()`` library method. Note, you cannot legally reassign the standard streams to point elsewhere (e.g. ``stdin = x``). You must use ``freopen()`` to change standard streams in place:

To illustrate, this would be illegal:

.. code-block:: c

   fclose(stdin);
   void *result = fopen("my_file", "r");
   if (!result) err(EXIT_FAILURE, "my_file);
   stdin = result; /* This is an illegal reassignment of stdin */
   
The correct method:

.. code-block:: c

   void *result = freopen("my_file", "r", stdin);
   if (!result) err(EXIT_FAILURE, "my_file");

If you use ``freopen()`` on ``stdin``, you *do not* need to later close ``stdin``, since no new resources were allocated. 

If you instead use an approach like,

.. code-block:: c

   FILE *fp = stdin;
   if (filename) {
     fp = fopen(filename, "r");
     /* ... */

Then you must later close the newly opened file, since ``fopen()`` is a resource allocation.

Performing i/o
^^^^^^^^^^^^^^
  
For reading and writing, the standard i/o library provides four basic types of i/o: character, string, block, and formatted. There are several variations of each type for practical and historical reasons.

For example, the character input methods ``fgetc()``, ``getc()``, and ``getchar()`` all read a single byte of data from a stream, but differ in subtle ways--\ ``getc()`` may be a macro, and ``getchar()`` does not take a stream argument; instead it reads from ``stdin``.

For this program, you are reading and writing fixed-size blocks of data. The character and block i/o methods are made for working with arbitrary data; do not use the string methods (``fgets()``, ``fputs()``, etc.) nor the formatted methods (``scanf()``, ``printf()``, etc.) for working with arbitrary data.

To get to the point, you should use the block i/o ``fread()`` and ``fwrite()`` when working with more than one byte at a time, and the character i/o when working byte-by-byte. Character i/o is significantly slower than block i/o for large amounts of data, so don't abuse it!

For historical reasons, the ``fread()`` and ``fwrite()`` functions accept both a ``size`` and ``nmemb`` argument; ``size`` should always be ``1`` and ``nmemb`` should be the amount of bytes requested to be read or written:

.. code-block:: c

   size_t n = fread(buf, 1, sizeof buf, stdin);
   /* ... */
   fwrite(buf, 1, n, stdout);

Also, ``ferror()`` and ``feof()`` have indeterminate results prior to first i/o, so this,

.. code-block:: c

   while (!feof(stdin)) {
     /* ... */
   }

is always wrong. Instead, exit from the middle of the loop after a failed i/o:

.. code-block:: c

   for (;;) {
     size_t n = fread(buf, 1, sizeof buf, stdin);
     if (n < sizeof buf) {
       if (ferror(stdin)) err(1, "stdin");
     }
     if (n == 0 && feof(stdin)) break;

     /* ... */

     if (n < sizeof buf && feof(stdin)) break;
   }

Programming Fundamentals
------------------------

Bitwise Arithmetic
^^^^^^^^^^^^^^^^^^

You are expected to be familiar with bitwise arithmetic from your prior coursework. When working with bitwise operators in C, be aware that bitwise operations on negative *values* are generally undefined or produce implementation-defined results.

The best way to avoid this issue is to always use an unsigned data type when performing bitwise arithmetic; technically you can use signed types if you ensure they hold positive values--and actually C will implicilty convert your narrow types like ``unsigned char`` to ``int``, but this is not a problem because the conversion always produces a positive value. When writing constants, use the suffix ``u`` or ``U`` (they are identical, it's a matter of preference) to explicitly specifiy its type as an unsigned one.

Do not under any circumstances use the plain ``char`` type for anything other than ASCII characters in the range of values 0-127. It is unspecified whether ``char`` is signed or unsigned, so it is not possible to portably store values larger than 127 or less than 0 in a ``char``. Since you are *reading* arbitrary input data this means,

.. code-block:: c

   unsigned char input[...];
   fread(input, 1, sizeof input, stdin);
   
On the other hand, when you are writing your encoded output, it only contains ASCII characters, so you *should* use plain ``char``:

.. code-block:: c

   char output[...];
   fwrite(output, 1, sizeof output, stdout);

The encoding algorithm described in the base64 RFC transforms raw bytes (``unsigned char``) into ASCII characters (``char``)-- that's the whole point of it! Review the ``encoding.c`` example code to see implementations of similar encoding schemes.

Circular Counters
^^^^^^^^^^^^^^^^^

Never use modular arithmetic if you don't have to. Less than 10% of graduates can correctly identify the problem with this code:

.. code-block:: c

   int x = 0;
   for (;;) {
     /* ... */
     if (++x % 76) {
     /* ... */
     }
   }

First, division is ridiculously expensive and slow. Second, what happens when ``x`` inevitably overflows? These are the sorts of things that set apart programmers in interviews--think carefully about what you are doing.

Keep it simple:

.. code-block:: c

   int x = 0;
   for (;;) {
     /* ... */
     if (++x >= 76) {
       x = 0;
       /* ... */
     }
   }

Testing Your Program
--------------------

**WARNING** There is a system ``base64`` utility; you can use it to compare your output for correctness. If you run the command ``base64``, the shell will search system binary directories and run *this* utility. To run *your* program, you must explicitly specify a path to it containing at least one forward slash. For example, ``$ build/release/base64``. Or, if the program is in the current directory, you can use the ``.`` pathname to explicitly reference the current directory: ``$ ./base64``.

Test your program using input redirection (``$ base64 < testfile``) or a file argument (``$ base64 testfile``). Do not try to test it interactively; you will get confused by how the terminal processes your input.

Standard Input
^^^^^^^^^^^^^^

For short inputs, use shell pipes from the ``printf`` program,

.. code-block:: console

   $ printf 'foobar' | build/.../base64

Do **not** use ``echo`` in place of ``printf``.

You can also test non-printable characters,

.. code-block:: console

   $ printf '\x00\x01\x02\x03' | build/.../base64

For longer inputs, you can use shell redirection,

.. code-block:: console

   $ build/.../base64 <filename

File Arguments
^^^^^^^^^^^^^^

Be sure to test your program with file arguments as well,

.. code-block:: console

   $ build/.../base64 filename

Don't forget to test other things...

.. code-block:: console

   $ ./base64 filename extra_filename
   $ ./base64 filename <ignored_stdin
   $ ./base64 - <not_ignored_stdin
   etc...

Rubric
------

[5 points] Compiles strictly

[5 points] Correct output for 0 - 57 input bytes in multiples of 3

[5 points] Correct output for 0 - 57 input bytes

[15 points] Correct formatting for input of any length

[20 points] Correct output for input of any length that is a multiple of 3

[10 points] Correct output for input of any length

[5 points] Recognizes "-" FILE argument correctly

[5 points] Ignores any input on stdin if FILE argument is provided

[5 points] Incorrect number of arguments error handling

[5 points] Read from input file error handling

[5 points] Failure to open FILE argument error handling

[5 points] No memory/resource leaks

[10 points] Never crashes during testing


What to Submit
--------------

Submit your code to gradescope. Gradescope only wants your \*.c source file(s). It doesn't matter what you call them, but don't upload anything else, and don't upload anything with a directory structure.
