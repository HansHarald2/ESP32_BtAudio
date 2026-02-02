#pragma once
#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

class SdAudio {
public:
    bool begin(int csPin = 5);
    void listFiles(const char* dir = "/");
    bool playSong(const char* filename);  // später mit Sound-System verbinden
};
