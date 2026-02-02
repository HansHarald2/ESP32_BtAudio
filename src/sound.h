#pragma once
#include <Arduino.h>

class Sound {
public:
    void begin();
    void loop();
    bool playSong(const String& filename);

private:
    bool playing = false;
};
