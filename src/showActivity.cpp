#include <fstream>
#include <signal.h>
#include <wiringPi.h>
#include <unistd.h>
#include "Log.hpp"
#include "showActivity.hpp"

bool showActivity(int pin, unsigned long int delay, const char *path2dData, Log log) {
    char buffer[8192];
    FILE* dData = fopen(path2dData, "r");
    if(dData == NULL) {
        log.write("Can't open %s!", path2dData);
        return(false);
    }
    for(int i = 0; i < 3; i ++) {
        digitalWrite(pin, HIGH);
        usleep(500000);
        digitalWrite(pin, LOW);
        usleep(500000);
    }
    unsigned long int onDelay = delay * 2;
    size_t i = 0; int n = 0; int itemNum = 0;
    char __buffer[64];
    size_t rBytes;
    size_t dataSize = fread(buffer, 1, sizeof(buffer), dData);
    while(1) {
        fseek(dData, 0, SEEK_SET);
        i = 0; n = 0; itemNum = 0;
        rBytes = fread(buffer, 1, dataSize - 1, dData);
        if(rBytes > 0) {
            while(i < rBytes) {
                if(buffer[i] != ' ' && buffer[i] != '\n') {
                    __buffer[n] = buffer[i];
                    n ++;
                } else if(buffer[i] == '\n') {
                    itemNum = 0;
                    n = 0;
                } else if(buffer[i] == ' ' && n > 0) {
                    if(itemNum == 11) {
                        if(__buffer[0] != 48) {
                            digitalWrite(pin, HIGH);
                            usleep(onDelay);
                            fseek(dData, 0, SEEK_SET);
                            dataSize = fread(buffer, 1, sizeof(buffer), dData);
                            break;
                        }
                    }
                    itemNum ++;
                    n = 0;
                }
                i ++;
            }
        }
        usleep(delay);
        digitalWrite(pin, LOW);
    }
    fclose(dData);
    return(true);
}

