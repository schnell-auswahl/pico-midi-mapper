# Pico MIDI Mapper

**A flexible, JSON-configurable MIDI-to-GPIO controller for Raspberry Pi Pico**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg)](https://platformio.org/)
[![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi%20Pico-red.svg)](https://www.raspberrypi.com/products/raspberry-pi-pico/)

Transform your Raspberry Pi Pico into a universal MIDI-controlled I/O interface. Control solenoids, LEDs, motors, relays, or any GPIO-compatible device using MIDI messages with an easy-to-configure JSON mapping system.

---

## 🚀 Key Feature: Drag & Drop Configuration

**No recompilation needed!** After the initial firmware flash:

1. **Hold the CONFIG button** (GPIO 22 to GND) while connecting USB
2. Your Pico appears as a **USB mass storage device** 
3. **Drag & drop** your `config.json` directly onto the drive
4. **Or edit the file** with any text editor right on the drive
5. Disconnect and reconnect - **done!** ✨

Perfect for:
- 🎵 Quick tweaks during live performances
- 🔧 Testing different MIDI mappings without code changes
- 👥 Non-programmers who want to customize behavior
- 🎹 Multiple configurations for different projects

---

## ✨ Features

- **🎹 Dual MIDI Input**: USB MIDI + UART MIDI (5-pin DIN) simultaneously
- **📍 Flexible GPIO Mapping**: Map any MIDI note or CC to any GPIO pin
- **⚡ Multiple Action Types**:
  - **Pulse** - Timed trigger (perfect for solenoids)
  - **Toggle** - On/Off switching (LEDs, relays)
  - **PWM** - Analog control via velocity or CC values
- **🎚️ Advanced Filtering**: Velocity ranges, sustain pedal support
- **🌐 Web-Based Editor**: Visual configuration tool at [elektrologue.com/midi-mapper-editor.html](https://elektrologue.com/midi-mapper-editor.html)
- **💾 Easy Updates**: USB mass storage mode for configuration changes
- **🔧 Up to 50 mappings**: Control multiple outputs independently

---

## 🎯 Use Cases

<table>
<tr>
<td width="33%">

### 🎺 Solenoid Instruments
Build MIDI-controlled acoustic instruments like glockenspiels, chimes, or percussion robots

</td>
<td width="33%">

### 💡 LED Control
Drive LED strips, matrices, or individual LEDs with velocity-sensitive brightness

</td>
<td width="33%">

### 🔌 Automation
Control relays, motors, pneumatic valves, or other actuators via MIDI

</td>
</tr>
</table>

---

## 🎹 Quick Start with the Web Editor

### 1. Create Your Configuration

Visit the **[MIDI Mapper Editor](https://elektrologue.com/midi-mapper-editor.html)** in your browser:

1. **Add mappings** using the intuitive interface
2. Set MIDI channel, note/CC number
3. Assign GPIO pins and actions
4. Configure pulse durations, PWM modes, velocity filters
5. **Export** your `config.json` file

![MIDI Mapper Editor](docs/images/midi-mapper-editor.png)
*Visual configuration - no coding required!*

### 2. Flash the Firmware (One Time)

```bash
# Clone this repository
git clone https://github.com/YOUR_USERNAME/pico-midi-mapper.git
cd pico-midi-mapper

# Build and upload (PlatformIO required)
pio run --target upload
pio run --target uploadfs
```

### 3. Upload Your Configuration

**Method A: Hardware Button**
1. Connect GPIO 22 to GND (or hold CONFIG button)
2. Connect USB cable
3. Pico appears as a USB drive
4. Drag your `config.json` onto the drive
5. Disconnect and reconnect without button

**Method B: MIDI SysEx** (no hardware button needed)
1. Send SysEx command: `F0 7D 47 43 01 F7`
2. Pico reboots as USB drive
3. Edit or replace `config.json`
4. Disconnect and reconnect

---

## 📖 Example: Solenoid Glockenspiel

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

[→ See more examples](examples/)

---

## 🛠️ Hardware Setup

### Minimal Setup (Testing)
- Raspberry Pi Pico
- USB cable
- That's it! Test with the onboard LED (GPIO 25)

### Full Setup (Solenoids/Actuators)
- Raspberry Pi Pico
- N-Channel MOSFETs (logic-level: IRLZ44N, IRL540N)
- Flyback diodes (1N5819 Schottky recommended)
- Gate resistors (220Ω - 1kΩ)
- External power supply (12V for solenoids)
- Optional: MIDI input circuit (6N138 optocoupler)

[→ Detailed wiring guide](docs/WIRING_DETAILS.md) | [→ Bill of materials](docs/BILL_OF_MATERIALS.md)

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

## 💡 Example Projects

### 🎼 Solenoid Glockenspiel
19-note electro-acoustic instrument
- [Configuration](examples/solenoid-glockenspiel/)
- MIDI channel 1, notes 60-78
- 30ms pulse duration

### 💡 LED Matrix
RGB LED control with velocity
- [Configuration](examples/led-control/)
- PWM brightness via velocity
- Multi-channel support

### 🔌 Relay Board
8-channel home automation
- [Configuration](examples/relay-board/)
- Toggle mode for relays
- Sustain pedal support

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- Built with [PlatformIO](https://platformio.org/)
- Uses [Adafruit TinyUSB Library](https://github.com/adafruit/Adafruit_TinyUSB_Arduino)
- Uses [Arduino MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library)
- Inspired by the maker and MIDI hacking community

---

## 🔗 Links

- **Web Configuration Tool**: [elektrologue.com/midi-mapper-editor.html](https://elektrologue.com/midi-mapper-editor.html)
- **Report Issues**: [GitHub Issues](https://github.com/YOUR_USERNAME/pico-midi-mapper/issues)
- **Raspberry Pi Pico**: [Official Documentation](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)

---

**Made with ❤️ for the MIDI and maker community**
