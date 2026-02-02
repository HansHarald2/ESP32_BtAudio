#include "sd_audio.h"

bool SdAudio::begin(int csPin) {
    return SD.cardSize() > 0;
}

