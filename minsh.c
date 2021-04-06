/*
 * Minimal Shell Project 
 *
*/

/*** includes ***/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "minsh.h"

/*** defines ***/
#define TRUE 1
#define BUFSIZE 1024
#define TOK_BUF 64


/*** shell builtin functions implementations. ***/
int total_number_of_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/* cd function is needed to be builtin function, because every process has a working directory in linux, cannot change working directory from outside of a process.*/
int func_cd(char **args)
{
  if (args[1] == NULL) { fprintf(stderr, "expected argument for cd \n"); }

  else
  {
    if (chdir(args[1]) != 0) { perror("minsh"); }
  }
  return 1;
}

int func_help()
{
  int i;
  printf("minsh help test \n");
  printf("builtin functions : ");
  for (i = 0; i < total_number_of_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }
  return 1;
}

int func_exit()
{
  exit(EXIT_SUCCESS);
}

void handle_error(const char *prompt)
{
    perror(prompt);
    exit(EXIT_FAILURE);
}

/*** initialization ***/
int main()
{
    main_loop();
}

int execute_loop(char **args)
{
  int i;

  if (args[0] == NULL) { return 1; }

  for (i = 0; i < total_number_of_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }
  return exec_cmd(args);
}

/*main shell loop*/
void main_loop()
{
    char *line;
    char **args;
    while(TRUE)
    {

        printf("minsh -> ");
        line = read_line();
        /*debug*/
        //printf("line is %s \n",line);
        args = split_line(line);
        execute_loop(args);

        free(line);
        free(args);
    }
}

/*splits line by whitespace*/
char** split_line(char* line)
{
    int pos = 0;
    int bufsize = TOK_BUF; //64 bytes
    char *tok;
    char **toks;

    tok = strtok(line, " ");
    toks = malloc(bufsize * sizeof(char *));
    while(tok != NULL)
    {
        /*debug*/
        //printf("%s\n", to);
        toks[pos] = tok;
        pos++;

        /*handle memory allocation problem*/
        if(pos >= bufsize)
        {
            /*double the size of the buffer if pos is more than buf*/
            bufsize += TOK_BUF;
            toks = realloc(toks, bufsize *sizeof(char*));
            if(!toks)
            {
                fprintf(stderr, "minsh : alloc error ! \n");
                exit(EXIT_FAILURE);
            }
        }
        tok = strtok(NULL, " ");
    }
    toks[pos] = NULL;
    return toks;
}
/*read user input char by char until eof or newline*/
char* read_line()
{
    int bufsize = BUFSIZE;
    char *line = (char*)malloc(sizeof(char*) * BUFSIZE);
    int i = 0;
    char c;
    while(TRUE)
    {
        c = getchar();
        if(c == EOF || c == '\n') { line[i] = '\0'; return line; }
        else
        {
            if(i >= bufsize)
            {
                bufsize += BUFSIZE;
                line = realloc(line, bufsize * sizeof(char*));
                if(!line)
                {
                    fprintf(stderr, "minsh : alloc error !\n");
                    exit(EXIT_FAILURE);
                }
            }
            line[i] = c;
        }
        i++;
    }
}

/*execute splitted input*/
int exec_cmd(char **args)
{
    /*create a new process, execute arguments comes from split line with new process, wait until it finishes, then terminate the process*/
    pid_t child_ps, wait;
    int waitstatus;

    switch (child_ps = fork())
    {
    case -1:
        perror("cannot create child process");
        exit(EXIT_FAILURE);
    case 0:
        /*code to be executed by child*/
        if (execvp(args[0], args) == -1)
        {
            handle_error("execvp err ");
        }
        break;
    case 1:
        /*code to be executed by parent*/
        wait = waitpid(child_ps, &waitstatus, WUNTRACED | WCONTINUED);
        if(wait == -1) { perror("waitpid"); exit(EXIT_FAILURE); }
        do
        {
            if(WIFEXITED(waitstatus)) {printf("exited\n");}
            else if(WIFSIGNALED(waitstatus)) {puts("signaled");}

        }while(!WIFEXITED(waitstatus) && !WIFSIGNALED(waitstatus));

    }
    /*indicates successful execution.*/
    return 1;
}
