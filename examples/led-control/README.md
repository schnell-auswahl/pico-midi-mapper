# LED Control Example

MIDI-controlled LED lighting with velocity-sensitive brightness.

## Hardware

- **8 LEDs** (or LED strips)
- **8 MOSFETs** (for PWM control)
- **Current limiting resistors** (if using discrete LEDs)
- **Power supply**: Appropriate for your LED type

## Configuration

This example demonstrates three different LED control methods:

1. **PWM via Velocity** - Brightness controlled by MIDI velocity
2. **PWM via CC** - Brightness controlled by continuous controller
3. **Toggle Mode** - Simple on/off switching

### Example 1: Velocity-Sensitive LED (GPIO 2)

MIDI Note 60 controls brightness via velocity:
- Velocity 0 = Off
- Velocity 127 = Full brightness

### Example 2: CC-Controlled LED (GPIO 3)

MIDI CC 7 (Volume) controls brightness continuously:
- CC value 0 = Off
- CC value 127 = Full brightness

### Example 3: Toggle LED (GPIO 4)

MIDI Note 62 toggles LED on/off:
- Note On = Toggle state
- Note Off = Ignored

## Wiring

For PWM-capable LEDs:

```
Pico GPx --[470Ω]-- MOSFET Gate
                    MOSFET Drain -- LED Anode
                    LED Cathode -- GND
```

For LED strips with separate power:

```
Pico GPx --[470Ω]-- MOSFET Gate
                    MOSFET Drain -- LED Strip -
                    MOSFET Source -- GND (common with Pico)
                    LED Strip + -- External PSU +
```

## Usage

1. Upload configuration to Pico
2. Send MIDI notes or CC messages
3. Observe LED brightness changes

## Customization

### RGB LED Control

Map three GPIOs to RGB channels:

```json
{
  "t": "c",
  "ch": 1,
  "c": 20,
  "p": 2,
  "a": "w",
  "pm": "c"
}
```

Use CC 20, 21, 22 for R, G, B respectively.

### Fade Effects

The firmware handles PWM automatically. For smooth fades, send incremental CC values from your MIDI controller or DAW.
