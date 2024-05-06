#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


void help(void) {
    printf("ptr <pointer> a kernel debugger\n");
    printf("h ................... help\n");
    printf("att [driver] .......... attach to driver.\n");
    printf("sym [symbol] .......... find symbol of the current driver.\n");
    printf("all ................... show all the symbols of current driver.\n");
    printf("krn [symbol] .......... show a kernel symbol\n");
    printf("r [addr] [amount] ... read bytes\n");
    printf("q ................... quit\n");
}


void show_driver_functions(const char *driver_lookup) {
    FILE *fp;
    char line[512];
    unsigned long address;
    char type;
    char name[256];
    char driver[256];

    fp = fopen("/proc/kallsyms", "r");
    if (!fp) {
        perror("Failed to open /proc/kallsyms");
        return;
    }

    while (fgets(line, sizeof(line), fp))
        if (sscanf(line, "%lx %c %s\t[%s]", &address, &type, name, driver) == 3)
            if (strcmp(driver, driver_lookup)==0)
                printf("0x%lx %c %s\n", address, type, name);

    fclose(fp);
}


void show_symbol_address(const char *symbol_name) {
    FILE *fp;
    char line[512];
    unsigned long address;
    char type;
    char name[256];

    fp = fopen("/proc/kallsyms", "r");
    if (!fp) {
        perror("Failed to open /proc/kallsyms");
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%lx %c %s", &address, &type, name) == 3) {
            if (strstr(name, symbol_name)) {
                printf("0x%lx %c %s\n", address, type, name);
            }
        }
    }

    fclose(fp);
}

void show_driver_symbol_address(const char *symbol_name, const char *driver_lookup) {
    FILE *fp;
    char line[512];
    unsigned long address;
    char type;
    char name[256];
    char driver[256];

    fp = fopen("/proc/kallsyms", "r");
    if (!fp) {
        perror("Failed to open /proc/kallsyms");
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%lx %c %s\t[%s]", &address, &type, name, driver) == 3) {
            if (strstr(name, symbol_name) && strcmp(driver, driver_lookup) == 0) {
                printf("0x%lx %c %s\n", address, type, name);
            }
        }
    }

    fclose(fp);
}


int main(void) {
    char cmd[1024];
    char driver[255];

    if (getuid() != 0) {
        printf("launch ptr as root\n");
        exit(1);
    }

    int fd = open("/dev/ptr", O_RDONLY);

    for(;;) {
        memset(cmd, 0, 1024);
        printf("ptr> ");
        fgets(cmd, 1000, stdin);
        cmd[strlen(cmd)-1] = 0;
        
        if (strcmp(cmd,"h")==0)
            help();

        if (strcmp(cmd,"q")==0)
            break;

        if (cmd[0] == 'a' && cmd[1] == 't' && cmd[2] == 't' && cmd[3] == ' ') {
            strncpy(driver, &cmd[4], 255);
            printf("attached to %s\n", driver);
            continue;
        }

        if (cmd[0] == 'k' && cmd[1] == 'r' && cmd[2] == 'n' && cmd[3] == ' ') {
            show_symbol_address(&cmd[4]);
            continue;
        }

        if (cmd[0] == 's' && cmd[1] == 'y' && cmd[2] == 'm' && cmd[3] == ' ') {
            show_driver_symbol_address(&cmd[4], driver);
            continue;
        }

        if (cmd[0] == 'a' && cmd[1] == 'l' && cmd[2] == 'l') {
            show_driver_functions(driver);
            continue;
        }
    }

    close(fd);
}

