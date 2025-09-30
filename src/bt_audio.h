#pragma once

//#include <Arduino.h>
//#include <BluetoothA2DPSink.h>
//#include "AudioTools.h"
//#include <SD.h>
//#include <SPI.h>

//#define I2S_BCK_PIN 26
//#define I2S_LRCK_PIN 33
//#define I2S_DATA_PIN 22
//#define SD_CS_PIN   5

class BtAudio {
public:
    BtAudio();          // Constructor
    void begin();       // Initialize audio and peripherals
    void loop();        // Main loop handling

private:
    //I2SStream i2s;                   // I2S interface
    //BluetoothA2DPSink a2dp_sink;     // Bluetooth audio sink
    //bool sdInitialized;              // SD card status
};
