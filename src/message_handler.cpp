// message_handler.cpp
#include "message_handler.h"
#include <SD.h>
#include <FS.h>

unsigned long relayOnTime = 0;
bool relayActive = false;
String currentDir = "/";

void MessageHandler::showTextOnOLED(String content){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  int maxLines = SCREEN_HEIGHT / 8; // 8px font
  int charsPerLine = 21; // approximate for 128px width
  int lineCount = 0;
  String lineStr = "";
  for (unsigned int i = 0; i < content.length(); i++){
    char c = content[i];
    if (c == '\r') continue;
    if (c == '\n' || lineStr.length() >= (unsigned)charsPerLine){
      display.println(lineStr);
      lineStr = "";
      lineCount++;
      if (lineCount >= maxLines) break;
      if (c == '\n') continue;
    }
    lineStr += c;
  }
  if (lineCount < maxLines && lineStr.length() > 0) display.println(lineStr);
  display.display();
}

void MessageHandler::showDebugStatus(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  display.print("SD Ready: ");
  display.println(sdCardReady ? "YES" : "NO");
  
  if (sdCardReady) {
    uint64_t cardSize = SD.cardSize();
    display.print("Size: ");
    display.print(cardSize / (1024*1024));
    display.println("MB");
  }
  
  display.print("Dir: ");
  display.println(currentDir);
  
  display.display();
}

void MessageHandler::ReadFile(const char * path){
  File myFile = SD.open(path);
  if (myFile) {
    if (btSerial) btSerial->printf("Reading file from %s\n", path);
    String content = "";
    while (myFile.available()) {
      char c = (char)myFile.read();
      if (btSerial) btSerial->print(c);
      content += c;
    }
    myFile.close();
    // Show on OLED
    showTextOnOLED(content);
  } else {
    if (btSerial) btSerial->println("error opening file");
  }
}

void MessageHandler::ListPlayDirectory(const char * path){
  File dir = SD.open(path);
  if (!dir) {
    if (btSerial) btSerial->printf("failed to open %s\n", path);
    showTextOnOLED(String("Dir not found:\n") + path);
    return;
  }
  if (!dir.isDirectory()){
    if (btSerial) btSerial->printf("%s is not a directory\n", path);
    dir.close();
    showTextOnOLED("Not a directory");
    return;
  }
  if (btSerial) btSerial->printf("Listing directory %s\n", path);
  String content = "";
  int count = 0;
  File entry = dir.openNextFile();
  while (entry) {
    String name = String(entry.name());
    if (entry.isDirectory()) name += "/";
    content += name;
    content += '\n';
    if (btSerial) btSerial->println(name);
    entry.close();
    count++;
    if (count >= 50) break; // avoid huge lists
    entry = dir.openNextFile();
  }
  dir.close();
  if (content.length() == 0) content = "(empty)";
  showTextOnOLED(content);
}

void MessageHandler::begin(Sound* audioInstance, int ledPin, int relayPin, BluetoothSerial* bt) {
    this->audio = audioInstance;
    this->ledPin = ledPin;
    this->relayPin = relayPin;
    this->btSerial = bt;

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        return;
    }
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Ready");
    display.display();

    if (btSerial) {
        btSerial->println("Boot complete");
        btSerial->println("Waiting for SD card...");
    }

    if (audio) {
        audio->playSong("/startup.wav");
    }
}

void MessageHandler::sdReady() {
    sdCardReady = true;
    if (btSerial) {
        btSerial->println("SD card detected!");
    }
}

