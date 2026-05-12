# MIDI Features & Control

Advanced MIDI functionality and control options for the Pico MIDI Mapper.

---

## Supported MIDI Messages

### Note Messages

**Note On** (`0x90 - 0x9F`)
- Channel: 1-16
- Note number: 0-127
- Velocity: 0-127

**Note Off** (`0x80 - 0x8F`)
- Channel: 1-16
- Note number: 0-127
- Velocity: 0-127 (usually ignored)

**Running Status:**
- Note On with velocity 0 = Note Off (common optimization)
- Supported automatically

---

### Control Change (CC)

**CC Messages** (`0xB0 - 0xBF`)
- Channel: 1-16
- Controller number: 0-127
- Value: 0-127

**Special CCs:**
- **CC 64 (Sustain Pedal):** 0-63 = Off, 64-127 = On
- **CC 7 (Volume):** Common for PWM brightness/speed
- **CC 1 (Modulation):** User-definable
- **CC 11 (Expression):** User-definable

---

### System Exclusive (SysEx)

**Custom SysEx Commands:**

```
F0 7D 47 43 <CMD> F7
```

- `F0` = SysEx start
- `7D` = Non-commercial manufacturer ID
- `47 43` = "GC" (GPIO Controller)
- `<CMD>` = Command byte
- `F7` = SysEx end

**Implemented Commands:**

| CMD | Hex | Function |
|-----|-----|----------|
| Enter Config Mode | `01` | Reboot to USB mass storage |

**Future Expansion:**
- `02` - Factory reset
- `03` - Get firmware version
- `04` - Toggle debug mode
- `10`-`1F` - Direct GPIO control

---

## MIDI Input Modes

### USB MIDI

**How to connect:**
1. Connect Pico to computer via USB
2. Device appears as "MIDI Device" or "Pico MIDI"
3. Send MIDI from DAW, MIDI software, or hardware interface

**Advantages:**
- No additional hardware required
- Powered via USB
- Easy debugging (serial monitor)
- Low latency

**Use cases:**
- DAW-controlled projects
- Computer-based installations
- Development and testing

---

### UART MIDI (5-Pin DIN)

**How to connect:**
1. Build MIDI input circuit (see [Wiring Details](WIRING_DETAILS.md))
2. Connect 5-pin MIDI cable from source
3. MIDI messages appear on GPIO 1 (UART RX)

**Advantages:**
- Industry standard
- Galvanic isolation (noise immunity)
- Works with hardware synthesizers, controllers
- Standalone operation (no computer needed)

**Use cases:**
- Live performance
- Studio integration
- Hardware MIDI controllers
- Modular synthesizer systems

---

### Dual MIDI (USB + UART Simultaneous)

**Both inputs merged!**

Messages from USB and UART are combined and processed identically.

**Use cases:**
- Backup input during performance
- Control from multiple sources
- Development (USB) + production (UART)

---

## Velocity Handling

### Velocity-Sensitive Actions

**PWM Velocity Mode** (`pm="v"`)

MIDI velocity (0-127) maps directly to PWM duty cycle:

```
Velocity 0   → 0% duty cycle (off)
Velocity 64  → 50% duty cycle
Velocity 127 → 100% duty cycle (full on)
```

**Example:**
```json
{
  "t": "n",
  "ch": 1,
  "n": 60,
  "p": 5,
  "a": "w",
  "pm": "v"
}
```

**Use cases:**
- Velocity-sensitive LED brightness
- Motor speed control
- Dynamic solenoid strikes

---

### Velocity Filtering

Ignore notes outside a velocity range:

```json
{
  "t": "n",
  "ch": 1,
  "n": 60,
  "p": 2,
  "a": "p",
  "d": 30,
  "vmin": 40,
  "vmax": 120
}
```

**Behavior:**
- Velocity < 40: Ignored
- Velocity 40-120: Triggered
- Velocity > 120: Ignored

**Use cases:**
- Filter out weak/ghost notes
- Prevent accidental triggers
- Limit maximum force
- Create velocity layers (different pins for different ranges)

---

## Sustain Pedal Control

### Sustain Filtering

Control whether a mapping responds based on sustain pedal state:

