#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <Arduino.h>

void saveEvent(
    String eventText,
    int timestamp,
    String currentTime
);

#endif