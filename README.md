# 300 Pebble Lights – Raspberry Pi Pico W

Holiday light controller for a ~300-LED addressable "pebble" (fairy-string) light strand, driven by a
**Raspberry Pi Pico W**. The strand alternates **red** and **green** LEDs, each colour running its own
travelling sine wave at a different speed and direction, with random white "sparkles" on top.

The Pico W joins your Wi‑Fi so it can be **re-flashed over the air (OTA)** from the Arduino IDE, and it
listens for **UDP messages** on port 5005 as the starting point for remote control.

> History: started as a Particle Photon sketch (Dec 2023), moved to the Raspberry Pi Pico in Dec 2024,
> and gained Wi‑Fi / OTA / UDP support in Nov 2025. The sketch was formerly named
> `20251116_RPICO_LightsXmas_SinRedGrn_v006.ino`.

---

## Features

- **Red/green chase** – even pixels carry a red wave, odd pixels a green wave; the two waves move in
  opposite directions at different speeds, so the pattern never looks quite the same twice.
- **Sparkle** – a random pixel flashes white for one frame roughly every 0.5–1 s.
- **Soft brightness** – each wave peaks at about 60/255, keeping current draw and voltage drop
  manageable on a long strand.
- **OTA updates** – upload new firmware over Wi‑Fi using the Arduino IDE network port.
- **UDP listener** – receives text packets on port 5005 and replies `OK: <message>` (hook for future
  pattern selection).
- **Manual override input** – GPIO 2 (`D2`) forces the lights on when pulled high.

## Hardware

| Item | Notes |
|---|---|
| Raspberry Pi Pico W | Board `rp2040:rp2040:rpipicow` |
| Addressable pebble/fairy LED strand | WS2812-compatible, **BGR** colour order, 297 LEDs configured |
| 5 V power supply | Sized for the strand; see [docs/HARDWARE.md](docs/HARDWARE.md) |
| (Optional) momentary switch / jumper | GPIO 2 to 3.3 V for the "force on" override |

### Wiring summary

| Pico W pin | Connects to |
|---|---|
| GPIO 22 (physical pin 29) | LED strand **DIN** (data) |
| GND (e.g. physical pin 28) | LED strand **GND** and power-supply GND |
| VSYS (physical pin 39) | 5 V supply (optional – to power the Pico from the same supply) |
| GPIO 2 (physical pin 4) | Optional override switch to 3.3 V (internal pull-down) |

Full details, the measured voltage-drop table and power notes are in
[docs/HARDWARE.md](docs/HARDWARE.md).

## Software requirements

