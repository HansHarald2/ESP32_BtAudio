#include <Arduino.h>
#include <Wire.h>
#include "sound.h"
#include "message_handler.h"
#include "nvs_flash.h"
#include <BluetoothSerial.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED_PIN 25
#define RELAY_PIN 27
#define OLED_SDA 21
#define OLED_SCL 22

#define SD_CS   5
#define SD_MOSI 23
#define SD_MISO 19
#define SD_SCK  18

Sound audio;
MessageHandler msgHandler;
BluetoothSerial SerialBT;

void btTask(void* param) {
    vTaskDelay(500 / portTICK_PERIOD_MS);

    Serial.println("[SD] Initializing SD card...");
    if (!SD.begin(SD_CS)) {
        Serial.println("[SD] initialization failed!");
    } else {
        Serial.printf("[SD] SD card initialized! Size: %lld MB\n", SD.cardSize() / (1024*1024));
        msgHandler.sdReady();
    }

    while (true) {
        audio.loop();
        msgHandler.loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    while (!Serial) { ; }
    Serial.println("Initializing SD card...");
    Wire.begin(OLED_SDA, OLED_SCL);
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }
    if (!SD.begin(SD_CS)) {
    Serial.println("initialization failed!");
    return;
    }
  Serial.println("initialization done.");
    SerialBT.begin("ESP32_Display");
    audio.begin();
    msgHandler.begin(&audio, LED_PIN, RELAY_PIN, &SerialBT);
    
    xTaskCreatePinnedToCore(btTask, "bt", 4096, NULL, 5, NULL, 1);
}

void loop() {
    delay(100);
}

