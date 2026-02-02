#include "sd_helper.h"

void createDir(fs::FS &fs, const char * path) {
    fs.mkdir(path);
}

void removeDir(fs::FS &fs, const char * path) {
    fs.rmdir(path);
}
