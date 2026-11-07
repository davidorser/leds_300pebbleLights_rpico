# Building and uploading with Arduino CLI

The [README](../README.md) walks through setup in the Arduino IDE. This page covers the same steps with
[`arduino-cli`](https://arduino.github.io/arduino-cli/) instead. Tested with arduino-cli 1.3.1,
arduino-pico 6.3.1 and Adafruit NeoPixel 1.15.5.

All commands are run from the repository folder (`leds_300pebbleLights_rpico`). Create `secrets.h` from
`secrets.h.example` before compiling (see the README).

## 1. Install arduino-cli

Download it from the [installation page](https://arduino.github.io/arduino-cli/latest/installation/), or on
Windows:

```bash
winget install ArduinoSA.CLI
```

If you already have Arduino IDE 2.x, a copy is bundled with it (on Windows:
`C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe`). It shares the IDE's
board and library folders, so anything installed in one is visible to the other.

## 2. Install the Philhower Pico board core

Add the board package URL to the CLI configuration once, so you don't have to pass it on every command:

```bash
arduino-cli config init
```

```bash
arduino-cli config add board_manager.additional_urls https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
```

Then refresh the index and install the core:

```bash
arduino-cli core update-index
```

```bash
arduino-cli core install rp2040:rp2040
```

`arduino-cli config init` fails if a config file already exists; that's fine, skip it and run the
`config add` command.

## 3. Install the Adafruit NeoPixel library

```bash
arduino-cli lib install "Adafruit NeoPixel"
```

## 4. Compile

```bash
arduino-cli compile -b rp2040:rp2040:rpipicow .
```

`rp2040:rp2040:rpipicow` is the fully qualified board name (FQBN) for the Raspberry Pi Pico W.

## 5. Upload over USB

Find the Pico W's port:

```bash
arduino-cli board list
```

Then upload (replace `COM5` with your port, e.g. `/dev/ttyACM0` on Linux):

```bash
arduino-cli upload -b rp2040:rp2040:rpipicow -p COM5 .
```

If the board doesn't show a port (blank board, or a sketch that crashed USB), hold **BOOTSEL** while
plugging it in; it then mounts as a USB drive named `RPI-RP2` and the upload uses that instead.

You can also compile and upload in one step:

```bash
arduino-cli compile -b rp2040:rp2040:rpipicow -u -p COM5 .
```

## 6. Serial monitor

```bash
arduino-cli monitor -p COM5
```

Press `Ctrl+C` to exit.

## 7. Upload over the air (OTA)

Once the sketch is running and connected to Wi‑Fi, the Pico W advertises itself over mDNS. List ports with
a longer discovery time so network ports have a chance to appear:

```bash
arduino-cli board list --discovery-timeout 10s
```

Look for a `network` port with the board's IP address, then upload to it with your `OTA_PASSWORD`:

```bash
arduino-cli upload -b rp2040:rp2040:rpipicow -p 192.168.1.50 -l network --upload-field password=YOUR_OTA_PASSWORD --discovery-timeout 10s .
```

Your computer and the Pico W must be on the same subnet. This command hasn't been tested against the
hardware yet; if it fails, the Arduino IDE's network-port upload described in the README is the known-good
path.
