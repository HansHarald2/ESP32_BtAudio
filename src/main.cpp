#include <Arduino.h>
#include <Wire.h>
#include "sound.h"
#include "message_handler.h"
#include "nvs_flash.h"
#include <BluetoothSerial.h>


#define LED_PIN 25
#define OLED_SDA 21
#define OLED_SCL 19


Sound audio;
MessageHandler msgHandler;
BluetoothSerial SerialBT;

void btTask(void* param) {
    while (true) {
        audio.loop();
        msgHandler.loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    delay(100);
    
    Wire.begin(OLED_SDA, OLED_SCL); // OLED korrekt initialisieren

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Bluetooth starten
    if (!SerialBT.begin("ESP32_Display")) {
        Serial.println("Bluetooth start failed!");
    } else {
        Serial.println("Bluetooth started");

    }
    audio.begin();
    msgHandler.begin(&audio, LED_PIN, &SerialBT);
    xTaskCreatePinnedToCore(btTask, "bt", 4096, NULL, 5, NULL, 1);
}



void loop() {
    delay(100);  // alles läuft im Task
}
