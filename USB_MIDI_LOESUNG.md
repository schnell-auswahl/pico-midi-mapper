# USB MIDI Konfiguration - Lösung

## Problem

Der Raspberry Pi Pico hat sich als **Serial CDC Gerät** (`/dev/cu.usbmodem1101`) angemeldet, statt als **MIDI Gerät**. Dadurch war keine MIDI Kommunikation über USB möglich.

## Ursache

Die arduino-pico Framework-Optionen (wie `board_build.arduino.earlephilhower.usb_type = midi`) funktionieren **nicht** in PlatformIO. PlatformIO nutzt diese Board-Menü-Optionen nicht.

### Was NICHT funktioniert hat:

```ini
# ❌ FALSCH - Board Menu Optionen werden von PlatformIO ignoriert
board_build.arduino.earlephilhower.usb_type = midi

# ❌ FALSCH - Build Flags ohne Library
build_flags = 
    -DUSB_MIDI
    -DUSE_TINYUSB
```

## Lösung

Die Lösung verwendet die **externe Adafruit TinyUSB Library** statt der Framework-eigenen USB-Implementierung.

### Funktionierende Konfiguration

**platformio.ini:**
```ini
[env:pico]
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = pico
board_build.core = earlephilhower
framework = arduino
upload_protocol = picotool
monitor_speed = 115200

; WICHTIG: Externe Bibliotheken verwenden!
lib_deps = 
    adafruit/Adafruit TinyUSB Library @ ^3.7.5
    fortyseveneffects/MIDI Library @ ^5.0.2

build_flags = 
    -D USE_TINYUSB
    -std=gnu++17
```

**Code (test_midi.cpp):**
```cpp
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

// USB MIDI Objekte erstellen
Adafruit_USBD_MIDI usbMidi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMidi, MIDI);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    
    // USB Gerät konfigurieren
    TinyUSBDevice.setManufacturerDescriptor("Glockenspiel");
    TinyUSBDevice.setProductDescriptor("Pico MIDI Solenoid");
    
    // USB MIDI starten
    usbMidi.begin();
    MIDI.begin();
}

void loop() {
    MIDI.read();  // Empfangen
    
    // MIDI senden
    MIDI.sendNoteOn(60, 100, 1);
    delay(500);
    MIDI.sendNoteOff(60, 0, 1);
    delay(500);
}
```

## Warum funktioniert das?

1. **Adafruit TinyUSB Library** übernimmt die komplette USB-Initialisierung
2. Sie konfiguriert den Pico automatisch als **USB MIDI Device**
3. Das Framework wird durch `USE_TINYUSB` angewiesen, die externe Library zu verwenden
4. Die MIDI Library nutzt dann `Adafruit_USBD_MIDI` als Transport

## Quelle & Template

Lösung basiert auf diesem funktionierenden Template:
**https://github.com/madskjeldgaard/raspberry-pi-pico-usbmidi-platformio-template**

Dieses Template verwendet exakt denselben Ansatz und wurde erfolgreich getestet.

## Speichernutzung

- **RAM:** 4.7% (12.208 von 262.144 Bytes)
- **Flash:** 3.8% (79.096 von 2.093.056 Bytes)

Genug Platz für die komplette Solenoid-Steuerung mit 19 Kanälen!

## Wichtige Hinweise

### Upload-Problem nach MIDI-Aktivierung

Nach dem ersten erfolgreichen Upload als MIDI Device kann der Pico **nicht mehr automatisch** in den Upload-Modus wechseln. Du siehst dann:

```
No accessible RP2040 devices in BOOTSEL mode were found.
*** [upload] Error 249
```

**Lösung:** BOOTSEL-Button beim Upload drücken:

1. **BOOTSEL-Taste** am Pico gedrückt halten
2. **USB-Kabel** einstecken (oder Reset drücken)
3. Pico erscheint als Massenspeicher
4. `pio run -t upload` ausführen
5. BOOTSEL-Taste loslassen

### Alternative: Temporär Serial aktivieren

Für einfacheres Entwickeln kannst du temporär Serial CDC nutzen:

```cpp
// In setup() VOR usbMidi.begin():
#ifdef DEBUG_MODE
  Serial.begin(115200);
  while (!Serial) delay(10);
#endif
```

Dann in platformio.ini:
```ini
build_flags = 
    -D USE_TINYUSB
    -D DEBUG_MODE  ; Aktiviert Serial neben MIDI
```

## Nächste Schritte

Die vollständige Solenoid-Implementierung liegt bereit in:
- `src/main.cpp.backup`
- `src/config.h.backup`
- `src/solenoid_driver.h/.cpp.backup`
- `src/midi_handler.h/.cpp.backup`

Diese nutzen dieselbe MIDI-Konfiguration und können jetzt integriert werden!
