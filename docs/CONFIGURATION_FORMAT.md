# Configuration Format Reference

## Overview

The MIDI Mapper uses a compact JSON configuration file (`config.json`) stored on the Raspberry Pi Pico's filesystem. This file defines how MIDI events are mapped to GPIO pins.

## JSON Structure

```json
{
  "version": 1,
  "maps": [
    { /* Mapping object */ },
    { /* Mapping object */ }
  ]
}
```

## Field Reference

### Top-Level Fields

| Field | Type | Description |
|-------|------|-------------|
| `version` | Number | Format version (currently: 1) |
| `maps` | Array | Array of mapping objects |

### Mapping Object Fields

All fields use abbreviations to save memory.

| Field | Full Name | Type | Values | Required | Description |
|-------|-----------|------|--------|----------|-------------|
| `t` | Type | String | `"n"`, `"c"` | ✅ | MIDI type: `n`=Note, `c`=Control Change |
| `ch` | Channel | Number | 1-16 | ✅ | MIDI channel |
| `n` | Note | Number | 0-127 | if `t="n"` | MIDI note number |
| `c` | CC | Number | 0-127 | if `t="c"` | Control Change number |
| `p` | Pin | Number | 0-40 | ✅ | GPIO pin number |
| `a` | Action | String | `"p"`, `"w"`, `"t"` | ✅ | Pin action: `p`=Pulse, `w`=PWM, `t`=Toggle |
| `d` | Duration | Number | 0-2000 | conditional | Duration in milliseconds |
| `pm` | PWM Mode | String | `"v"`, `"c"`, `"f"` | if `a="w"` | PWM mode: `v`=Velocity, `c`=CC-Value, `f`=Fixed |
| `pv` | PWM Value | Number | 0-127 | if `pm="f"` | Fixed PWM value (Fixed mode only) |
| `vmin` | Velocity Min | Number | 0-127 | optional | Minimum velocity (default: 0) |
| `vmax` | Velocity Max | Number | 0-127 | optional | Maximum velocity (default: 127) |
| `s` | Sustain | Number | `0`, `1`, `-1` | optional | Sustain: `0`=Off only, `1`=On only, `-1`=Don't care |

### Required Fields by Action Type

**Pulse (`a="p"`):**
- Required: `t`, `ch`, `n`/`c`, `p`, `a`, `d`

**Toggle (`a="t"`):**
- Required: `t`, `ch`, `n`/`c`, `p`, `a`

**PWM Velocity (`a="w"`, `pm="v"`):**
- Required: `t`, `ch`, `n`/`c`, `p`, `a`, `pm`

**PWM CC (`a="w"`, `pm="c"`):**
- Required: `t`, `ch`, `n`/`c`, `p`, `a`, `pm`

**PWM Fixed (`a="w"`, `pm="f"`):**
- Required: `t`, `ch`, `n`/`c`, `p`, `a`, `pm`, `d`, `pv`

## Examples

### 1. Simple Note-to-Pulse Mapping
MIDI Note 60 (Middle C) on channel 1 → GPIO 25 pulse for 30ms

```json
{
  "t": "n",
  "ch": 1,
  "n": 60,
  "p": 25,
  "a": "p",
  "d": 30
}
```

**Use case**: Trigger a solenoid or LED flash

### 2. CC-to-PWM (CC Value Mode)
CC #7 on channel 2 → GPIO 12 PWM controlled by CC value

```json
{
  "t": "c",
  "ch": 2,
  "c": 7,
  "p": 12,
  "a": "w",
  "pm": "c"
}
```

**Use case**: LED dimmer or motor speed control

### 3. Note-to-PWM with Velocity Filter
Note 48 on channel 1, only velocities 20-100 → GPIO 5 PWM

```json
{
  "t": "n",
  "ch": 1,
  "n": 48,
  "p": 5,
  "a": "w",
  "pm": "v",
  "vmin": 20,
  "vmax": 100
}
```

**Use case**: Velocity-sensitive LED with soft threshold

### 4. Toggle Mode
Note 72 on channel 1 → GPIO 18 toggle on/off

```json
{
  "t": "n",
  "ch": 1,
  "n": 72,
  "p": 18,
  "a": "t"
}
```

**Use case**: Relay or LED on/off switching

### 5. PWM Fixed Mode
Note 84 on channel 1 → GPIO 10 PWM at 50% for 100ms

```json
{
  "t": "n",
  "ch": 1,
  "n": 84,
  "p": 10,
  "a": "w",
  "pm": "f",
  "d": 100,
  "pv": 64
}
```

