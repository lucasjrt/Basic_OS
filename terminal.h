#ifndef TERMINAL_H
#define TERMINAL_H

typedef struct command Command;

void terminal_start();
void loop_terminal();
void start_commands();
int recognize(char* command); //Tests if a command is on the command_list; 1 for true, 0 for false
void* run(char *command, int index); //Runs simple command
void* param_run(char *command, int index, char** params, int num_params); //Runs commands with parameters

//Commands
void clearScreen(void* ret);
void print_pwd(void* ret);
void echo(char** params, int num_params, void* ret);
void help();

#endif // TERMINAL_H
