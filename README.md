# Pico MIDI Mapper

**A flexible, JSON-configurable MIDI-to-GPIO controller for Raspberry Pi Pico**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) [![GitHub release](https://img.shields.io/github/v/release/schnell-auswahl/pico-midi-mapper?label=Latest%20Release)](https://github.com/schnell-auswahl/pico-midi-mapper/releases/latest) [![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg)](https://platformio.org/) [![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi%20Pico-red.svg)](https://www.raspberrypi.com/products/raspberry-pi-pico/)

Transform your Raspberry Pi Pico into a universal MIDI-controlled I/O interface. Control solenoids, LEDs, motors, relays, or any GPIO-compatible device using MIDI messages - **all configured visually in your browser, no coding required!**

---

## Features

- **Dual MIDI Input**: USB MIDI + UART MIDI (5-pin DIN) simultaneously
- **MIDI Through**: Daisy-chain multiple Picos for unlimited I/O expansion
- **Flexible GPIO Mapping**: Map any MIDI note or CC to any GPIO pin
- **Multiple Action Types**: Pulse (solenoids), Toggle (relays), PWM (LEDs)
- **Advanced Filtering**: Velocity ranges, sustain pedal support
- **Web-Based Editor**: Visual node-based configuration tool
- **USB Mass Storage**: Update configurations without recompiling
- **Up to 50 mappings per Pico**: Scale with daisy-chaining

---

## Installation

### Step 1: Download Firmware

Get the latest pre-compiled firmware from [GitHub Releases](https://github.com/schnell-auswahl/pico-midi-mapper/releases/latest). Download the `.uf2` file (typically 200-500 KB).

### Step 2: Flash Firmware to Pico

Using BOOTSEL mode (works on all operating systems, no drivers needed):

1. Disconnect your Pico from USB
2. Hold down the BOOTSEL button on your Pico
3. While holding BOOTSEL, connect USB cable to computer
4. Release BOOTSEL - Pico appears as **RPI-RP2** USB drive
5. Drag and drop the `.uf2` file onto the RPI-RP2 drive
6. Pico automatically reboots and runs the firmware

You only need to flash firmware once. Config updates use a different method (see Step 4).

[→ Troubleshooting](docs/INSTALLATION.md#troubleshooting)

### Step 3: Create Your MIDI Mappings

**Option A: Web Editor (Recommended)**

![MIDI Mapper Editor Screenshot](docs/images/midi-mapper-editor.png)

Open the [MIDI Mapper Editor](https://elektrologue.com/midi-mapper-editor.html) in your browser:

1. Add MIDI input nodes (Note or Control Change)
2. Add GPIO output nodes (Pulse, Toggle, or PWM)
3. Connect nodes by dragging between ports
4. Configure each node (pin numbers, durations, velocity ranges)
5. Export and download your `config.json` file

**Option B: Manual JSON**

Write your configuration manually - see [Configuration Format](docs/CONFIGURATION_FORMAT.md) for details.

### Step 4: Upload Configuration to Pico

Using USB Mass Storage mode:

1. Disconnect your Pico from USB
2. Connect GPIO 22 to GND (or press CONFIG button if installed)
   ```
   GPIO 22 ──┤ ├── GND
   ```
3. While GPIO 22 is grounded, connect USB cable
4. Pico appears as **PICO FS** USB drive (LED blinks slowly)
5. Drag your `config.json` onto the PICO FS drive
6. Eject safely and disconnect USB
7. Reconnect normally (without GPIO 22 grounded)

Your mappings are now active.

Alternative: Send MIDI SysEx `F0 7D 47 43 01 F7` to enter config mode.

[→ USB Mass Storage details](docs/USB_MASS_STORAGE.md)

### Step 5: Connect and Test

Connect your MIDI source:
- **USB MIDI**: Connect MIDI controller or DAW to Pico's USB port
- **Hardware MIDI**: Connect 5-pin DIN to GPIO 1 (RX) - [wiring guide](docs/HARDWARE_SETUP.md)

Test your mappings by sending MIDI messages. GPIO pins trigger according to your configuration. The onboard LED (GPIO 25) can be used for testing without external hardware.

---

## Build from Source

For developers who want to modify the firmware:

```bash
# Install PlatformIO (if not already installed)
# https://platformio.org/install

# Clone repository
git clone https://github.com/schnell-auswahl/pico-midi-mapper.git
cd pico-midi-mapper

# Build and flash
pio run
pio run --target upload

# Build custom release
pio run
cp .pio/build/pico/firmware.uf2 my-custom-firmware.uf2
```
---

## Use Cases

Build MIDI-controlled projects:
- **Solenoid instruments** - Glockenspiels, chimes, percussion robots
- **LED control** - Strips, matrices, velocity-sensitive brightness
- **Automation** - Relays, motors, pneumatic valves

---

## Example Configuration

Simple solenoid glockenspiel mapping:

```json
{
  "version": 1,
  "maps": [
    {
      "t": "n",       // Type: Note
      "ch": 1,        // MIDI Channel 1
      "n": 60,        // Note 60 (Middle C)
      "p": 2,         // GPIO Pin 2
      "a": "p",       // Action: Pulse
      "d": 30         // Duration: 30ms
    },
    {
      "t": "n",
      "ch": 1,
      "n": 62,        // Note 62 (D)
      "p": 3,
      "a": "p",
      "d": 30
    }
  ]
}
```

More examples in the [examples/](examples/) folder.

---

## Hardware Setup

**Minimal Setup (Testing)**
- Raspberry Pi Pico
- USB cable
- Test with onboard LED (GPIO 25)

**Full Setup (Solenoids/Actuators)**
- Raspberry Pi Pico
- N-Channel MOSFETs (logic-level: IRLZ44N, IRL540N)
- Flyback diodes (1N5819 Schottky)
- Gate resistors (220Ω - 1kΩ)
- External power supply (12V for solenoids)
- Optional: MIDI input circuit (6N138 optocoupler)

See [Wiring Guide](docs/WIRING_DETAILS.md) and [Bill of Materials](docs/BILL_OF_MATERIALS.md) for details.

---

## 📚 Documentation

- **[Installation Guide](docs/INSTALLATION.md)** - First-time setup
- **[Configuration Format](docs/CONFIGURATION_FORMAT.md)** - JSON structure reference
- **[Hardware Setup](docs/HARDWARE_SETUP.md)** - Component selection & assembly
- **[Wiring Details](docs/WIRING_DETAILS.md)** - Circuit diagrams
- **[MIDI Features](docs/MIDI_FEATURES.md)** - Advanced MIDI control
- **[USB Mass Storage](docs/USB_MASS_STORAGE.md)** - Technical details

---

## 🔧 Configuration Reference

### Action Types

| Action | Code | Description | Required Fields |
|--------|------|-------------|----------------|
| **Pulse** | `"p"` | Brief trigger (e.g., solenoid strike) | `d` (duration) |
| **Toggle** | `"t"` | On/Off state (LED, relay) | - |
| **PWM** | `"w"` | Analog control (brightness, speed) | `pm` (PWM mode) |

### PWM Modes

| Mode | Code | Description |
|------|------|-------------|
| **Velocity** | `"v"` | Use MIDI velocity (0-127) as PWM value |
| **CC Value** | `"c"` | Use Control Change value for PWM |
| **Fixed** | `"f"` | Fixed PWM value with timed pulse |

[→ Complete configuration reference](docs/CONFIGURATION_FORMAT.md)

---

## Daisy-Chain Multiple Picos

Chain multiple Picos using MIDI Through for more than 50 mappings.

### How It Works

Each Pico forwards incoming MIDI to the next unit via its TX pin:

```
MIDI Source → Pico 1 (50 mappings) → Pico 2 (50 mappings) → Pico 3 (50 mappings)
              ├─ GPIO 2-26           ├─ GPIO 2-26           ├─ GPIO 2-26
              └─ Up to 24 outputs    └─ Up to 24 outputs    └─ Up to 24 outputs
```

Capabilities:
- Unlimited expansion
- Same MIDI source for all units
- Independent configs per Pico
- <1ms propagation delay per unit
- Each Pico powered via USB

### Wiring

Simple 3-wire connection between Picos:

```
Pico 1 (Primary)              Pico 2 (Secondary)
┌─────────────┐               ┌─────────────┐
│             │               │             │
│ GP0 (TX) ○──┼───────────────┼──○ GP1 (RX) │
│             │               │             │
│ GND ●───────┼───────────────┼───● GND     │
│             │               │             │
└─────────────┘               └─────────────┘
```

Pin connections:
- Pico 1 GP0 (TX) → Pico 2 GP1 (RX)
- Common GND between all Picos
- Optional: Pico 1 VBUS → Pico 2 VSYS (power daisy-chain, max 2-3 units)

For longer chains:
```
MIDI In → [Pico 1] → [Pico 2] → [Pico 3] → [Pico N]
           GP0  →    GP1  GP0 → GP1  GP0 → GP1
```

All GND pins connected, each Pico powered individually via USB.

### Configuration Tips

1. Use different MIDI channels for easier management
2. Or use non-overlapping notes/CCs on same channel
3. Label each Pico physically
4. Keep configs backed up (name them `pico1.json`, `pico2.json`, etc.)
5. Test individually before chaining

### Best Practices

- Keep TX/RX wires short (<50cm for reliable signal)
- Use twisted pair for TX/RX if longer runs needed
- Individual USB power recommended for 4+ Picos
- Common ground essential
- Star topology possible with MIDI splitter box

---

## Example Projects

**Solenoid Glockenspiel** - 19-note electro-acoustic instrument  
[Configuration](examples/solenoid-glockenspiel/) | MIDI channel 1, notes 60-78, 30ms pulse

**LED Matrix** - RGB LED control with velocity  
[Configuration](examples/led-control/) | PWM brightness via velocity, multi-channel

**Relay Board** - 8-channel home automation  
[Configuration](examples/relay-board/) | Toggle mode, sustain pedal support

---

## Contributing

Contributions welcome. Submit pull requests on GitHub.

---

## License

MIT License - see [LICENSE](LICENSE) file.

---

## Acknowledgments

Built with [PlatformIO](https://platformio.org/), uses [Adafruit TinyUSB](https://github.com/adafruit/Adafruit_TinyUSB_Arduino) and [Arduino MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library).

---

## Links

- Web Configuration Tool: [elektrologue.com/midi-mapper-editor.html](https://elektrologue.com/midi-mapper-editor.html)
- Report Issues: [GitHub Issues](https://github.com/schnell-auswahl/pico-midi-mapper/issues)
- Raspberry Pi Pico Docs: [raspberrypi.com/documentation](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
