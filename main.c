#include <stdio.h>
#include <stdlib.h>

#include "stringHelper.h"
#include "utils.h"
#include "terminal.h"

void startup();

int main() {
    startup();
    return 0;
}

void startup() {
    terminal_start();
}
