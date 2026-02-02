#pragma once
#include <Arduino.h>
#include "FS.h"
#include "SD.h"

void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
