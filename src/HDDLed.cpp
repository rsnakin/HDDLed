#include <fstream>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wiringPi.h>
#include "Log.hpp"
#include "cfgini.hpp"
#include "showActivity.hpp"

#define APP_NAME "hddled"
#define VERSION "1.0"
#define CONFIG "/usr/local/etc/hddled.ini"

void killOtherInstances(const char *app_name, int timeout_ms) {
    char buffer[512];
    pid_t my_pid = getpid();

    char command[128];
    snprintf(command, sizeof(command), "pidof %s", app_name);

    FILE *pipe = popen(command, "r");
    if (!pipe || !fgets(buffer, sizeof(buffer), pipe)) {
        if (pipe) pclose(pipe);
        fprintf(stderr, "Failed to get PID list for %s\n", app_name);
        return;
    }
    pclose(pipe);

    char *token = strtok(buffer, " \n");
    while (token) {
        pid_t pid = strtoul(token, NULL, 10);
        if (pid > 0 && pid != my_pid) {
            printf("Old PID found, kill \e[31m%d\e[0m ... ", pid);
            if (kill(pid, SIGTERM) == 0) {
                printf("\e[33m[term]\e[0m ");
                usleep(timeout_ms * 1000);
                if (kill(pid, 0) == 0) {
                    if (kill(pid, SIGKILL) == 0) {
                        printf("\e[31m[kill]\e[0m\n");
                    } else {
                        printf("\e[31m[kill error]\e[0m\n");
                    }
                } else {
                    printf("\e[32m[done]\e[0m\n");
                }
            } else {
                printf("\e[31m[term error]\e[0m\n");
            }
        }
        token = strtok(NULL, " \n");
    }
}

int main() {
    printf("HDDLed V%s\n", VERSION);

    Log hlog;
    cfg config(CONFIG);
    if (config.error) {
        printf("%s\n", config.errorMsg);
        return 0;
    }

    char pinNumbering[64], path2dData[64], path2log[64];
    int pin = 0;
    unsigned long int delay = 0;

    if (!config.getValue("%s", "setup", "PIN_NUMBERING", pinNumbering) ||
        !config.getValue("%d", "setup", "PIN", &pin) ||
        !config.getValue("%lu", "setup", "DELAY", &delay)) {
        printf("%s\n", config.errorMsg);
        return 0;
    }

    config.getValue("%s", "files", "PATH_TO_DISK_DATA", path2dData);
    if (config.getValue("%s", "files", "PATH_TO_LOG", path2log)) {
        hlog.setLogPath(path2log);
    }

    printf(" * PIN_NUMBERING: %s\n", pinNumbering);
    printf(" * PIN: %d\n", pin);
    printf(" * DELAY: %lu\n", delay);
    printf(" * PATH_TO_DISK_DATA: %s\n", path2dData);
    if (*path2log) printf(" * PATH_TO_LOG: %s\n", path2log);

    hlog.write("HDDLed V%s started", VERSION);
    hlog.write("PIN_NUMBERING: %s", pinNumbering);
    hlog.write("PIN: %d", pin);
    hlog.write("DELAY: %d", delay);
    hlog.write("PATH_TO_DISK_DATA: %s", path2dData);

    killOtherInstances(APP_NAME, 1000);

    int wpiSetup = -1;
    if (strcmp(pinNumbering, "WPI") == 0) {
        wpiSetup = wiringPiSetup();
    } else if (strcmp(pinNumbering, "BCM_GPIO") == 0) {
        wpiSetup = wiringPiSetupGpio();
    } else {
        printf("Invalid PIN_NUMBERING \"%s\", exit...\n", pinNumbering);
        hlog.write("Invalid PIN_NUMBERING \"%s\", exit...", pinNumbering);
        return 0;
    }

    if (wpiSetup == 0) {
        pinMode(pin, OUTPUT);
        int pid = fork();
        if (pid == -1) {
            printf("\e[31mError: Start Daemon failed\e[0m\n");
            hlog.write("Start Daemon failed!");
            return -1;
        } else if (pid == 0) {
            config.~cfg();
            setsid();
            chdir("/");
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            if (!showActivity(pin, delay, path2dData, hlog)) {
                hlog.write("Error... exit...");
            }
        }
    } else {
        hlog.write("wiringPi setup failed!");
    }

    return 0;
}
