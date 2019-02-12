#ifndef UTILS_H
#define UTILS_H

void* new(char *type, int size);
char* get_command(char *command);
char** get_params(char *command, int *number_params);

#endif // UTILS_H
