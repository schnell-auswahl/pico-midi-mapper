# USB Mass Storage Mode

## Overview

The Pico MIDI Mapper implements a clever dual-mode USB configuration:
- **MIDI Mode** (default): Acts as a USB MIDI device
- **Config Mode**: Acts as a USB mass storage device for easy configuration updates

This allows you to update your MIDI mappings **without recompiling or re-flashing firmware**!

---

## How It Works

### Architecture

```
┌─────────────────────┐
│  Pico Filesystem    │
│   (LittleFS)        │
│                     │
│  ┌──────────────┐   │
│  │ config.json  │   │
│  └──────────────┘   │
└──────────┬──────────┘
           │
    ┌──────┴───────┐
    │              │
┌───▼───┐    ┌────▼─────┐
│ MIDI  │    │   USB    │
│ Mode  │    │  Storage │
│       │    │   Mode   │
└───────┘    └──────────┘
```

**MIDI Mode:**
- Reads `config.json` from LittleFS filesystem
- Processes MIDI messages
- Controls GPIO pins

**Config Mode:**
- Creates a FAT12 RAM disk
- Copies `config.json` to RAM disk
- Exposes RAM disk via USB mass storage
- Writes changes back to LittleFS on disconnect

---

## Entering Config Mode

### Method 1: Hardware Button (Recommended)

1. **Disconnect** the Pico from USB
2. **Connect GPIO 22 to GND** (or press CONFIG button if installed)
3. **Connect USB** while GPIO 22 is grounded
4. **LED indicator**: Slow blinking (1 Hz)
5. Pico appears as **"PICO FS"** USB drive
6. **Release button** (or disconnect GPIO 22)

**Button Circuit:**
```
Pico GP22 ──┤ ├── GND
         Button (NO)
```

---

### Method 2: MIDI SysEx Command

**Send this SysEx message:**
```
F0 7D 47 43 01 F7
```

**Breakdown:**
- `F0` = SysEx start
- `7D` = Non-commercial manufacturer ID
- `47 43` = "GC" (GPIO Controller)
- `01` = Command: Enter Config Mode
- `F7` = SysEx end

**LED indicator:** 
- Blinks rapidly 10 times
- Then reboots into config mode
- Slow blinking (1 Hz) once ready

**How to send:**
- DAW (Ableton, Logic, etc.) - MIDI track with SysEx
- MIDI utility software (MIDI-OX, SendMIDI)
- Web MIDI API (JavaScript)

**Example (Web MIDI API):**
```javascript
const sysex = [0xF0, 0x7D, 0x47, 0x43, 0x01, 0xF7];
midiOutput.send(sysex);
```

---

## Using Config Mode

### 1. Edit Configuration

Once in config mode, the Pico appears as a USB drive:

**macOS:**
```
/Volumes/PICO FS/config.json
```

**Windows:**
```
E:\config.json  (drive letter may vary)
```

**Linux:**
```
/media/user/PICO FS/config.json
```

**Edit with:**
- Any text editor (VS Code, Notepad++, TextEdit, etc.)
- Or replace with new file

### 2. Validate JSON