void MessageHandler::loop() {
    String cmd;

    if (relayActive && (millis() - relayOnTime >= 4000)) {
        digitalWrite(relayPin, LOW);
        relayActive = false;
    }

    if (btSerial && btSerial->available()) {
        cmd = btSerial->readStringUntil('\n');
    } else if (Serial.available()) {
        cmd = Serial.readStringUntil('\n');
    }

    if (cmd.length() == 0) return;
    
    cmd.trim();

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(currentDir);
    display.println(cmd);
    display.display();

    if (cmd.equalsIgnoreCase("help")) {
        if (btSerial) {
            btSerial->println("cd <dir>     - Change directory");
            btSerial->println("ls           - List files");
            btSerial->println("dir <path>   - List directory contents");
            btSerial->println("read <file>  - Read file content");
            btSerial->println("play <file>  - Play song");
            btSerial->println("spark <file> - Play + relay");
            btSerial->println("ledon/off    - LED");
            btSerial->println("relayon/off  - Relay");
            btSerial->println("pwd          - Current path");
            btSerial->println("status       - SD card status");
            btSerial->println("debug        - Show debug info on OLED");
        }
    }
    else if (cmd.equalsIgnoreCase("status")) {
        if (btSerial) {
            if (sdCardReady && SD.cardSize() > 0) {
                btSerial->print("SD card: READY (");
                btSerial->print(SD.cardSize() / (1024*1024));
                btSerial->println("MB)");
            } else {
                btSerial->println("SD card: NOT READY");
            }
        }
    }
    else if (cmd.startsWith("cd ")) {
        String target = cmd.substring(3);
        target.trim();
        
        if (!sdCardReady) {
            if (btSerial) btSerial->println("ERR: SD card not ready");
            return;
        }
        
        if (target == "..") {
            int lastSlash = currentDir.lastIndexOf('/');
            if (lastSlash > 0) {
                currentDir = currentDir.substring(0, lastSlash);
            } else {
                currentDir = "/";
            }
        } else if (target == "/") {
            currentDir = "/";
        } else {
            String newPath = currentDir;
            if (newPath != "/") newPath += "/";
            newPath += target;
            
            File dir = SD.open(newPath);
            if (dir) {
                if (dir.isDirectory()) {
                    currentDir = newPath;
                    dir.close();
                } else {
                    if (btSerial) btSerial->println("ERR: not a directory");
                    return;
                }
            } else {
                if (btSerial) btSerial->println("ERR: dir not found");
                return;
            }
        }
        if (btSerial) {
            btSerial->print("$ ");
            btSerial->println(currentDir);
        }
    }
    else if (cmd.equalsIgnoreCase("ls")) {
        if (!sdCardReady) {
            if (btSerial) btSerial->println("ERR: SD card not ready");
            return;
        }
        
        File root = SD.open(currentDir);
        if (root && root.isDirectory()) {
            File file = root.openNextFile();
            while (file) {
                if (file.isDirectory()) {
                    if (btSerial) {
                        btSerial->print("[DIR]  ");
                        btSerial->println(file.name());
                    }
                } else {
                    if (btSerial) {
                        btSerial->print("[FILE] ");
                        btSerial->print(file.name());
                        btSerial->print(" (");
                        btSerial->print(file.size());
                        btSerial->println("B)");
                    }
                }
                file = root.openNextFile();
            }
            root.close();
        } else {
            if (btSerial) btSerial->println("ERR: cannot open directory");
        }
    }
    else if (cmd.equalsIgnoreCase("pwd")) {
        if (btSerial) {
            btSerial->print("$ ");
            btSerial->println(currentDir);
        }
    }
    else if (cmd.startsWith("play ")) {
        String filename = cmd.substring(5);
        filename.trim();
        String fullPath = currentDir;
        if (fullPath != "/") fullPath += "/";
        fullPath += filename;
        
        if (audio) {
            bool ok = audio->playSong(fullPath);
            if (btSerial) btSerial->println(ok ? "Playing..." : "File not found");
        }
    }
    else if (cmd.startsWith("spark ")) {
        String filename = cmd.substring(6);
        filename.trim();
        String fullPath = currentDir;
        if (fullPath != "/") fullPath += "/";
        fullPath += filename;
        
        digitalWrite(relayPin, HIGH);
        relayOnTime = millis();
        relayActive = true;
        if (btSerial) btSerial->println("Relay ON");
        
        if (audio) {
            bool ok = audio->playSong(fullPath);
            if (btSerial) btSerial->println(ok ? "Playing..." : "File not found");
        }
    }
    else if (cmd.equalsIgnoreCase("ledon")) {
        digitalWrite(ledPin, HIGH);
        if (btSerial) btSerial->println("LED ON");
    }
    else if (cmd.equalsIgnoreCase("ledoff")) {
        digitalWrite(ledPin, LOW);
        if (btSerial) btSerial->println("LED OFF");
    }
    else if (cmd.equalsIgnoreCase("relayon")) {
        digitalWrite(relayPin, HIGH);
        relayOnTime = millis();
        relayActive = true;
        if (btSerial) btSerial->println("Relay ON");
    }
    else if (cmd.equalsIgnoreCase("relayoff")) {
        digitalWrite(relayPin, LOW);
        relayActive = false;
        if (btSerial) btSerial->println("Relay OFF");
    }
    else if (cmd.startsWith("dir ")) {
        String path = cmd.substring(4);
        path.trim();
        ListPlayDirectory(path.c_str());
    }
    else if (cmd.startsWith("read ")) {
        String filename = cmd.substring(5);
        filename.trim();
        String fullPath = currentDir;
        if (fullPath != "/") fullPath += "/";
        fullPath += filename;
        ReadFile(fullPath.c_str());
    }
    else if (cmd.equalsIgnoreCase("debug")) {
        showDebugStatus();
    }
    else {
        if (btSerial) btSerial->println("Unknown command");
    }
}
