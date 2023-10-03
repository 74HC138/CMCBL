#ifndef CMDPARSER_H
#define CMDPARSER_H

#include <string.h>

struct Command {
    char* cmd;
    int parameters;
    void* cb_param;
    int (*callback)(char**, int, void*);
};
typedef struct Command Command_t;

int Parse(char** data, int dataSize, Command_t* cmd, int rules);

#endif