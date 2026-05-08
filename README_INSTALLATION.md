# Glockenspiel MIDI Controller - Installation

## Schnellstart

### 1. Kompilieren & Uploaden

```bash
# Programm kompilieren und auf Pico laden
pio run --target upload

# Config-Dateien auf Pico laden (NUR EINMAL!)
pio run --target uploadfs
```

**Wichtig:** `uploadfs` lädt die `data/config.json` ins LittleFS-Dateisystem!

### 2. Testen

**Test 1: Interne LED**
- MIDI Note 60 (Middle C) senden
- LED sollte blinken (100ms Puls)

**Test 2: Config Mode aktivieren**
- SysEx senden: `F0 7D 47 43 01 F7`
- LED blinkt 10x schnell
- Pico startet als USB-Laufwerk neu

### 3. Config bearbeiten

**Variante A: Button (Hardware)**
1. Pico trennen
2. GP22 mit GND verbinden (oder Button drücken)
3. USB anschließen
4. Pico erscheint als Laufwerk "RPI-RP2"
5. `config.json` bearbeiten
6. Pico trennen, ohne Button neu starten

**Variante B: MIDI-Befehl (Software)**
1. SysEx `F0 7D 47 43 01 F7` senden
2. Pico startet als USB-Laufwerk
3. `config.json` bearbeiten
4. Pico trennen & neu verbinden

---

## Projekt-Struktur

```
Glockenspiel/
├── src/
│   ├── main.cpp          ← Neue Haupt-Implementierung
│   └── test_midi.cpp     ← Alter Test-Code (behalten)
├── data/
│   └── config.json       ← MIDI Mappings (wird auf Pico geflasht)
├── platformio.ini        ← Projekt-Konfiguration
├── README_MIDI_CONTROL.md ← MIDI API Dokumentation
└── USB_MIDI_LOESUNG.md   ← Problemlösung von vorher
```

---

## config.json Beispiel

```json
{
  "mappings": [
    {
      "comment": "Interne LED - Note C4",
      "type": "note_on",
      "channel": 1,
      "note": 60,
      "pin": 25,
      "action": "pulse",
      "duration": 100
    }
  ]
}
```

**Pin 25 = Interne LED** (perfekt zum Testen ohne Hardware!)

---

## Hardware-Anforderungen

### Minimal (Nur Testen)
- Raspberry Pi Pico
- USB-Kabel
- Fertig!

### Für Config-Button
- Raspberry Pi Pico
- Taster/Button zwischen GP22 und GND
- USB-Kabel

### Für 2 Picos (MIDI THRU)
```
Pico 1 GP0 (TX) → Pico 2 GP1 (RX)
Pico 1 GND      → Pico 2 GND
Pico 1 VBUS     → Pico 2 VSYS (optional für Stromversorgung)
```

---

## Workflows

### Entwicklung

```bash
# Code ändern → Upload
pio run --target upload

# Config ändern → FS Upload
pio run --target uploadfs

# Serial Monitor
pio device monitor -b 115200
```

### Production

1. **Einmal flashen:**
   ```bash
   pio run --target upload
   pio run --target uploadfs
   ```

2. **Config ändern (ohne Terminal):**
   - SysEx senden via Software
   - Oder Button drücken beim Start
   - config.json im Finder bearbeiten
   - Neu starten

---

## Modi

### MIDI Mode (Normal)
- **Aktivierung:** Normaler Start ohne Button
- **LED:** 3x Blinken → Bereit
- **Funktion:** MIDI-Nachrichten verarbeiten
- **USB:** MIDI Device

### Config Mode (USB-Laufwerk)
- **Aktivierung:** 
  - Button (GP22) beim Start gedrückt
  - Oder SysEx `F0 7D 47 43 01 F7` senden
- **LED:** 1x/Sekunde langsam blinken
- **Funktion:** Config bearbeiten
- **USB:** Mass Storage Device (Laufwerk)

---

## Speicher-Aufteilung

```
2MB Flash:
├── ~80KB   Programm-Code
├── 1MB     LittleFS (config.json)
└── ~900KB  Frei
```

---

## Troubleshooting

### Upload funktioniert nicht

**Problem:** `No accessible RP2040 devices in BOOTSEL mode`

**Lösung:**
1. BOOTSEL-Taste am Pico drücken
2. USB einstecken (oder Reset drücken)
3. Pico erscheint als Laufwerk
4. Upload wiederholen

### Config wird nicht geladen

**Serial Monitor Ausgabe prüfen:**
```bash
pio device monitor -b 115200
```

Erwartete Ausgabe:
```
=== Config laden ===
Mapping 0: note_on Note/CC=60 Pin=25 Action=0
=== 6 Mappings geladen ===
```

Falls "FEHLER: /config.json nicht gefunden!":
```bash
pio run --target uploadfs
```

### USB-Laufwerk erscheint nicht

**Prüfen:**
- Ist Button korrekt angeschlossen? (GP22 + GND)
- LED blinkt langsam? (= Config Mode aktiv)
- Serial Monitor zeigt "USB MASS STORAGE MODE"?

---

## Next Steps

1. **Test:** LED mit Note 60 blinken lassen
2. **Config:** Weitere Mappings in config.json hinzufügen
3. **Hardware:** MOSFETs für Solenoids anschließen
4. **Software:** Web-Interface bauen (siehe README_MIDI_CONTROL.md)
