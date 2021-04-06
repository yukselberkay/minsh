#ifndef MINSH_H
#define MINSH_H

/*** function prototypes ***/
void handle_error(const char* prompt);
void main_loop();
char* read_line();
char** split_line(char* line);
int exec_cmd(char **args);

int total_number_of_builtins();
int func_cd(char **args);
int func_help();
int func_exit();

char *builtin_str[] = {
  "help",
  "cd",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &func_help,
  &func_cd,
  &func_exit
};

#endif // MINSH_H
