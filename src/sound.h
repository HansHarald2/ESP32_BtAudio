#pragma once
#include <Arduino.h>
#include <Audio.h> // ESP32-audioI2S

class Sound {
public:
    void begin();
    void loop();
    bool playSong(const String& filename);

private:
    Audio audio;
    bool playing = false;
};
