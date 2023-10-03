#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "cmdParser.h"

#define displayError errorHandler(__LINE__, __FILE__)

struct Parameters {
    char* inputFile;
    char* outputFile;
    int writeBase;
    int readBase;
    int readLength;
    char* port;
    int speed;
    int verbose;
    int color;
    int force;
    int run;
};
struct Parameters uploadParam;

char HexTable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int cb_help(char** data, int pointer, void* parameter) {
    printf("Usage:\nCMCUpload [command]\n");
    printf("Commands:\n");
    printf("-h\t--help\t\t\tDisplay this help text\n");
    printf("-i\t--input\t\t[file]\tSet input file for writing\n");
    printf("-o\t--output\t[file]\tSet ouptut file for reading\n");
    printf("-b\t--writeBase\t[page]\tSet page address for writing [default 1]\n");
    printf("-r\t--readBase\t[page]\tSet page address for reading [default 0]\n");
    printf("-l\t--length\t[pages]\tSet number of pages to read [default 8]\n");
    printf("-p\t--port\t\t[file]\tSet serial port\n");
    printf("-s\t--speed\t\t[speed]\tSet communication speed [default 115200]\n");
    printf("-v\t--verbose\t\tEnable vorbose output\n");
    printf("-c\t--color\t\t\tEnable colored output\n");
    printf("-f\t--force\t\t\tForce upload of files to large for memory, file gets trunkated\n");
    printf("-r\t--run\t\t\tJump to page 1 to run uploaded code after upload\n");
    printf("Valid communication speeds:\n");
    printf("4800\t9600\t19200\t115200\n");
    printf("Example:\n");
    printf("./CMCUpload -p /dev/ttyACM0 -i program.bin\n");
    exit(0);
}
int cb_setInput(char** data, int pointer, void* parameter) {
    ((struct Parameters*) parameter)->inputFile = data[pointer];
    return 0;
}
int cb_setOutput(char** data, int pointer, void* parameter) {
    ((struct Parameters*) parameter)->outputFile = data[pointer];
    return 0;
}
int cb_setWritebase(char** data, int pointer, void* parameter) {
    ((struct Parameters*) parameter)->writeBase = (int) strtol(data[pointer], NULL, 0);
    return 0;
}
int cb_setReadbase(char** data, int pointer, void* parameter) {
    ((struct Parameters*) parameter)->readBase = (int) strtol(data[pointer], NULL, 0);
    return 0;
}
int cb_setReadlength(char** data, int pointer, void* parameter) {
    ((struct Parameters*) parameter)->readLength = (int) strtol(data[pointer], NULL, 0);
    return 0;
}
int cb_setPort(char** data, int pointer, void* parameter) {
    ((struct Parameters*) parameter)->port = data[pointer];
    return 0;
}
int cb_setSpeed(char** data, int pointer, void* parameter) {
    int speed = strtol(data[pointer], NULL, 0);
    int speedConv;
    switch (speed) {
        case 9600:
            speedConv = B9600;
            break;
        case 115200:
            speedConv = B115200;
            break;
        case 4800:
            speedConv = B4800;
            break;
        case 19200:
            speedConv = B19200;
            break;
        default:
            printf("[Error] %i is not a valid speed!\n", speed);
            return 1;
    }
    ((struct Parameters*) parameter)->speed = speedConv;
    return 0;
}
int cb_setVerbose(char** data, int pointer, void* parameter) {
    ((struct Parameters*) parameter)->verbose = 1;
    return 0;
}
int cb_setUsecolor(char** data, int pointer, void* parameter) {
    ((struct Parameters*) parameter)->color = 1;
    return 0;
}
int cb_setForce(char** data, int pointer, void* parameter) {
    ((struct Parameters*) parameter)->force = 1;
    return 0;
}
int cb_setRun(char** data, int pointer, void* parameter) {
    ((struct Parameters*) parameter)->run = 1;
    return 0;
}
#pragma GCC diagnostic pop
Command_t commands[] = {
    {"-h", 0, NULL, &cb_help}, {"--help", 0, NULL, &cb_help},
    {"-i", 1, &uploadParam, &cb_setInput}, {"--input", 1, &uploadParam, &cb_setInput},
    {"-o", 1, &uploadParam, &cb_setOutput}, {"--output", 1, &uploadParam, &cb_setOutput},
    {"-b", 1, &uploadParam, &cb_setWritebase}, {"--writeBase", 1, &uploadParam, &cb_setWritebase},
    {"-r", 1, &uploadParam, &cb_setReadbase}, {"--readBase", 1, &uploadParam, &cb_setReadbase},
    {"-l", 1, &uploadParam, &cb_setReadlength}, {"--length", 1, &uploadParam, &cb_setReadlength},
    {"-p", 1, &uploadParam, &cb_setPort}, {"--port", 1, &uploadParam, &cb_setPort},
    {"-s", 1, &uploadParam, &cb_setSpeed}, {"--speed", 1, &uploadParam, &cb_setSpeed},
    {"-v", 0, &uploadParam, &cb_setVerbose}, {"--verbose", 0, &uploadParam, &cb_setVerbose},
    {"-c", 0, &uploadParam, &cb_setUsecolor}, {"--color", 0, &uploadParam, &cb_setUsecolor},
    {"-f", 0, &uploadParam, &cb_setForce}, {"--force", 0, &uploadParam, &cb_setForce},
    {"-r", 0, &uploadParam, &cb_setRun}, {"--run", 0, &uploadParam, &cb_setRun}
};


