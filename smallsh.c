#include "smallsh.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h> /* for strcmp */

/* program buffers and work pointers */
static char inbuf[MAXBUF], tokbuf[2*MAXBUF],
*ptr = inbuf, *tok = tokbuf;

char *prompt = "Command> ";

typedef enum { RUNNING, STOPPED } job_state;

/* Joblist linked list */
typedef struct job {
    int job_id; /* job id */
    pid_t pid; /* child process id */
    job_state state; /* running or stopped */
    int where; /* foreground 0 or background */
    struct job *next; /* link to next node */
} job_t;

/* head of the linked list */
static job_t *job_list_head = NULL;

/* counter to assign the job ids */
static int next_job_id = 1;

/* PID of the currrent foreground process (o if none) */
pid_t pid_foregrnd = 0;

/* Find the job with a matching PID, null if none found*/
static job_t *find_job_by_pid(pid_t pid)
{
    for (job_t *j = job_list_head; j; j = j->next) {
        if (j->pid == pid) return j;
    }
    return NULL;
}

/* removes node from linked list */
static void remove_job(job_t *r)
{
    job_t **pp = &job_list_head;
    while (*pp && *pp != r)
        pp = &(*pp)->next;
    if (*pp) {
        *pp = r->next;
        free(r);
    }
}

/* Handles SIGINT (Ctrl+C) by killing foreground process */
void sact_int(int signo)
{
    if (pid_foregrnd > 0) {
        kill(pid_foregrnd, SIGKILL);
        /* now remove it immediately from our job list */
        job_t *j = find_job_by_pid(pid_foregrnd);
        if (j) remove_job(j);
    }
    /* reprint prompt on its own line */
    write(STDOUT_FILENO, "\n", 1);
}

/* Handles SIGTSTP (Ctrl+Z) by stopping foreground process */
void sact_tstp(int signo)
{
    if (pid_foregrnd > 0) {
        kill(pid_foregrnd, SIGSTOP);
        /* mark it stopped and switch it to background */
        job_t *j = find_job_by_pid(pid_foregrnd);
        if (j) {
            j->state = STOPPED;
            j->where = BACKGROUND;
        }
    }
    /* reprint prompt on its own line */
    write(STDOUT_FILENO, "\n", 1);
}

/* update the values in the linked list */
/* by the time the all finsihed this function might onloy handle when the process ends naturally*/
static void handle_jobs(int signo)
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG|WUNTRACED|WCONTINUED)) > 0) {
        job_t *j = find_job_by_pid(pid);
        if (!j) continue;
        /* removing when a process naturallt ends (technically it only needs WIFEXITED to do that)*/
        /* the WIFSIGNALED i believe is checking for the ^C signal but we dont need to check that here*/
        /* can test to make sure and then remove WIFSIGNALED*/
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            /* BG job already so finished remove it (as in the 60 seconds are over)*/
            remove_job(j);
        } else if (WIFCONTINUED(status)) { /* mark as running on sigcont */
            /* i think this if statement will be removed later, it would be the FG command*/
            /* however its prob better to make it so it changes the state to running in the*/
            /* actual FG function, for example the ^C and ^Z do it in their own function*/
            j->state = RUNNING;
        }
    }
}

/* function to add jobs to linked list */
static void add_job(pid_t pid, int where)
{
    job_t *j = malloc(sizeof *j);
    j->job_id = next_job_id++;
    j->pid = pid;
    j->state = RUNNING;
    j->where = where;
    j->next = job_list_head;
    job_list_head = j;
}

/* this prints all the jobs with ‘jobs’ command */
static void jobs_cmd(void)
{
    for (job_t *j = job_list_head; j; j = j->next) {
        const char *st = (j->state == RUNNING) ? "Running" : "Stopped";
        const char *wg = (j->where == BACKGROUND) ? "Background" : "Foreground";
        printf("[%d] %d %s %s\n",
               j->job_id,
               j->pid,
               st,
               wg);
    }
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

    while (1) {
        if ((c = getchar()) == EOF)
            return EOF;

        if (count < MAXBUF)
            inbuf[count++] = c;

        if (c == '\n' && count < MAXBUF) {
            inbuf[count] = '\0';
            return count;
        }

        /* If line too long restart */
        if (c == '\n') {
            printf("smallsh: input line too long\n");
            count = 0;
            printf("%s", p);
        }
    }
}

static char special[] = {' ', '\t', '&', ';', '\n', '\0'};

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
int runcommand(char **cline, int where)
{
    pid_t pid;
    int status;
    struct sigaction sa_default, sa_ign;

    /* the signal actions we defined (the signal default) */
    sa_default.sa_handler = SIG_DFL;
    sigemptyset(&sa_default.sa_mask);
    sa_default.sa_flags = 0;

    /* ignore the signals */
    sa_ign.sa_handler = SIG_IGN;
    sigemptyset(&sa_ign.sa_mask);
    sa_ign.sa_flags = 0;

    switch (pid = fork()) {
      case -1:
        perror("smallsh");
        return -1;

      case 0:  /* child */
        setpgid(0, getpgrp()); /* returns the sheels PGID, making sure the child joins the shells process group */

        if (where == BACKGROUND) {
            /* ignore ^C and ^Z */
            sigaction(SIGINT, &sa_ign, NULL);
            sigaction(SIGTSTP, &sa_ign, NULL);
        } 
        else {
            /* for FOREGROUND, for ^C and ^Z*/
            sigaction(SIGINT, &sa_default, NULL); /* terminates process ^C*/
            sigaction(SIGTSTP, &sa_default, NULL); /* stops process ^Z */
        }

        execvp(cline[0], cline);
        _exit(1);
    }

    /* parent */
    /* makes sure the child is in the shells PGID*/
    setpgid(pid, getpgrp());

    /* record the foregeound pid*/
    pid_foregrnd = pid;

    /* adds process to jobs list */
    add_job(pid, where);

    if (where == FOREGROUND) {
        /* wait for exit or stop */
        /* if stopped (^Z), it stays in the list as STOPPED */
        if (waitpid(pid, &status, WUNTRACED) > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                /* will only enter here if the process was terminated ^C, and will be removed from the list */
                job_t *j = find_job_by_pid(pid);
                if (j) remove_job(j);
            }
        }
    }

    pid_foregrnd = 0; /* its done waiting  for a foreground process*/
    return status;
}
void procline(void)  /* Process input line */
{
    char *arg[MAXARG+1];
    int toktype, narg = 0, type;

    for (;;) {
        switch (toktype = gettok(&arg[narg])) {
            case ARG:
                if (narg < MAXARG) narg++;
                break;
            case EOL:
            case SEMICOLON:
            case AMPERSAND:
                if (toktype == AMPERSAND)
                    type = BACKGROUND;
                else
                    type = FOREGROUND;

                if (narg != 0) {
                    arg[narg] = NULL;
                    if (strcmp(arg[0], "jobs") == 0) {
                        jobs_cmd();
                    } else {
                        runcommand(arg, type);
                    }
                }

                if (toktype == EOL)
                    return;

                narg = 0;
                break;

        }
    }
}

int main()
{
    static struct sigaction sa;

    /* Creating SIGINT Handler */
    sa.sa_handler = sact_int;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    /* Creating SIGTSTP Handler */
    sa.sa_handler = sact_tstp;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa, NULL);

    /* SIGCHLD handler */
    sa.sa_handler = handle_jobs;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    while (userin(prompt) != EOF)
        procline();
    return 0;
}
