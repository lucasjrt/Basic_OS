#include "stringHelper.h"

#include <stdlib.h>

void setString(char *variable, const char *value) {
    int i = 0;
    if(variable == NULL) return;
    while(value[i] != '\0')
        variable[i] = value[i++];
    variable[i] = '\0';
}

int compare(char *s1, char *s2) {
    int i = 0;
    if(s1 == NULL || s2 == NULL || (length(s1) != length((s2))))
        return 0;
    while(s1[i] != '\0' && s2[i] != '\0')
        if(s1[i] != s2[i++]) return 0;
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