void errorHandler(int line, const char* file) {
    printf("Error %i at line %i in %s: %s\n", errno, line, file, strerror(errno));
    exit(-1);
}

int DrawBar(int length, int percent) {
    printf("\r[");
    for (int i = 0; i < length; i++) {
        if (((float) i / length) * 100 >= percent) {
            printf(" ");
        } else {
            printf("#");
        }
    }
    printf("] %d%%", percent);
    return 0;
}
int initSerial(char* port, int speed) {
    int serial_port = open(port, O_RDWR);
    if (serial_port < 0) displayError;

    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) displayError;
    tty.c_cflag &= ~PARENB; //no parity
    tty.c_cflag &= ~CSTOPB; //one stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8; //8 bits per char
    tty.c_cflag |= CRTSCTS; //RTS/CTS flow controll
    tty.c_cflag |= CREAD; //we want to read from the port
    tty.c_cflag |= CLOCAL; //we dont care about modem controll lines
    tty.c_lflag &= ~ICANON; //no canonical mode
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;
    cfsetispeed(&tty, speed);
    cfsetospeed(&tty, speed);
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) displayError;
    return serial_port;
}
int setDtr(int sp) {
    int DTR_flag = TIOCM_DTR;
    if (ioctl(sp, TIOCMSET, &DTR_flag) != 0) displayError;
    return 0;
}
int clearDtr(int sp) {
    int DTR_flag = ~TIOCM_DTR;
    if (ioctl(sp, TIOCMSET, &DTR_flag) != 0) displayError;
    return 0;
}
int ResetMC(int sp) {
    setDtr(sp);
    usleep(100000); //sleep 100ms
    clearDtr(sp);
    usleep(10000); //sleep 10ms
    return 0;
}
int Receive(int sp, char* buffer, int bufferSize, int minChars) {
    int pointer = 0;
    while (42) {
        char temp;
        int n = read(sp, &temp, 1);
        if (n == 0) {
            if (pointer >= minChars) return pointer;
        } else {
            if (buffer) buffer[pointer] = temp;
            pointer++;
            if ((minChars == 0) && (temp == '.')) return pointer;
            if (pointer >= bufferSize) return pointer;
        }
    }
}
int SendByte(int sp, char data) {
    write(sp, &data, 1);
    return 0;
}
int Sync(int sp) {
    ResetMC(sp);

    SendByte(sp, 't');

    char temp;
    if (Receive(sp, &temp, 1, 0) == 1) {
        if (temp == '.') return 0; //got expected answer
    }
    return 1;
}
int GetChecksum(int sp) {
    SendByte(sp, 's');
    unsigned char temp;
    Receive(sp, (char*) &temp, 1, 1);
    return temp;
}
int CalcChecksum(unsigned char* data) {
    int sum;
    for (int i = 0; i < 16384; i++) {
        sum += data[i];
        sum = sum & 0xff;
    }
    return sum;
}
int ReadMemory(int sp) {
    if ((uploadParam.readBase < 0) || (uploadParam.readBase >= 8) || (uploadParam.readBase + uploadParam.readLength > 8)) {
        printf("[Error] read out of bound!\n");
        exit(-1);
    }
    FILE* of = fopen(uploadParam.outputFile, "w");
    if (of == NULL) displayError;
    char buffer[16384];
    char temp;
    struct timespec startTime, endTime;
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    for (int i = uploadParam.readBase; i < uploadParam.readBase + uploadParam.readLength; i++) {
        float percent = ((float) (i - uploadParam.readBase) / uploadParam.readLength) * 100;
        DrawBar(40, percent);
        fflush(stdout);
        SendByte(sp, 'r');
        SendByte(sp, HexTable[i]);
        Receive(sp, &temp, 1, 0);
        if (temp != '.') {
            printf("\n[Error] Sync lost!\n");
            fflush(stdout);
            exit(-1);
        }
        int checkSum = GetChecksum(sp);
        SendByte(sp, 'g');
        Receive(sp, buffer, sizeof(buffer), sizeof(buffer));
        if (CalcChecksum((unsigned char*) buffer) != checkSum) {
            printf("\n[Error] Checksum missmatch!\n");
            fflush(stdout);
        }
        fwrite(buffer, sizeof(buffer), 1, of);
    }
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    DrawBar(40, 100);
    printf("\n");
    fflush(stdout);
    fclose(of);
    if (uploadParam.verbose) {
        float timeUsed = (endTime.tv_sec - startTime.tv_sec) + ((endTime.tv_nsec - startTime.tv_nsec) / 1e9);
        float dataRate = (float) ((uploadParam.readLength * 16384) / timeUsed) / 1024;
        printf("Read completed in %.2f seconds [%.2f kb/s]\n", timeUsed, dataRate);
    }
    return 0;
}
int WriteMemory(int sp) {
    if ((uploadParam.writeBase < 0) || (uploadParam.writeBase >= 8)) {
        printf("[Error] write out of bounds!\n");
        exit(-1);
    }
    FILE* inFile = fopen(uploadParam.inputFile, "r");
    if (inFile == NULL) displayError;
    fseek(inFile, 0, SEEK_END);
    int length = (int) ceil((float) ftell(inFile) / 16384);
    fseek(inFile, 0, SEEK_SET);
    if (uploadParam.writeBase + length > 8) {
        if (uploadParam.force) {
            printf("[Warning] file to large!\n");
        } else {
            printf("[Error] file to large!\n");
            exit(-1);
        }
    }
    if (uploadParam.writeBase == 0) {
        printf("Are you shure you want to write page 0?\nThis will overwrite the bootloader (y/N) ");
        char temp;
        scanf("%c", &temp);
        printf("\n");
        if ((temp != 'y') && (temp != 'Y')) {
            printf("Aborting!\n");
            exit(0);
        }
    }
    char buffer[16384];
    char temp;
    int count = 0;
    struct timespec startTime, endTime;
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    while (42) {
        float percent = ((float) count / length) * 100;
        DrawBar(40, percent);
        fflush(stdout);
        fread(buffer, sizeof(buffer), 1, inFile);
        SendByte(sp, 'p');
        write(sp, buffer, sizeof(buffer));
        Receive(sp, &temp, 1, 1);
        if (temp != '.') {
            printf("\n[Error] Sync lost!\n");
            fflush(stdout);
            exit(-1);
        }
        if (CalcChecksum((unsigned char*) buffer) != GetChecksum(sp)) {
            printf("\n[Error] Checksum error!\n");
            exit(-1);
        }
        SendByte(sp, 'w');
        SendByte(sp, HexTable[count + uploadParam.writeBase]);
        if (count + uploadParam.writeBase == 0) {
            printf("\nWaiting for rom write...\n");
            fflush(stdout);
            usleep(4000000);
            printf("Updated bootloader. Resyncing: ");
            fflush(stdout);
            int synced = 0;
            for (int i = 0; i < 10; i++) {
                printf(".");
                fflush(stdout);
                if (Sync(sp) == 0) {
                    synced = 1;
                    break;
                }
            }
            if (synced) {
                printf("[SUCCES]\n");
                fflush(stdout);
            } else {
                printf("[FAIL]\n");
                printf("[Error] Sync lost!\n");
                fflush(stdout);
                exit(-1);
            }
        } else {
            Receive(sp, &temp, 1, 0);
            if (temp != '.') {
                printf("\n[Error] Write error!\n");
                fflush(stdout);
                exit(-1);
            }
            SendByte(sp, 'c');
            SendByte(sp, HexTable[count + uploadParam.writeBase]);
            Receive(sp, &temp, 1, 1);
            if (temp != '.') {
                printf("\n[Error] Verify error!\n");
                fflush(stdout);
                exit(-1);
            }
        }
        count++;
        if ((count >= length) || count + uploadParam.writeBase >= 8) break;
    }
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    DrawBar(40, 100);
    printf("\n");
    fflush(stdout);
    fclose(inFile);
    if (uploadParam.verbose) {
        float timeUsed = (endTime.tv_sec - startTime.tv_sec) + ((endTime.tv_nsec - startTime.tv_nsec) / 1e9);
        float dataRate = (float) ((uploadParam.readLength * 16384) / timeUsed) / 1024;
        printf("Read completed in %.2f seconds [%.2f kb/s]\n", timeUsed, dataRate);
    }
    return 0;
}
int Boot(int sp) {
    SendByte(sp, 'b');
    return 0;
}