- [Arduino IDE 2.x](https://www.arduino.cc/en/software)
- **Raspberry Pi Pico/RP2040/RP2350** board core by Earle Philhower
  ([arduino-pico](https://github.com/earlephilhower/arduino-pico)) – tested with 6.3.1
- **Adafruit NeoPixel** library – tested with 1.15.5

Install steps for both are under [Getting started](#getting-started).

`WiFi`, `WiFiUdp`, `SimpleMDNS` and `ArduinoOTA` are bundled with the arduino-pico core.

## Getting started

1. **Clone** the repository. Keep the folder name `leds_300pebbleLights_rpico` – the Arduino IDE
   requires the sketch folder to match the `.ino` file name.
   ```bash
   git clone https://github.com/davidorser/leds_300pebbleLights_rpico.git
   ```
2. **Add the Philhower board package URL**: in the Arduino IDE open **File → Preferences** (on macOS,
   **Arduino IDE → Settings**). In **Additional boards manager URLs** add the URL below, then click **OK**.
   If the field already has URLs, put this one on its own line (click the icon beside the field to open
   the list editor).
   ```
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```
3. **Install the Pico board core**: open **Tools → Board → Boards Manager…**, search for `pico`, find
   **Raspberry Pi Pico/RP2040/RP2350** *by Earle F. Philhower, III*, and click **Install**. This core
   includes the `WiFi`, `WiFiUdp`, `SimpleMDNS` and `ArduinoOTA` libraries the sketch uses.
   > Don't confuse it with **Arduino Mbed OS RP2040 Boards** by Arduino, which has no Pico W Wi‑Fi support
   > and won't compile this sketch.
4. **Install the Adafruit NeoPixel library**: open **Tools → Manage Libraries…**, search for
   `Adafruit NeoPixel`, and click **Install**.
5. **Create your credentials file**: copy `secrets.h.example` to `secrets.h` in the same folder and fill
   in your Wi‑Fi SSID/password and an OTA password. `secrets.h` is git-ignored.
6. **Open** `leds_300pebbleLights_rpico.ino` in the Arduino IDE.
7. Select **Tools → Board → Raspberry Pi Pico/RP2040/RP2350 → Raspberry Pi Pico W**.
8. **First upload over USB**: hold BOOTSEL while plugging in the Pico W (or select its COM port) and
   click *Upload*.
9. Open the Serial Monitor to see the assigned IP address and `UDP listening on port 5005`.

Prefer the command line? The same setup, build and upload steps using `arduino-cli` are in
[docs/ARDUINO_CLI.md](docs/ARDUINO_CLI.md).

### Over-the-air (OTA) updates

After the first USB flash, the board advertises itself on the network as `OTA_HOSTNAME`
(default `djoOneStrandPico`) on port 2040. In the Arduino IDE choose it under
**Tools → Port → Network ports**, then *Upload*; enter `OTA_PASSWORD` when prompted.
Your PC and the Pico W must be on the same subnet (mDNS is used for discovery).

> If the Pico W can't connect to Wi‑Fi at boot it prints `Connection Failed! Rebooting...` and restarts
> after 5 s, so it will keep retrying until the network is available.

### Sending UDP messages

The sketch replies to any UDP packet on port 5005 with `OK: ` followed by the received text. The text is
stored in `rxBuf` (default `"redgrn"`) but **does not yet change the pattern** – it's a placeholder for
future mode selection.

Python example:

```python
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.settimeout(2)
s.sendto(b"hello", ("192.168.1.50", 5005))   # use the IP printed on the Serial Monitor
print(s.recvfrom(1024)[0].decode())           # -> OK: hello
```

## Configuration

| Setting | Where | Default | Meaning |
|---|---|---|---|
| `LED_PIN` | `.ino` | `22` | GPIO driving the strand |
| `LED_COUNT` | `.ino` | `297` | Number of LEDs |
| Colour order | `.ino` | `NEO_BGR + NEO_KHZ800` | Change if red/green/blue appear swapped |
| `sinAmp` | `loop()` | `80.0` | Wave amplitude (peak brightness ≈ ¾ of this) |
| `freq`, `freq2` | `loop()` | 0.4 Hz, 0.33 Hz | Wave speeds |
| `timeHour` | `loop()` | `18` (always on) | Placeholder for a real clock; lights are on when ≥ 17 |
| `LOCAL_PORT` | `.ino` | `5005` | UDP listen port |
| `djoDebug` | `.ino` | `false` | Print timing info to Serial each frame |
| `WIFI_SSID`, `WIFI_PASSWORD`, `OTA_HOSTNAME`, `OTA_PASSWORD` | `secrets.h` | – | Network credentials |

How the animation maths works is explained in [docs/ANIMATION.md](docs/ANIMATION.md).

## Repository layout

```
leds_300pebbleLights_rpico/
├── leds_300pebbleLights_rpico.ino   # the sketch
├── secrets.h.example                # template for Wi-Fi / OTA credentials
├── secrets.h                        # your copy (git-ignored, not in the repo)
├── docs/
│   ├── HARDWARE.md                  # wiring, power, voltage-drop measurements
│   ├── ANIMATION.md                 # how the red/green waves and sparkle work
│   └── ARDUINO_CLI.md               # building and uploading with arduino-cli
└── README.md
```

## Known limitations / ideas

- **No real clock** – `timeHour` is fixed at 18, so the lights are always on. The commented-out
  `Sunrise` code and the "off at midnight" comment show the intent; NTP (`configTime()` in the
  arduino-pico core) would be a natural next step.
- **UDP is echo-only** – received messages are not yet used to switch patterns.
- **Last LED unused** – with an odd `LED_COUNT` (297) the pixel loop stops at index 295, so pixel 296
  stays dark.
- **Sparkle only on red pixels** – `sparkle()` is only called with even indices, and picks from 0–299
  rather than `LED_COUNT`.

## Author

David Orser
