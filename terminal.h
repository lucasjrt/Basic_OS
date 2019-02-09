#ifndef TERMINAL_H
#define TERMINAL_H

typedef struct command Command;

void terminal_start();
void loop_terminal();
void start_commands();
int recognize(char* command);
void run(char *command, int index);

//Commands
void clearScreen();

#endif // TERMINAL_H
