// message_handler.h
#pragma once
#include <Arduino.h>
#include "sound.h"
#include <BluetoothSerial.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

class MessageHandler {
public:
    void begin(Sound* audioInstance, int ledPin, int relayPin, BluetoothSerial* bt = nullptr);
    void loop();

private:
    Sound* audio = nullptr;
    int ledPin = -1;
    int relayPin = -1;
    BluetoothSerial* btSerial = nullptr;
    Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // OLED-Objekt
};