**Before disconnecting**, validate your JSON:
- Use [jsonlint.com](https://jsonlint.com/)
- Or VS Code JSON validation
- Or `jq` command-line tool

**Common errors:**
- Missing commas
- Trailing commas (not allowed in JSON)
- Mismatched brackets
- Unquoted strings

### 3. Safely Eject

**Important:** Always eject/unmount the drive before disconnecting!

**macOS:**
```bash
diskutil eject /Volumes/PICO\ FS
```

**Windows:**
- Right-click → "Eject"

**Linux:**
```bash
umount /media/user/PICO\ FS
```

### 4. Return to MIDI Mode

1. **Disconnect USB**
2. **Reconnect USB** (without button pressed)
3. LED blinks 3 times → Ready
4. Pico appears as MIDI device
5. New configuration is active

---

## Technical Implementation

### Why This Approach?

**Problem with board menu options:**
```ini
# ❌ Does NOT work in PlatformIO
board_build.arduino.earlephilhower.usb_type = midi
```

PlatformIO ignores Arduino framework board menu options.

**Solution: Adafruit TinyUSB Library**

```ini
# ✅ Works!
lib_deps = 
    adafruit/Adafruit TinyUSB Library @ ^3.7.5
    fortyseveneffects/MIDI Library @ ^5.0.2

build_flags = 
    -D USE_TINYUSB
```

This gives us full control over USB descriptors and allows dynamic switching.

---

### RAM Disk Implementation

**FAT12 Filesystem:**
- Size: 64 KB (128 blocks × 512 bytes)
- Format: FAT12 (simplest FAT variant)
- Contents: Single file `config.json`

**Why RAM disk?**
- LittleFS cannot be directly exposed via USB mass storage
- FAT12 is universally compatible
- Small enough to fit in Pico's RAM
- Fast read/write

**Memory Layout:**
```
Block 0:       Boot Sector
Block 1-2:     FAT Tables (2 copies)
Block 3:       Root Directory
Block 4+:      File data (config.json)
```

---

### State Machine

```
Power On
   │
   ▼
  [Check GPIO 22]
   │         │
   ├─ LOW ───┴─→ [Config Mode]
   │                │
   │                ▼
   │            USB Mass Storage
   │                │
   │                ▼
   │         [User edits config]
   │                │
   │                ▼
   │         [On disconnect: Save to LittleFS]
   │
   └─ HIGH ───→ [MIDI Mode]
                   │
                   ▼
              USB MIDI Device
                   │
                   ▼
          [Load config from LittleFS]
                   │
                   ▼
           [Process MIDI messages]
                   │
                   ◄──[Listen for SysEx]
                   │
                   └─ SysEx 01 ─→ [Reboot to Config Mode]
```

---

## Troubleshooting

### Drive Doesn't Appear

**Check:**
- Is LED blinking slowly (1 Hz)? If not, not in config mode
- Try different USB port
- Try different USB cable (data-capable)
- Check if GPIO 22 connected to GND during boot

**Serial Monitor:**
```bash
pio device monitor -b 115200
```

Should show:
```
=== Config Mode ===
USB Mass Storage active
```

---

### File Not Saving

**Possible causes:**
1. **Didn't eject properly** → Always eject before disconnect
2. **JSON syntax error** → Validate before saving
3. **Filesystem full** → Config too large (>60 KB)

**Check LittleFS:**
```cpp
// Enable DEBUG_MODE to see filesystem operations
#define DEBUG_MODE
```

Serial output will show:
```
Config saved to LittleFS: 1234 bytes
```

---

### Config Not Loading After Edit

**Symptoms:**
- Old configuration still active
- Serial shows "ERROR: JSON parse failed"

**Solutions:**
1. **Check JSON syntax** (most common)
2. **Verify file saved** (check file size, should not be 0)
3. **Re-upload via PlatformIO** if corrupted:
   ```bash
   pio run -t uploadfs
   ```

---

### Cannot Return to MIDI Mode

**If stuck in config mode:**
1. Disconnect USB
2. **Ensure GPIO 22 is NOT grounded**
3. Reconnect USB
4. Should boot to MIDI mode (LED blinks 3×)

**If still stuck:**
- Re-flash firmware via BOOTSEL mode
- Upload filesystem: `pio run -t uploadfs`

---

## Web-Based Workflow

### Recommended: Use the Web Editor

1. **Create config** at [elektrologue.com/midi-mapper-editor.html](https://elektrologue.com/midi-mapper-editor.html)
2. **Export** as `config.json`
3. **Enter config mode** (button or SysEx)
4. **Drag file** to PICO FS drive
5. **Eject and reconnect**

**Advantages:**
- Visual interface
- Real-time validation
- No syntax errors
- Field documentation built-in

---

## Advanced: SysEx Commands

### Current Commands

| Command | SysEx Bytes | Function |
|---------|-------------|----------|
| Enter Config | `F0 7D 47 43 01 F7` | Reboot to USB mass storage |

### Future Extensions

**Possible additions:**
- `02` - Factory reset
- `03` - Get version info
- `04` - Toggle debug mode
- `10-1F` - Direct mapping control

(Requires firmware modification)

---

## Source Code Reference

**Key files:**
- `src/main.cpp` - Mode detection and state machine
- `msc_read_cb()` - USB mass storage read
- `msc_write_cb()` - USB mass storage write
- `msc_flush_cb()` - Save back to LittleFS
- `createFAT12Image()` - Generate RAM disk

**Customization:**
```cpp
#define CONFIG_BUTTON_PIN 22    // Change button pin
#define DISK_BLOCK_NUM 128      // RAM disk size (×512 bytes)
```

---

## Best Practices

### 1. Version Control Your Configs

Keep backup copies:
```
configs/
  ├── default.json
  ├── solenoid-v1.json
  ├── led-matrix.json
  └── relay-board.json
```

### 2. Test Before Deploying

1. Load new config in config mode
2. Check serial monitor for errors
3. Test with simple MIDI notes
4. Verify all mappings work
5. Save working config as backup

### 3. Documentation

Add comments (as separate file, not in JSON):
```
config.json     - Active configuration
README.txt      - Mapping documentation
```

---

## Memory Considerations

**RAM usage:**
- Config mode: ~66 KB (FAT12 disk)
- MIDI mode: Variable (depends on mapping count)

**Flash usage:**
- Firmware: ~80 KB
- LittleFS partition: 1 MB (configurable in `platformio.ini`)

**Config size limit:**
- Practical: ~60 KB (leaves room for overhead)
- Maximum: 50 mappings × ~100 bytes = ~5 KB typical

---

## Related Documentation

- [Configuration Format →](CONFIGURATION_FORMAT.md)
- [Installation Guide →](INSTALLATION.md)
- [MIDI Features →](MIDI_FEATURES.md)