int main(int argc, char** argv) {
    memset(&uploadParam, 0, sizeof(struct Parameters));
    uploadParam.speed = B115200;
    uploadParam.writeBase = 1;
    uploadParam.readBase = 0;
    uploadParam.readLength = 8;
    int n = Parse(&argv[1], argc - 1, commands, sizeof(commands) / sizeof(commands[0]));
    if (n != 0) {
        printf("[Error] could not parse: %s\n", argv[1 + (n - 1)]);
        exit(-1);
    }
    if (uploadParam.port == NULL) {
        printf("[Error] no port specified!\n");
        exit(-1);
    }

    if (uploadParam.verbose) printf("opening port %s\n", uploadParam.port);
    int serial_port = initSerial(uploadParam.port, uploadParam.speed);

    printf("Syncing with CMCBL: ");
    int succes = 0;
    for (int i = 0; i < 10; i++) {
        printf(".");
        fflush(stdout);
        if (Sync(serial_port) == 0) {
            succes = 1;
            break;
        }
    }
    if (succes) {
        printf("[SUCCES]\n");
    } else {
        printf("[FAIL]\n");
        exit(-1);
    }

    char versionString[100];
    SendByte(serial_port, '?');
    n = Receive(serial_port, versionString, sizeof(versionString), 0);
    versionString[n - 1] = 0;
    printf("CMCBL version: %s\n", versionString);

    if (uploadParam.outputFile) ReadMemory(serial_port);

    if (uploadParam.inputFile) WriteMemory(serial_port);

    if (uploadParam.run) {
        printf("Jumping to page 1\n");
        fflush(stdout);
        Boot(serial_port);
    } else {
        //do not clocse serial port when specifying run flag (linux will set DTR high and hold the board in reset)
        close(serial_port);
    }

    printf("Done\n");

    return 0;
}