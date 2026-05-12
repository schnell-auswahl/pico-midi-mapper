# Installation Guide

## Prerequisites

- **Raspberry Pi Pico** (or Pico W)
- **USB cable** (micro-USB to USB-A/C)
- **PlatformIO** installed (via VS Code extension or CLI)
- **Computer** with macOS, Windows, or Linux

## Quick Start

### 1. Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/pico-midi-mapper.git
cd pico-midi-mapper
```

### 2. Build and Upload Firmware

```bash
# Compile and upload firmware
pio run --target upload

# Upload filesystem (config.json)
pio run --target uploadfs
```

**Note:** You only need to do `uploadfs` once. After that, use USB mass storage mode to update configurations.

### 3. Test the Installation

**Test with onboard LED:**
1. Connect your Pico to a MIDI source (DAW, MIDI controller, etc.)
2. Send MIDI Note 60 (Middle C) on channel 1
3. The onboard LED should blink briefly

**Serial monitor output:**
```bash
pio device monitor -b 115200
```

Expected output:
```
=== MIDI to GPIO Mapper ===
=== Loading Config ===
Mapping 0: note_on Note=60 Pin=25 Action=0
=== 6 Mappings loaded ===
Ready!
```

## Installation Methods

### Method 1: PlatformIO IDE (Recommended)

1. **Install VS Code**: https://code.visualstudio.com/
2. **Install PlatformIO Extension**:
   - Open VS Code
   - Go to Extensions (Cmd+Shift+X / Ctrl+Shift+X)
   - Search for "PlatformIO IDE"
   - Click Install

3. **Open Project**:
   - File → Open Folder
   - Select the `pico-midi-mapper` directory

4. **Upload**:
   - Click the PlatformIO icon in sidebar
   - Under "Project Tasks":
     - Click `pico` → `General` → `Upload` (for firmware)
     - Click `pico` → `Platform` → `Upload Filesystem Image` (for config)

### Method 2: PlatformIO CLI

1. **Install PlatformIO Core**:
   ```bash
   pip install platformio
   ```

2. **Navigate to project**:
   ```bash
   cd pico-midi-mapper
   ```

3. **Build and upload**:
   ```bash
   pio run -t upload      # Upload firmware
   pio run -t uploadfs    # Upload config.json
   ```

### Method 3: Manual BOOTSEL Upload

If automatic upload fails:

1. **Enter BOOTSEL mode**:
   - Disconnect Pico
   - Hold BOOTSEL button
   - Connect USB while holding button
   - Pico appears as USB drive "RPI-RP2"

2. **Build firmware**:
   ```bash
   pio run
   ```

3. **Copy firmware**:
   - Find compiled `.uf2` file in `.pio/build/pico/`
   - Drag to RPI-RP2 drive
   - Pico automatically reboots

4. **Upload filesystem** (after firmware is running):
   ```bash
   pio run -t uploadfs
   ```

## Project Structure

```
pico-midi-mapper/
├── src/
│   └── main.cpp              # Main firmware code
├── data/
│   └── config.json           # Default configuration
├── docs/                     # Documentation
├── examples/                 # Example configurations
├── platformio.ini            # Build configuration
├── README.md
└── LICENSE
```

## Configuration Modes

### MIDI Mode (Normal Operation)

**How to enter:**
- Simply connect Pico via USB (normal startup)

**Indicators:**
- LED blinks 3 times → Ready
- USB device appears as "MIDI Device"

**Function:**
- Processes MIDI messages
- Controls GPIO pins per configuration

### Config Mode (USB Mass Storage)

**How to enter (Method A - Hardware button):**
1. Disconnect Pico
2. Connect GPIO 22 to GND (or press CONFIG button if installed)
3. Connect USB while GPIO 22 is grounded
4. Pico appears as USB drive

**How to enter (Method B - MIDI SysEx):**
1. Send SysEx: `F0 7D 47 43 01 F7`
2. Pico LED blinks 10 times
3. Pico reboots as USB drive

**Indicators:**
- LED blinks slowly (1Hz)
- USB device appears as "Mass Storage Device"

**Function:**
- Edit or replace `config.json`
- View current configuration
- No MIDI processing

**Exit:**
- Disconnect USB
- Reconnect without button/SysEx

## Updating Configuration

### First-Time Setup (with PlatformIO)

```bash
pio run -t uploadfs
```

### Subsequent Updates (USB Mass Storage)

1. Enter config mode (button or SysEx)
2. Edit `config.json` on the USB drive
3. Disconnect and reconnect

**No recompilation needed!** ✨

## Testing Your Setup

### Test 1: Internal LED

Default config includes LED test:
```json
{
  "t": "n",
  "ch": 1,
  "n": 60,
  "p": 25,
  "a": "p",
  "d": 100
}
```

**Test:**
1. Connect Pico
2. Open MIDI software (DAW, MIDI-OX, etc.)
3. Send MIDI Note 60 on channel 1
4. Onboard LED should flash for 100ms

### Test 2: Serial Monitor

View debug output:
```bash
pio device monitor -b 115200
```

Send MIDI notes and observe output:
```
MIDI Note On: Ch=1 Note=60 Vel=100
Trigger Pin 25 (100ms)
MIDI Note Off: Ch=1 Note=60
```

### Test 3: Config Mode

1. Send SysEx `F0 7D 47 43 01 F7` or use button
2. LED should blink 10 times (SysEx) or 1Hz (button)
3. USB drive should appear
4. File `config.json` should be visible

## Troubleshooting

### Upload Fails: "No RP2040 in BOOTSEL mode"

**Solution:**
1. Hold BOOTSEL button on Pico
2. Connect USB (or press RESET while holding BOOTSEL)
3. Release BOOTSEL after 1 second
4. Retry upload

### No MIDI Device Appears

**Check:**
- Firmware uploaded successfully?
- Not in config mode (button not pressed)?
- USB cable supports data (not power-only)?
- Try different USB port

**Serial output should show:**
```
=== MIDI to GPIO Mapper ===
USB MIDI initialized
```

### Config Not Loading

**Serial output:**
```bash
pio device monitor -b 115200
```

**Check for:**
- `ERROR: /config.json not found!` → Run `pio run -t uploadfs`
- JSON parse errors → Validate JSON syntax
- Invalid field errors → Check required fields

### Filesystem Upload Fails

**Solution:**
```bash
# Clean and rebuild filesystem
pio run -t clean
pio run -t buildfs
pio run -t uploadfs
```

### Pin Not Working

**Check:**
- Pin number correct? (GPIO 0-40 for Pico)
- Pin not reserved? (GPIO 25 = LED, 0/1 = UART)
- External wiring correct?
- Power supply sufficient?

## Memory Usage

After installation, you should see:
```
RAM:   4-8% (varies with mappings)
Flash: 5-10% (plenty of room for expansion)
```

## Next Steps

1. ✅ Firmware installed
2. ✅ Config uploaded
3. ✅ MIDI working
4. → [Configure your mappings](CONFIGURATION_FORMAT.md)
5. → [Wire up your hardware](HARDWARE_SETUP.md)
6. → [Try example projects](../examples/)

## Advanced: Development Setup

### Enable Debug Mode

In `platformio.ini`:
```ini
build_flags = 
    -D USE_TINYUSB
    -D DEBUG_MODE        ; Add this line
```

Then rebuild and upload.

### Serial + MIDI Simultaneously

By default, USB is MIDI-only. For debugging during operation, use UART:
- Connect USB-Serial adapter to GPIO 0 (TX), GPIO 1 (RX)
- Monitor on separate serial port

### Custom Build Flags

```ini
build_flags = 
    -D USE_TINYUSB
    -D CONFIG_BUTTON_PIN=22
    -D MAX_MAPPINGS=100       ; Increase limit
    -D DEBUG_MODE
```

## Uninstallation

To restore Pico to factory state:
1. Download [flash_nuke.uf2](https://github.com/raspberrypi/pico-examples/raw/master/build/flash_nuke.uf2)
2. Enter BOOTSEL mode
3. Copy `flash_nuke.uf2` to RPI-RP2 drive
4. Pico will erase all flash memory

## Getting Help

- **Documentation**: Check [docs/](../docs/) folder
- **Examples**: See [examples/](../examples/) folder  
- **Issues**: Report on GitHub Issues
- **Community**: MIDI forums, Raspberry Pi forums
