# Hardware

## Components

- **Raspberry Pi Pico W** (RP2040 + CYW43439 Wi‑Fi)
- **Addressable "pebble" LED strand** – a fairy-light style string of WS2812-compatible LEDs,
  ~300 pixels, 5 V, colour order **BGR** (configured as `NEO_BGR + NEO_KHZ800`)
- **5 V DC power supply** for the strand (and optionally the Pico)
- Optional: a switch or jumper on GPIO 2 for the manual "force on" override

## Wiring

```
                 +5 V supply
                   │      │
                   │      └──────────────► LED strand +5 V
                   │
           (optional) VSYS (pin 39)
   ┌───────────────┴───────┐
   │     Raspberry Pi      │
   │        Pico W         │
   │                       │
   │  GPIO22 (pin 29) ─────┼──────────────► LED strand DIN
   │  GND    (pin 28) ─────┼───────┬──────► LED strand GND
   │                       │       └──────► supply GND
   │  GPIO2  (pin 4)  ─────┼── switch ── 3V3(OUT) (pin 36)   (optional override)
   └───────────────────────┘
```

Notes:

- **Common ground is required** between the Pico, the strand and the power supply.
- The Pico drives the data line at **3.3 V**. Most WS2812-style strands accept this over a short lead,
  but if you see flicker or random colours, add a 3.3 V→5 V level shifter (e.g. 74AHCT125) or keep the
  data wire short. A ~330 Ω series resistor on DIN and a large capacitor (≈1000 µF) across the strand's
  supply input are good practice.
- If you power the Pico from the same 5 V supply via **VSYS**, don't also leave it on USB without a
  diode – or simply power it from USB during development.
- **GPIO 2** is configured `INPUT_PULLDOWN`; pulling it to 3.3 V forces full brightness regardless of the
  (placeholder) time-of-day logic.

## Power and voltage drop

These measurements were recorded in the sketch header: supply voltage at the **start** and **end** of the
strand with *N* LEDs lit at RGB (31, 31, 31):

| LEDs lit | Start (V) | End (V) | Drop (V) |
|---:|---:|---:|---:|
| 10  | 5.07 | 5.01 | 0.06 |
| 80  | 5.03 | 4.90 | 0.13 |
| 160 | 5.00 | 4.68 | 0.32 |
| 240 | 4.96 | 4.35 | 0.61 |
| 290 | 4.94 | 4.08 | 0.86 |
| 300 | 4.93 | 4.02 | 0.91 |
| 300 | 4.90 | 3.98 | 0.92 |

Takeaways:

- Fairy/pebble strands use thin wire, so the drop grows quickly with the number of lit LEDs. At ~300
  LEDs the far end sits around 4.0 V even at low brightness – brighter settings will tint the far end
  (blue/green fall off first) and can cause glitches.
- The animation deliberately keeps each LED dim (one colour channel per pixel, peaking ≈ 60/255) to stay
  within this budget.
- For brighter patterns, **inject power at the far end** (and/or the middle) of the strand, and size the
  supply for the worst-case current you'll actually display.
