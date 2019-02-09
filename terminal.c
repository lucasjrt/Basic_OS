#include <stdlib.h>
#include <stdio.h>

#include "terminal.h"
#include "stringHelper.h"
#include "utils.h"

#define PAGE_SIZE 4096
#define MAX_COMMANDS 4096
#define clear() system("cls")
#define read(x) fflush(stdin); scanf("%s", (char*) &x) //Only works for reading strings

struct command {
    char name[PAGE_SIZE / 4];
    void (*function)();
    char params; //1 for true, 0 for false
};

char *pwd;
char command[PAGE_SIZE] = "";
Command command_list[MAX_COMMANDS];

void terminal_start() {
    clear();
    pwd = new("char", PAGE_SIZE);
    start_commands();
    setString(pwd, "/");
    printf("Terminal [version 1.0 alpha]\n%s> ", pwd);
    loop_terminal();
}

void loop_terminal() {
    while (1) {
        read(command);
        int index = recognize(command);
        if(index < 0 && !compare(command, "exit"))
            printf("Command not recognized\n");
        else if(compare(command, "exit"))
            break;
        else
            run(command, index);
        printf("%s> ", pwd);
    }
}

void start_commands() { //Start default commands
    //Clear
    setString(command_list[0].name, "clear");
    command_list[0].function = clearScreen;
    command_list[0].params = 1;
}

int recognize(char* command) {
    int i;
    for(i = 0; i < PAGE_SIZE; i++)
        if(compare(command_list[i].name, command)) return i;
    return -1;
}

void run(char *command, int index) {
    command_list[index].function();
}

//Commands
void clearScreen() {
    clear();
}
