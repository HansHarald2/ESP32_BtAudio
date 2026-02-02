#include "sound.h"
#include <SD.h>

void Sound::begin() {
    // No SPIFFS/audio initialization — this project uses SD for data only now.
}

void Sound::loop() {
    // nothing
}

bool Sound::playSong(const String& filename) {
    // keep compatibility: return true if file exists on SD, otherwise false
    if (filename.startsWith("/")) {
        return SD.exists(filename);
    }
    String sdPaths[2] = {String("/sound/") + filename, String("/") + filename};
    for (int i = 0; i < 2; i++) {
        if (SD.exists(sdPaths[i])) return true;
    }
    return false;
}


