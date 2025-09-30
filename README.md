# ESP32 BtAudio Project

This project is a skeleton setup for an ESP32 audio system using Bluetooth A2DP sink and optional I2S output. It is built with [PlatformIO](https://platformio.org/) and the Arduino framework.

## Features

- **Bluetooth A2DP Sink** – Receive audio from Bluetooth devices.
- **I2S Output** – Send audio to external DACs or speakers.
- **AudioTools integration** – Supports audio processing and effects.
- **Expandable** – Ready to add SD card support or other audio sources.

## Hardware

- ESP32 development board (e.g., ESP32 Dev Module)
- I2S DAC or audio output device
- Optional: SD card module for audio files

### Pin Configuration (default)

| Signal | Pin |
|--------|-----|
| BCK    | 26  |
| LRCK   | 33  |
| DATA   | 22  |

You can modify these pins in `bt_audio.h`.

## Project Structure