**Use case**: Fixed-brightness LED flash

### 6. Sustain Pedal Filtering
Only trigger when sustain pedal is pressed

```json
{
  "t": "n",
  "ch": 1,
  "n": 60,
  "p": 2,
  "a": "p",
  "d": 30,
  "s": 1
}
```

**Use case**: Different behaviors with/without sustain

### 7. Complete Configuration Example

```json
{
  "version": 1,
  "maps": [
    {
      "t": "n",
      "ch": 1,
      "n": 60,
      "p": 2,
      "a": "p",
      "d": 30
    },
    {
      "t": "c",
      "ch": 1,
      "c": 7,
      "p": 3,
      "a": "w",
      "pm": "c"
    },
    {
      "t": "n",
      "ch": 1,
      "n": 64,
      "p": 4,
      "a": "t"
    },
    {
      "t": "n",
      "ch": 1,
      "n": 65,
      "p": 5,
      "a": "w",
      "pm": "v",
      "vmin": 40,
      "vmax": 127
    }
  ]
}
```

## Action Types Explained

### Pulse (`"p"`)

Activates the pin for a specified duration, then deactivates.

**Parameters:**
- `d` (duration): Milliseconds to keep pin HIGH

**Timing diagram:**
```
         ┌─────────┐
Pin  ────┤         └────
         ├─────────┤
         d (e.g., 30ms)
```

**Best for:**
- Solenoid actuators
- Short LED flashes
- Momentary relays

### Toggle (`"t"`)

Changes pin state on each Note On event. Note Off is ignored.

**State transitions:**
```
Note On → Pin: OFF → ON
Note On → Pin: ON  → OFF
Note On → Pin: OFF → ON
...
```

**Best for:**
- LED on/off
- Relay switching
- State-based control

### PWM (`"w"`)

Pulse Width Modulation for analog-like control (0-127 maps to 0-100%).

**Modes:**
- **Velocity (`"v"`)**: PWM duty cycle from note velocity
- **CC Value (`"c"`)**: PWM duty cycle from CC value (continuous)
- **Fixed (`"f"`)**: Fixed PWM value for duration `d`

**Best for:**
- LED brightness
- Motor speed
- Servo positioning

## Optimization Tips

### Memory Usage

Average sizes per mapping:
- Simple mapping: ~60 bytes
- Complex mapping (with filters): ~80 bytes
- Header overhead: ~30 bytes

**Capacity:**
- Maximum 50 mappings supported
- Actual limit depends on complexity

### Omit Default Values

Save space by omitting defaults:
- `vmin=0` and `vmax=127` can be omitted
- Only include `s` if using sustain filtering

### Compact Keys

All field names are 1-2 characters:
- Saves memory
- Faster parsing
- More mappings possible

## Signal Flow

```
MIDI In → Velocity Filter → Sustain Filter → GPIO Output
            (optional)        (optional)
```

1. **MIDI Input**: Receives note/CC events
2. **Velocity Filter**: Checks `vmin`/`vmax` if present
3. **Sustain Filter**: Checks sustain pedal state if `s` is set
4. **GPIO Output**: Activates pin with specified action

## Web Editor Integration

The [MIDI Mapper Editor](https://elektrologue.com/midi-mapper-editor.html) provides a visual interface for creating configurations:

1. Add mappings via drag-and-drop interface
2. Visual representation of signal flow
3. Live validation
4. Export to `config.json`

## Troubleshooting

### Configuration Not Loading

**Check:**
- Valid JSON syntax (use a validator)
- `version` field is present and set to `1`
- `maps` is an array
- All required fields are present

**Serial output will show:**
```
=== Loading Config ===
Mapping 0: note_on Note=60 Pin=25 Action=0
=== 6 Mappings loaded ===
```

### Pin Not Activating

**Check:**
- Pin number is valid (0-40 for Pico)
- Pin is not used by USB/UART (GPIO 0, 1 for UART)
- MIDI channel matches
- Velocity is within `vmin`/`vmax` range

### PWM Not Working

**Check:**
- PWM mode (`pm`) is specified
- For `pm="f"`, both `d` and `pv` are set
- Pin supports PWM (most Pico pins do)

## Version History

**Version 1** (Current):
- Compact JSON structure with abbreviations
- Support for Pulse, Toggle, and PWM actions
- Velocity filtering
- Sustain pedal support
- Up to 50 mappings
