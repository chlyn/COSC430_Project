#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#define EOL         1   /* end of line */
#define ARG         2   /* normal aruguments */
#define AMPERSAND   3 
#define SEMICOLON   4

#define MAXARG      512 /* max. no. command args */
#define MAXBUF      512 /* max. length input line */

#define FOREGROUND  0
#define BACKGROUND  1