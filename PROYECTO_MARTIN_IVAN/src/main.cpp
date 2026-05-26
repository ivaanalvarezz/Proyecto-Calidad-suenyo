#include <Arduino.h>

#include "snore_detection.h"
#include "sd_logger.h"

#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

#include "driver/i2s.h"

// ---------------- SD ----------------
#define SD_CS 10

// ---------------- I2S ----------------
#define I2S_WS 4
#define I2S_SD 6
#define I2S_SCK 5

#define I2S_PORT I2S_NUM_0

// ---------------- WiFi ----------------
const char* ssid = "TU_WIFI";
const char* password = "TU_PASSWORD";

// ---------------- Audio ----------------
const int bufferLen = 64;

int16_t samples[bufferLen];

// ---------------- Variables ----------------
int sleepTime = 0;

// ---------------- Obtener hora actual ----------------
String getCurrentTime() {

    struct tm timeinfo;

    if(!getLocalTime(&timeinfo)) {

        return "00:00";
    }

    char timeString[6];

    strftime(timeString, sizeof(timeString), "%H:%M", &timeinfo);

    return String(timeString);
}

// ---------------- Configuracion I2S ----------------
void setupI2S() {

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

    i2s_set_pin(I2S_PORT, &pin_config);
}

void setup() {

    Serial.begin(115200);

    delay(1000);

    Serial.println("Iniciando sistema...");

    // ---------------- Iniciar microfono ----------------

    setupI2S();

    Serial.println("Microfono iniciado");

    // ---------------- WiFi ----------------

    WiFi.begin(ssid, password);

    Serial.print("Conectando al WiFi");

    while (WiFi.status() != WL_CONNECTED) {

        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi conectado");

    // ---------------- Hora real ----------------

    configTime(3600, 3600, "pool.ntp.org");

    // ---------------- SD ----------------

    if (!SD.begin(SD_CS)) {

        Serial.println("Error iniciando SD");

    } else {

        Serial.println("SD iniciada correctamente");
    }
}

void loop() {

    sleepTime++;

    // ---------------- Leer audio I2S ----------------

    size_t bytesRead;

    i2s_read(
        I2S_PORT,
        &samples,
        sizeof(samples),
        &bytesRead,
        portMAX_DELAY
    );

    // ---------------- Mostrar amplitud maxima ----------------

    int maxSample = 0;

    for(int i = 0; i < bufferLen; i++) {

        int sample = abs(samples[i]);

        if(sample > maxSample) {

            maxSample = sample;
        }
    }

    Serial.print("Nivel sonido: ");
    Serial.println(maxSample);

    // ---------------- Detectar ronquido ----------------

    if(detectSnore(samples, bufferLen)) {

        Serial.println("RONQUIDO DETECTADO");

        saveEvent(
            "Ronquido detectado",
            sleepTime,
            getCurrentTime()
        );
    }

    delay(1000);
}