**Only without sustain** (`s: 0`):
```json
{
  "t": "n",
  "ch": 1,
  "n": 60,
  "p": 2,
  "a": "p",
  "d": 30,
  "s": 0
}
```

**Only with sustain** (`s: 1`):
```json
{
  "t": "n",
  "ch": 1,
  "n": 60,
  "p": 3,
  "a": "p",
  "d": 50,
  "s": 1
}
```

**Don't care** (omit `s` field or `s: -1`):
- Triggered regardless of sustain state

---

### Sustain Use Cases

**Different sounds per pedal state:**
```json
[
  {
    "t": "n",
    "ch": 1,
    "n": 60,
    "p": 2,
    "a": "p",
    "d": 20,
    "s": 0
  },
  {
    "t": "n",
    "ch": 1,
    "n": 60,
    "p": 3,
    "a": "p",
    "d": 50,
    "s": 1
  }
]
```

**Result:**
- Same note, different outputs based on pedal
- Short pulse without sustain, long pulse with sustain

---

## Web MIDI API Integration

### JavaScript Control

**Basic setup:**

```javascript
// Request MIDI access
const midiAccess = await navigator.requestMIDIAccess({ sysex: true });

// Find Pico
let picoOutput = null;
for (let output of midiAccess.outputs.values()) {
    if (output.name.includes('Pico') || output.name.includes('MIDI')) {
        picoOutput = output;
        break;
    }
}
```

---

### Send MIDI Notes

```javascript
function sendNote(note, velocity, channel = 1) {
    const noteOn = 0x90 + (channel - 1);
    const noteOff = 0x80 + (channel - 1);
    
    // Note On
    picoOutput.send([noteOn, note, velocity]);
    
    // Note Off after 100ms
    setTimeout(() => {
        picoOutput.send([noteOff, note, 0]);
    }, 100);
}

// Example: Trigger Note 60 (Middle C)
sendNote(60, 100);
```

---

### Send Control Change

```javascript
function sendCC(cc, value, channel = 1) {
    const controlChange = 0xB0 + (channel - 1);
    picoOutput.send([controlChange, cc, value]);
}

// Example: Set PWM to 50% via CC 7
sendCC(7, 64);  // 64/127 ≈ 50%
```

---

### Enter Config Mode via Web

```javascript
function enterConfigMode() {
    // SysEx: F0 7D 47 43 01 F7
    const sysex = [0xF0, 0x7D, 0x47, 0x43, 0x01, 0xF7];
    picoOutput.send(sysex);
    
    alert('Pico will reboot as USB drive in 3 seconds...');
}
```

---

### Play Melody

```javascript
async function playMelody() {
    const melody = [
        { note: 60, duration: 200 },  // C
        { note: 62, duration: 200 },  // D
        { note: 64, duration: 200 },  // E
        { note: 65, duration: 400 },  // F (longer)
    ];
    
    for (let m of melody) {
        sendNote(m.note, 100);
        await sleep(m.duration);
    }
}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}
```

---

## Advanced MIDI Patterns

### Velocity Layers

Different GPIO outputs for different velocity ranges:

```json
[
  {
    "t": "n",
    "ch": 1,
    "n": 60,
    "p": 2,
    "a": "p",
    "d": 20,
    "vmin": 0,
    "vmax": 40
  },
  {
    "t": "n",
    "ch": 1,
    "n": 60,
    "p": 3,
    "a": "p",
    "d": 30,
    "vmin": 41,
    "vmax": 80
  },
  {
    "t": "n",
    "ch": 1,
    "n": 60,
    "p": 4,
    "a": "p",
    "d": 40,
    "vmin": 81,
    "vmax": 127
  }
]
```

**Result:** Soft/medium/hard strikes on different solenoids

---

### Round-Robin Triggering

Same note triggers different outputs in sequence:

**Implementation:** Requires firmware modification
**Use case:** Prevent mechanical fatigue, distribute wear

---

### Polyphonic Mode

Multiple notes simultaneously:

**Works automatically!** Each mapping is independent.

**Example:**
- Note 60 → GPIO 2
- Note 62 → GPIO 3
- Note 64 → GPIO 4

All can be active at once.

