// message_handler.cpp
#include "message_handler.h"

void MessageHandler::begin(Sound* audioInstance, int ledPin, BluetoothSerial* bt) {
    this->audio = audioInstance;
    this->ledPin = ledPin;
    this->btSerial = bt;

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    Serial.println("MessageHandler ready");
    if (btSerial) Serial.println("BluetoothSerial linked");

    // OLED initialisieren und Begrüßung anzeigen
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 allocation failed");
    } else {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("Hallo User");
        display.display();

        // Direkt beim Start WAV abspielen
        if (audio) {
            audio->playSong("test.wav");
        }
    }
}

void MessageHandler::loop() {
    String cmd;

    // Priorität: Bluetooth, dann USB Serial
    if (btSerial && btSerial->available()) {
        cmd = btSerial->readStringUntil('\n');
    } else if (Serial.available()) {
        cmd = Serial.readStringUntil('\n');
    }

    if (cmd.length() > 0) {
        cmd.trim();
        Serial.print("CMD: ");
        Serial.println(cmd);

        // OLED anzeigen
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("CMD:");
        display.println(cmd);
        display.display();

        if (cmd.equalsIgnoreCase("LEDON")) {
            digitalWrite(ledPin, HIGH);
            Serial.println("LED ON");
            if (btSerial) btSerial->println("ACK: LED ON");
        } 
        else if (cmd.equalsIgnoreCase("LEDOFF")) {
            digitalWrite(ledPin, LOW);
            Serial.println("LED OFF");
            if (btSerial) btSerial->println("ACK: LED OFF");
        } 
        else if (cmd.startsWith("PLAY ")) {
            Serial.println("Play");
            if (audio) {
                bool ok = audio->playSong(cmd.substring(5));
                if (btSerial) btSerial->println(ok ? "ACK: Playing song" : "ERR: Song not found");
            }
            
        }
        // weitere Kommandos hier ergänzen
    }
}
