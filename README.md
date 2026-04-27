# MIDI Solenoid Controller - Glockenspiel

Raspberry Pi Pico basierter MIDI-Controller für 19 Solenoids (elektromagnetische Aktoren) zur Ansteuerung über MOSFETs.

## Features

- ✅ **19 individuell ansteuerbare Solenoids**
- ✅ **Dual MIDI Input**: USB MIDI und UART MIDI (5-Pin DIN)
- ✅ **Velocity-Unterstützung** (optional mit PWM)
- ✅ **Konfigurierbare MIDI-Note-Zuordnung**
- ✅ **Automatische Impuls-Steuerung** (verhindert Überhitzung)
- ✅ **Debug-Ausgabe** über USB Serial
- ✅ **Test-Sequenz** für Hardware-Validierung

## Hardware-Anforderungen

### Komponenten

- **1x Raspberry Pi Pico** (oder Pico W)
- **19x N-Channel MOSFETs** (z.B. IRLZ44N, IRL540N oder ähnlich)
  - Logic-Level MOSFETs (aktivierbar mit 3.3V)
  - Min. 1A pro MOSFET (je nach Solenoid)
- **19x Solenoids** (z.B. 12V Push-Pull Solenoids)
- **19x Flyback-Dioden** (z.B. 1N4007 oder schneller: 1N5819 Schottky)
- **19x Gate-Widerstände** (220Ω - 1kΩ)
- **Externe Stromversorgung** für Solenoids (12V oder je nach Solenoid-Spezifikation)
- *Optional:* **MIDI-Eingangsschaltung** mit 6N138 Optokoppler für UART MIDI

### Pin-Belegung

#### Solenoid-Ausgänge (GPIO → MOSFET Gates)
```
Solenoid  0: GP2     Solenoid 10: GP12
Solenoid  1: GP3     Solenoid 11: GP13
Solenoid  2: GP4     Solenoid 12: GP14
Solenoid  3: GP5     Solenoid 13: GP15
Solenoid  4: GP6     Solenoid 14: GP16
Solenoid  5: GP7     Solenoid 15: GP17
Solenoid  6: GP8     Solenoid 16: GP18
Solenoid  7: GP9     Solenoid 17: GP19
Solenoid  8: GP10    Solenoid 18: GP20
Solenoid  9: GP11
```

#### MIDI-Eingänge
```
UART MIDI RX: GP1 (Serial1)
UART MIDI TX: GP0 (Serial1, optional)
USB MIDI:     Über USB-Verbindung
```

#### Status
```
LED (Onboard): GP25 (blinkt bei Betrieb)
Debug Serial:  USB-Verbindung
```

## Verkabelung

### MOSFET-Ansteuerung (pro Solenoid)

```
                     +12V (externe Versorgung)
                       |
                       |
                    [Solenoid]
                       |
                       +-------|<|---+ (Flyback-Diode)
                       |              |
                       D              |
                       |              |
Pico GPx ---[220Ω]--- G   MOSFET     |
                       |              |
                       S              |
                       |              |
                      GND ----------GND (Pico GND)
```

**Wichtig:**
1. **Gemeinsame Masse**: Pico GND MUSS mit der externen Versorgung GND verbunden sein
2. **Flyback-Diode**: Kathode (Streifen) an +12V, Anode an Solenoid/Drain
3. **Gate-Widerstand**: Schützt den GPIO-Pin
4. **Externe Versorgung**: NICHT über Pico versorgen! Separate 12V Versorgung verwenden

### UART MIDI Eingang (5-Pin DIN)

```
MIDI IN (5-Pin DIN):
Pin 4 ----[220Ω]---- +3.3V (Pico)
Pin 5 ----[220Ω]----+
                    |
                  [6N138]
                    |
                   GP1 (RX)
```

