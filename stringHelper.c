#include <stdlib.h>
#include <stdio.h>

#include "stringHelper.h"
#include "constants.h"

void set_string(char *variable, char *value) {
    int i = 0;
    if(variable == NULL) return;
    while(value[i] != '\0') {
        variable[i] = value[i];
        i++;
    }
    variable[i] = '\0';
}

int compare(char *s1, char *s2) {
    int i = 0;
    if(s1 == NULL || s2 == NULL || (length(s1) != length((s2))))
        return 0;
    while(s1[i] != '\0' && s2[i] != '\0') {
        if(s1[i] != s2[i])
            return 0;
        i++;
    }
    return 1;
}

int length(char *array) {
    int n = 0, i = 0;
    if(array == NULL)
        return -1;
    while(array[i++] != '\0')
        n++;
    return n;
}

int is_quoted(char *array) {
    int i = 0;
    int flag = 0; //0 not quoted, 1 one opening quotes, 2 true quoted
    while (array[i] != '\0') {
        if(!flag && array[i] == '\"')
            flag = 1;
        else if(flag == 1 && array[i] == '\"')
            flag = 2;
        else if(flag == 2 && array[i] == '\"')
            return 0; //More than one opening quotes
        i++;
    }
    if(flag == 2)
        return 1;
    return 0;
}

char** split(char *string, char character, int *array_size) {
    int i = 0, n = 0, start = 0, j;
    char **strings; //Array that will store the split strings

    strings = (char**) malloc(PAGE_SIZE * sizeof(char*));
    for(j = 0; j < PAGE_SIZE; j++)
        strings[j] = (char*) malloc(PAGE_SIZE * sizeof(char));

    while(string[i] != '\0') {
        if(string[i] == character) {
            for(j = start; j < i; j++)
                strings[n][j - start] = string[j];
            strings[n][j-start] = '\0';
            n++;
            start = i + 1;
        }
        i++;
    }
    for(j = start; j < i; j++)
        strings[n][j-start] = string[j];
    strings[n][j-start] = '\0';
    *array_size = n + 1;
    return strings;
}

char *remove_quotes(char* string) {
    int i = 1;
    if(string[0] != '"')
        return NULL;
    while(string[i] != '\0' && string[i] != '"') {
        string[i-1] = string[i];
        i++;
    }
    if(string[i] == '\0')
        return NULL;
    else{
        string[i-1] = '\0';
        return string;
    }
}

