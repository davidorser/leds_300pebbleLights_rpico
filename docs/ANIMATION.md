# Animation

Every pass through `loop()` the sketch recomputes all pixels from the current time
`t = millis() / 1000` (seconds), then calls `strip.show()`. There is no frame delay – the frame rate is
set by how long it takes to compute and push ~300 pixels.

Pixels are processed in **pairs**: even index `i` is red, odd index `i+1` is green.

## Red wave (even pixels)

```
R(i, t) = max(0, 80 · sin(ω₁·t + 2π·i/60) − 20)
ω₁ = 2π · 0.4  rad/s    (0.4 Hz)
```

- **Spatial period:** 60 pixel indices (30 red LEDs per wave).
- **Direction / speed:** the crest moves toward pixel 0 at 0.4 × 60 = **24 pixels/s**.
- **Brightness:** peaks at 80 − 20 = **60**; subtracting 20 and clipping at zero makes each lit band
  narrower than half a wave, leaving dark gaps between red bands.

## Green wave (odd pixels)

```
G(i, t) = max(0, 80 · sin(−ω₂·t/2 + 2π·i/60 + 2π/3) − 80/3)
ω₂ = 2π · 0.33  rad/s   → effective 0.165 Hz after the /2
```

- **Direction / speed:** the negative time term makes it travel the opposite way (toward the end of the
  strand) at 0.165 × 60 ≈ **10 pixels/s**.
- **Phase offset:** 2π/3 (120°) relative to the red wave.
- **Brightness:** peaks at 80 − 26.7 ≈ **53**; the larger offset gives narrower green bands than red.

Because the two waves have different speeds and directions, red and green bands continually slide past
each other.

The blue channel and the alternate red/green formulas are left commented out in the source so other
colour combinations can be re-enabled easily.

## Sparkle

`sparkle(i)` is called for each even pixel after its colour is set:

- It picks a random location `rand() % 300` and only fires if it equals `i` **and** the current time has
  passed `timeToFlash`.
- When it fires, the pixel is set to full white (255, 255, 255) for that one frame and `timeToFlash` is
  pushed forward by a random 500–949 ms.

The result is a brief white glint roughly every 0.5–1 s at a random spot. Because `timeToFlash` starts
at 0 and only advances by one interval per sparkle, there is a short burst of extra sparkles right after
boot while it catches up with `millis()`.

## Brightness gate

```cpp
timeHour = 18;
if (timeHour >= 17 || digitalRead(D2)) strip.setBrightness(255);
else                                   strip.setBrightness(0);
```

`timeHour` is a placeholder for a real clock, so the strand is currently always on. Once a time source is
added, the lights will be on from 17:00 and off otherwise, with GPIO 2 as a manual override.
