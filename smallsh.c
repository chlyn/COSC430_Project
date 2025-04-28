#include "smallsh.h"
#include <stdlib.h>

/* program buffers and work pointers */
static char inbuf[MAXBUF], tokbuf[2*MAXBUF],
    *ptr = inbuf, *tok = tokbuf;

char *prompt = "Command> ";

volatile pid_t pid_foregrnd = 0;

/* Handles SIGINT (Ctrl+C) by killing foreground progress */
void sact_int(int signo)
{
    if (pid_foregrnd > 0)
    {
        kill(pid_foregrnd, SIGKILL);
    }

    /* Printing new line for the Command> prompt */
    write(STDOUT_FILENO, "\n ", 1);
}

/* Handles SIGTSPT (Ctrl+C) by stopping foreground progress */
void sact_tstp(int signo)
{
    if (pid_foregrnd > 0)
    {
        kill(pid_foregrnd, SIGSTOP);
    }

    write(STDOUT_FILENO, "\n", 1);
}

/* Print prompt and read a line */
int userin(char *p)
{
    int c, count;

    /* Initialization for later routines */
    ptr = inbuf;
    tok = tokbuf;

    /* Display prompt */
    printf("%s", p);

    count = 0;

    while(1)
    {
        if((c = getchar()) == EOF)
            return(EOF);

        if(count < MAXBUF)
            inbuf[count++] = c;

        if(c == '\n' && count < MAXBUF)
        {
            inbuf[count] = '\0';
            return count;
        }

        /* If line too long restart */
        if(c == '\n')
        {
            printf("smallsh: input line too long\n");
            count = 0;
            printf("%s", p);
        }
    }
}

static char special [] = {' ', '\t', '&', ';', '\n', '\0'};

int inarg(char c)
{
    char *wrk;

    for(wrk = special; *wrk; wrk++)
    {
        if(c == *wrk)
            return (0);
    }

    return (1);
}

/* Get token, place into tokbuf */
int gettok(char **outptr)
{
    int type;

    /* Set the outptr string to tok */
    *outptr = tok;

    /* Strop white space from the buffer containing the tokens */
    while(*ptr == ' ' || *ptr == '\t')
        ptr++;

    /* Set the token pointer to the first token in the buffer */
    *tok++ = *ptr;

    /* Set the type variable depending on the token in the buffer */
    switch(*ptr++)
    {
        case '\n':
            type = EOL;
            break;
        case '&':
            type = AMPERSAND;
            break;
        case ';':
            type = SEMICOLON;
            break;
        default:
            type = ARG;
            /* keep reading valid ordinary characters */
            while(inarg(*ptr))
                *tok++ = *ptr++;
    }

    *tok++ = '\0';
    return type;
}

/* Execute a command with optional wait */
int runcommand(char **cline, int where)
{
    pid_t pid;
    int status;
    static struct sigaction sact;

    switch(pid = fork())
    {
        case -1:
            perror("smallsh");
            return(-1);

        case 0:
            /* Code for child */
            /* Resetting signal handlers to default */
            sact.sa_handler = SIG_DFL;
            sigemptyset(&sact.sa_mask);
            sact.sa_flags = 0;
            sigaction(SIGINT, &sact, NULL);
            sigaction(SIGTSTP, &sact, NULL);

            execvp(*cline, cline);
            perror(*cline);

            exit(1);
    }

    /* Code for parent */
    /* If background process print pid and exit */
    pid_foregrnd = pid;
    if(waitpid(pid, &status, 0) == -1)
        return(-1);

    pid_foregrnd = 0;

    return(status);
}

void procline(void)          /* Process input line */
{
    char *arg[MAXARG + 1];  /* Pointer array for runcommand */
    int toktype;            /* Type of token in command */
    int narg;               /* Number of arguments so far */
    int type;               /* FOREGROUND or BACKGROUND */

    narg=0;

    for(;;)                 /* Loop forever */
    {
        /* Take action according to token type */
        switch(toktype = gettok(&arg[narg]))
        {
            case ARG: 
                if(narg < MAXARG)
                    narg++;
                break;

            case EOL:
            case SEMICOLON:
            case AMPERSAND:
                if (toktype == AMPERSAND)
                    type = BACKGROUND;
                else
                    type = FOREGROUND;

                if(narg != 0)
                {
                    arg[narg] = NULL;
                    runcommand(arg, type);
                }

                if(toktype == EOL)
                    return;

                narg = 0;
                break;
                        
        }
    }
}

int main()
{
    static struct sigaction sact;

    /* Creating SIGINT Handler */
    sact.sa_handler = sact_int;
    sigemptyset(&sact.sa_mask);
    sact.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sact, NULL);

    /* Creating SIGTSTP Handler */
    sact.sa_handler = sact_tstp;
    sigemptyset(&sact.sa_mask);
    sact.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sact, NULL);

    while(userin(prompt) != EOF)
        procline();
}