Für vollständige MIDI-Schaltung siehe: [MIDI Hardware Specification](https://www.midi.org/specifications)

## Software-Installation

### Voraussetzungen

1. **PlatformIO** installieren (VS Code Extension oder CLI)
2. **Git** (optional, zum Klonen)

### Installation

```bash
# In VS Code:
# 1. Projekt-Ordner öffnen
# 2. PlatformIO installiert automatisch alle Abhängigkeiten

# Oder per CLI:
cd Glockenspiel
pio lib install
```

### Kompilieren und Hochladen

```bash
# In VS Code:
# Klicke auf "Upload" in der PlatformIO-Leiste

# Oder per CLI:
pio run -t upload
```

**Wichtig beim Upload:**
- Halte den **BOOTSEL**-Button am Pico gedrückt
- Verbinde den Pico per USB
- Lasse den Button los → Pico erscheint als USB-Laufwerk
- PlatformIO lädt automatisch hoch

## Konfiguration

Alle Einstellungen befinden sich in [`src/config.h`](src/config.h):

### MIDI-Konfiguration

```cpp
#define MIDI_CHANNEL 10          // MIDI-Kanal (10 = Drums)
#define MIN_VELOCITY 20          // Minimale Velocity zum Auslösen
```

### Solenoid-Timing

```cpp
#define SOLENOID_PULSE_MS 30     // Impulsdauer in Millisekunden
```

**Wichtig:** Zu lange Impulse können Solenoids überhitzen! Start mit 30ms, anpassen nach Bedbedarf.

### MIDI-Note-Zuordnung

Standardmäßig chromatisch ab C4 (MIDI-Note 60):

```cpp
const uint8_t MIDI_NOTE_MAP[NUM_SOLENOIDS] = {
    60, 61, 62, 63, 64,  // C4 bis E4
    65, 66, 67, 68, 69,  // F4 bis A4
    70, 71, 72, 73, 74,  // Bb4 bis D5
    75, 76, 77, 78       // Eb5 bis F#5
};
```

**Anpassen für Glockenspiel-Tonleiter:**
Ändere die Werte entsprechend deiner Glockenspiel-Stimmung.

### Velocity-Mapping (optional)

Für dynamische Lautstärke per PWM:

```cpp
#define USE_VELOCITY_MAPPING true  // PWM aktivieren
#define PWM_FREQ 1000              // PWM-Frequenz
```

**Hinweis:** Viele Solenoids funktionieren besser mit einfachem On/Off (false).

## Verwendung

### 1. Verbindung herstellen

**USB MIDI:**
- Verbinde Pico per USB mit Computer
- Sollte automatisch als "Pico MIDI" erkannt werden

**UART MIDI:**
- Verbinde 5-Pin DIN MIDI-Kabel mit MIDI-Eingangsschaltung

### 2. MIDI senden

Von deiner DAW oder MIDI-Controller:
- **Kanal:** 10 (Standard, oder wie konfiguriert)
- **Noten:** 60-78 (C4 bis F#5, oder wie gemappt)
- **Velocity:** 20-127 (unter 20 wird ignoriert)

### 3. Debugging

Öffne Serial Monitor (115200 Baud):

```bash
# In VS Code: PlatformIO → Serial Monitor

# Oder per CLI:
pio device monitor
```

Du siehst:
- System-Informationen beim Start
- MIDI-Nachrichten
- Solenoid-Triggering
- Fehlerausgaben

### 4. Test-Sequenz

Aktiviere in [`src/main.cpp`](src/main.cpp):

```cpp
// In setup(), auskommentieren:
solenoids.testSequence();
```

Alle Solenoids werden nacheinander getestet (200ms Pause).

## Sicherheitshinweise

⚠️ **WICHTIG:**

1. **Überhitzung vermeiden:**
   - Solenoids nicht dauerhaft aktivieren
   - `SOLENOID_PULSE_MS` auf 20-50ms begrenzen
   - Bei Überhitzung: Impulsdauer reduzieren oder Pause zwischen Triggern erhöhen

2. **Stromversorgung:**
   - Solenoids NIEMALS über Pico versorgen
   - Separate 12V Versorgung mit ausreichend Ampere verwenden
   - Gemeinsame Masse zwischen Pico und Versorgung

3. **MOSFETs:**
   - Logic-Level MOSFETs verwenden (3.3V-kompatibel)
   - Flyback-Dioden NICHT vergessen
   - Bei Überhitzung: Kühlkörper verwenden

4. **Not-Aus:**
   - `solenoids.allOff()` deaktiviert sofort alle Solenoids

## Fehlerbehebung

### Solenoid triggert nicht

1. **Hardware prüfen:**
   - MOSFET richtig angeschlossen?
   - Flyback-Diode richtig gepolt?
   - Externe Versorgung aktiv?
   - Gemeinsame Masse verbunden?

2. **Software prüfen:**
   - Serial Monitor: Wird MIDI-Note empfangen?
   - Ist Note in `MIDI_NOTE_MAP` enthalten?
   - Velocity über `MIN_VELOCITY`?

### USB MIDI wird nicht erkannt

- Neustart des Pico (BOOTSEL + Reset)
- USB-Kabel prüfen (Daten-fähig?)
- Andere USB-Port probieren

### UART MIDI funktioniert nicht

- Optokoppler-Schaltung prüfen
- Pin-Konfiguration in `config.h` prüfen
- MIDI-Kabel richtig angeschlossen (nicht vertauscht)?

## Projekt-Struktur

```
Glockenspiel/
├── platformio.ini          # PlatformIO Konfiguration
├── src/
│   ├── main.cpp            # Hauptprogramm
│   ├── config.h            # Alle Einstellungen
│   ├── solenoid_driver.h   # Solenoid-Treiber Header
│   ├── solenoid_driver.cpp # Solenoid-Treiber Implementation
│   ├── midi_handler.h      # MIDI-Handler Header
│   └── midi_handler.cpp    # MIDI-Handler Implementation
└── README.md               # Diese Datei
```

## Lizenz

Open Source - nutze und modifiziere nach Belieben!

## Credits

Projekt für MIDI-gesteuertes Glockenspiel mit Raspberry Pi Pico.

---

**Viel Erfolg beim Bauen! 🎵🔧**
