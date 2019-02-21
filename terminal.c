#include <stdlib.h>
#include <stdio.h>

#include "terminal.h"
#include "string_helper.h"
#include "utils.h"
#include "constants.h"

#define MAX_COMMANDS 4096
#define clear() system("cls")
#define read(x) fflush(stdin); scanf("%[^\n]s", (char*) &x) //Only works for reading strings

struct command {
    char name[PAGE_SIZE / 4];
    char description[PAGE_SIZE];
    void (*function)();
    char params; // 0 if command has parameters, otherwise, not 0
};

char *pwd;
char command[PAGE_SIZE] = "";
Command command_list[MAX_COMMANDS];

void terminal_start() {
    clear();
    pwd = new("char", PAGE_SIZE);
    start_commands();
    set_string(pwd, "/");
    printf("Terminal [version 1.0 alpha]\n%s> ", pwd);
    loop_terminal();
}

void loop_terminal() {
    int n;
    while (1) {
        read(command);
        int index = recognize(get_command(command));
        if(index < 0 && !compare(get_command(command), "exit"))
            printf("\"%s\" not recognized as a command\n", get_command(command));
        else if(compare(command, "exit"))
            break;
        else if(compare(command, "")) {}
        else if(command_list[index].params) {
            char **params = get_params(command, &n);
            param_run(get_command(command), index, params, n);
        }
        else
            run(command, index);
        printf("%s> ", pwd);
        set_string(command, "");
    }
}

int create_command() {

}

//Start default commands
void start_commands() {

    //Clear
    set_string(command_list[0].name, "clear");
    command_list[0].function = clearScreen;
    command_list[0].params = 0;

    //Pwd
    set_string(command_list[1].name, "pwd");
    command_list[1].function = print_pwd;
    command_list[1].params = 0;

    //Echo
    set_string(command_list[2].name, "echo");
    command_list[2].function = echo;
    command_list[2].params = 1;

    //Help
    set_string(command_list[3].name, "help");
    command_list[3].function = help;
    command_list[3].params = 0;
}

int recognize(char* command) {
    int i;
    for(i = 0; i < PAGE_SIZE; i++)
        if(compare(command_list[i].name, command)) return i;
    return -1;
}

void* run(char *command, int index) {
    void *ret = NULL;
    command_list[index].function(ret);
    return ret;
}

void* param_run(char *command, int index, char** params, int num_params) {
    void *ret = NULL;
    command_list[index].function(params, num_params, ret);
    return ret;
}

//Commands without parameters
void clearScreen(void *ret) {
    clear();
}

void print_pwd(void *ret) {
    printf("%s\n", pwd);
}

void help() {
    int i;
    printf("Available commands: ");
    //for(i = 0; i < )
}

//Commands with parameters
void echo(char** params, int num_params, void* ret) {
    if(num_params > 1) {
        //TODO: add parameters (e.g -i, -f)
    } else {
        if(is_quoted(params[0]))
            printf("%s\n", remove_quotes(params[0]));
        else
            printf("Invalid string\n");
    }
}
