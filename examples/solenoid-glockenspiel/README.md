# Solenoid Glockenspiel Example

A 19-note MIDI-controlled glockenspiel using solenoid actuators.

## Hardware

- **19 solenoids** (12V push-type)
- **19 MOSFETs** (IRLZ44N)
- **19 flyback diodes** (1N5819)
- **Power supply**: 12V, 5A minimum

## Configuration

This configuration maps MIDI notes 60-78 (C4 to F#5) to GPIO pins 2-20.

Each solenoid triggers with a 30ms pulse when receiving the corresponding note.

### MIDI Mapping

| Note | Note Name | GPIO Pin | Solenoid |
|------|-----------|----------|----------|
| 60   | C4        | 2        | 1        |
| 61   | C#4       | 3        | 2        |
| 62   | D4        | 4        | 3        |
| 63   | D#4       | 5        | 4        |
| 64   | E4        | 6        | 5        |
| 65   | F4        | 7        | 6        |
| 66   | F#4       | 8        | 7        |
| 67   | G4        | 9        | 8        |
| 68   | G#4       | 10       | 9        |
| 69   | A4        | 11       | 10       |
| 70   | A#4       | 12       | 11       |
| 71   | B4        | 13       | 12       |
| 72   | C5        | 14       | 13       |
| 73   | C#5       | 15       | 14       |
| 74   | D5        | 16       | 15       |
| 75   | D#5       | 17       | 16       |
| 76   | E5        | 18       | 17       |
| 77   | F5        | 19       | 18       |
| 78   | F#5       | 20       | 19       |

## Wiring

Each solenoid connects via MOSFET as follows:

```
Pico GPx --[470Ω]-- MOSFET Gate
                    MOSFET Drain -- Solenoid -- +12V
                    MOSFET Source -- GND
                    Flyback Diode across Solenoid
```

See the main [wiring documentation](../../docs/WIRING_DETAILS.md) for complete circuit diagrams.

## Usage

1. Upload `config.json` to your Pico via USB mass storage mode
2. Connect to your DAW or MIDI controller
3. Play notes 60-78 to trigger the corresponding bars

## Customization

### Adjusting Strike Force

Modify the `d` (duration) value to change how long the solenoid stays energized:
- Lighter strike: 20-25ms
- Normal strike: 30-35ms  
- Harder strike: 40-50ms

**Warning**: Longer durations generate more heat. Don't exceed 100ms without proper cooling.

### Velocity Sensitivity

Add velocity filtering to prevent weak strikes:

```json
{
  "t": "n",
  "ch": 1,
  "n": 60,
  "p": 2,
  "a": "p",
  "d": 30,
  "vmin": 40,
  "vmax": 127
}
```

This ignores notes with velocity below 40.
