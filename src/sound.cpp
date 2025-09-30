#include "sound.h"
#include <SPIFFS.h>

#define I2S_BCK_PIN   26   // Bit Clock
#define I2S_DATA_PIN  22   // Data
#define I2S_LRCK_PIN  33   // Word Select / LRCK

void Sound::begin() {
    if(!SPIFFS.begin(true)) {
        Serial.println("SPIFFS mount failed!");
        return;
    }

    audio.setPinout(I2S_BCK_PIN, I2S_LRCK_PIN, I2S_DATA_PIN); // Reihenfolge: BCK, LRCK, DATA
    audio.setVolume(21); // maximal
    Serial.println("Sound / I2S initialized");
}

void Sound::loop() {
    audio.loop(); // muss im Hauptloop laufen!
}

bool Sound::playSong(const String& filename) {
    String path = "/songs/" + filename;
    Serial.print("Trying to play: ");
    Serial.println(path);

    if (!SPIFFS.exists(path)) {
        Serial.println("Song not found: " + path);
        return false;
    }

    audio.connecttoFS(SPIFFS, path.c_str()); // spielt WAV/MP3/OGG ab
    playing = true;
    Serial.println("Playing song: " + filename);
    return true;
}


