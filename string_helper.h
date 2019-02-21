#ifndef STRING_HELPER_H
#define STRING_HELPER_H

void set_string(char *variable, char *value); //set a char array a string value
int compare(char *s1, char *s2); //returns 1 if equals and 0 if different
int length(char *array); //returns the size of the string
int is_quoted(char *array); //returns 1 if array is quoted, 0 if none or more than two opening quotes
char** split(char *string, char character, int *array_size); //returns an array of strings split on every character repetition, and as parameter, the array_size
char *remove_quotes(char* string); //returns a string with removed quotes, if strings has invalid quotes, returns NULL

#endif // STRING_HELPER_H