---

## DAW Integration

### Ableton Live

1. **Preferences → Link/Tempo/MIDI**
2. **Track:** Enable "Output: Pico MIDI"
3. **Create MIDI track**
4. **Add MIDI clip** with notes
5. **Play!**

**Tips:**
- Use Drum Rack for easy note mapping
- Create MIDI effects chains
- Use velocity randomization

---

### Logic Pro X

1. **Preferences → MIDI → Devices**
2. **Add Device:** Pico MIDI
3. **Create Software Instrument Track**
4. **Set output to Pico MIDI**
5. **Draw notes in Piano Roll**

---

### FL Studio

1. **Options → MIDI Settings**
2. **Enable Pico in Output list**
3. **Channel Rack:** Add instrument
4. **Route to Pico output**

---

## MIDI Monitor / Debugging

### Software Tools

**macOS:**
- MIDI Monitor (free)
- Snoize

**Windows:**
- MIDI-OX (free)
- MIDI Tools

**Linux:**
- `amidirecord`, `amidi`
- QjackCtl

**Cross-platform:**
- Web MIDI Console (browser-based)

---

### Serial Monitor

View MIDI processing in real-time:

```bash
pio device monitor -b 115200
```

**Output:**
```
MIDI Note On: Ch=1 Note=60 Vel=100
Mapping 0 matched: Pin 25
Trigger Pin 25 (30ms pulse)
MIDI Note Off: Ch=1 Note=60
```

**Enable debug mode:**
```cpp
#define DEBUG_MODE  // In platformio.ini build_flags
```

---

## Latency Considerations

### USB MIDI Latency

**Typical: 1-3ms**
- USB polling interval
- Processing time
- OS driver overhead

**Minimal for most applications!**

---

### UART MIDI Latency

**Typical: <1ms**
- Direct hardware interrupt
- No OS involvement
- Lower than USB

**Best for real-time performance**

---

### GPIO Response Time

**Pulse action:** <100μs (0.1ms)
**PWM update:** 1-2ms

**Total latency (MIDI to GPIO):**
- USB: ~2-5ms
- UART: ~1-2ms

**Imperceptible for human timing!**

---

## MIDI Thru (Daisy-Chaining)

**Not currently implemented**

To add MIDI THRU:
1. Modify firmware to echo received MIDI to GPIO 0 (TX)
2. Add MIDI output circuit (220Ω resistors)
3. Configure in code

**Use case:** Chain multiple Pico MIDI Mappers

---

## Troubleshooting MIDI

### No MIDI Messages Received

**Check:**
- Is device in MIDI mode (not config mode)?
- LED blinks 3× on startup?
- Correct USB cable (data-capable)?
- MIDI channel matches configuration?

**Serial monitor shows:**
```
MIDI Note received: Ch=X Note=Y Vel=Z
```

---

### Wrong Notes Triggering

**Check:**
- Correct note number in config?
- MIDI channel matches?
- Velocity filter not too restrictive?

---

### Delayed Response

**Possible causes:**
- USB buffer full (too many messages)
- PWM update rate (try reducing)
- Computer CPU load (try direct UART MIDI)

---

## Advanced Topics

### MIDI Clock & Sync

**Not currently implemented**

Could be added for:
- Tempo-synced effects
- Beat-aligned triggering
- Step sequencer mode

### MIDI Learn

**Not currently implemented**

Potential feature:
- Press button
- Send MIDI note
- Auto-configure mapping

### Multi-Pico Systems

**Connect multiple Picos:**
- Each with different MIDI channel
- 16 channels × 24 GPIOs = 384 outputs!
- Use MIDI splitter or USB hub

---

## Resources

- **MIDI Specification:** [midi.org](https://www.midi.org/specifications)
- **Web MIDI API:** [MDN Docs](https://developer.mozilla.org/en-US/docs/Web/API/Web_MIDI_API)
- **MIDI Message Reference:** [midi.org/message](https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message)

---

## Related Documentation

- [Configuration Format →](CONFIGURATION_FORMAT.md)
- [USB Mass Storage →](USB_MASS_STORAGE.md)
- [Hardware Setup →](HARDWARE_SETUP.md)
