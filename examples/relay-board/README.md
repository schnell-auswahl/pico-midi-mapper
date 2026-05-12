# Relay Board Example

8-channel relay control for home automation or stage lighting.

## Hardware

- **8 relays** (optocoupler-isolated recommended)
- **8 GPIOs** from Pico
- **Relay module** (5V or 3.3V compatible)
- **External power** for relay coils if needed

## Configuration

This example uses toggle mode for relay control:
- Each MIDI note toggles the corresponding relay
- Sustain pedal can be used to hold relays on

### MIDI Mapping

| Note | Function | GPIO | Relay |
|------|----------|------|-------|
| 60   | Light 1  | 2    | 1     |
| 61   | Light 2  | 3    | 2     |
| 62   | Light 3  | 4    | 3     |
| 63   | Light 4  | 5    | 4     |
| 64   | Fan      | 6    | 5     |
| 65   | Pump     | 7    | 6     |
| 66   | Motor    | 8    | 7     |
| 67   | Spare    | 9    | 8     |

## Wiring

For optocoupler relay modules:

```
Pico GPx -- Relay Module IN1-8
Pico GND -- Relay Module GND
Pico 5V  -- Relay Module VCC (if 5V tolerant)
```

For direct relay control:

```
Pico GPx --[1kΩ]-- Transistor Base
                   Transistor Collector -- Relay Coil -- +5V
                   Transistor Emitter -- GND
                   Flyback Diode across Relay Coil
```

## Usage

### Basic Toggle

1. Send MIDI Note On to toggle relay state
2. Note Off is ignored (toggle only happens on Note On)

### Momentary Mode

Change action to `"p"` for momentary contact:
- Note On = Relay closes
- Relay stays closed for duration
- Then opens automatically

```json
{
  "t": "n",
  "ch": 1,
  "n": 60,
  "p": 2,
  "a": "p",
  "d": 1000
}
```

### Sustain Pedal Control

Use sustain pedal to keep relays engaged:

```json
{
  "t": "n",
  "ch": 1,
  "n": 60,
  "p": 2,
  "a": "t",
  "s": 1
}
```

With `"s": 1`, the relay only toggles when sustain pedal is pressed.

## Safety Considerations

⚠️ **WARNING**: Relays can switch high voltage/current!

- Always use optocoupler isolation for safety
- Don't exceed relay ratings (check datasheet)
- Use proper enclosures for AC wiring
- Follow local electrical codes
- Consider using solid-state relays for frequent switching

## Advanced: Scene Recall

Use MIDI Program Change to recall scenes (requires custom firmware modification).
