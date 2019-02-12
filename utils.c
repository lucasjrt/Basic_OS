#include <stdlib.h>

#include "utils.h"
#include "stringHelper.h"

void* new(char *type, int size) {
    if(compare(type, "char")) {
        return (char*) malloc(size * sizeof(char));
    } else if(compare(type, "int")) {
        return (int*) malloc(size * sizeof(int));
    } else if(compare(type, "float")){
        return (float*) malloc(size * sizeof(float));
    } else if(compare(type, "double")) {
        return (double*) malloc(size * sizeof(double));
    } else if(compare(type, "long") || compare(type, "long int")) {
        return (long*) malloc(size * sizeof(long));
    } else if(compare(type, "long long") || compare(type, "long long int")) {
        return (long long*) malloc(size * sizeof(long long));
    } else {
        return NULL;
    }
}

char* get_command(char *command) {
    int a;
    char **temp = split(command, ' ', &a);
    char *c = (char*) malloc(length(temp[0]) * sizeof(char));
    set_string(c, temp[0]);
    free(temp);
    return c;
}

char** get_params(char *command, int *number_params) {
    char **params = split(command, ' ', number_params);
    for(int i = 0; i < *number_params - 1; i++) {
        set_string(params[i], params[i+1]);
    }
    (*number_params)--;
    return params;
}
