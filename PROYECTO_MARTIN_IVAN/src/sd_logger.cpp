#include <Arduino.h>
#include <SD.h>
#include "sd_logger.h"

void saveEvent(
    String eventText,
    int timestamp,
    String currentTime
) {

    File logFile = SD.open("/sleep_log.txt", FILE_APPEND);

    if(logFile) {

        logFile.print("[");

        logFile.print(timestamp);

        logFile.print("s] ");

        logFile.print(eventText);

        logFile.print(" -- ");

        logFile.println(currentTime);

        logFile.close();

        Serial.println("Evento guardado en SD");

    } else {

        Serial.println("Error abriendo archivo");
    }
}