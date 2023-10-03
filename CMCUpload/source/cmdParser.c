#include "cmdParser.h"

int Parse(char** data, int dataSize, Command_t* cmd, int rules) {
    if (dataSize == 0) return 0;
    int pointer = 0;
    while (42) {
        int executed = 0;
        for (int i = 0; i < rules; i++) {
            if (strcmp(data[pointer], cmd[i].cmd) == 0) {
                pointer++;
                if (pointer + cmd[i].parameters <= dataSize) {
                    int n = cmd[i].callback(data, pointer, cmd[i].cb_param);
                    if (n != 0) return pointer;
                }
                pointer += cmd[i].parameters;
                executed = 1;
                break;
            }
        }
        if (executed == 0) {
            return pointer + 1;
        }
        if (pointer >= dataSize) return 0;
    }
}