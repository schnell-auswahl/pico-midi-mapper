# MIDI Control - Web API Zugriff

Dieses Dokument erklärt, wie du den Glockenspiel MIDI Controller über die Web MIDI API (Browser) steuern kannst.

## Voraussetzungen

- **Browser:** Chrome, Edge oder Opera (Safari unterstützt Web MIDI nicht)
- **Pico angeschlossen** und im MIDI-Modus

## SysEx-Befehle

### Config-Modus aktivieren

**Befehl:** `F0 7D 47 43 01 F7`

**Erklärung:**
- `F0` = SysEx Start
- `7D` = Non-commercial Manufacturer ID  
- `47 43` = "GC" (Glockenspiel Config)
- `01` = Befehl: "Enter Config Mode"
- `F7` = SysEx End

**Reaktion:**
- LED blinkt 10x schnell
- Pico startet neu als USB-Laufwerk
- `config.json` kann bearbeitet werden

---

## Web MIDI API - JavaScript Beispiele

### 1. Grundlegendes Setup

```html
<!DOCTYPE html>
<html>
<head>
    <title>Glockenspiel Controller</title>
</head>
<body>
    <h1>Glockenspiel MIDI Controller</h1>
    
    <button id="connect">MIDI Verbinden</button>
    <button id="configMode">Config Mode aktivieren</button>
    <button id="testNote">Test Note (C4)</button>
    
    <div id="status">Nicht verbunden</div>
    
    <script src="controller.js"></script>
</body>
</html>
```

### 2. JavaScript Controller

```javascript
// controller.js

let midiOutput = null;
let midiInput = null;

// MIDI Access anfordern
async function connectMIDI() {
    try {
        const access = await navigator.requestMIDIAccess({ sysex: true });
        
        // Outputs (zum Senden)
        for (let output of access.outputs.values()) {
            if (output.name.includes('Glockenspiel') || 
                output.name.includes('Pico')) {
                midiOutput = output;
                console.log('Output gefunden:', output.name);
            }
        }
        
        // Inputs (zum Empfangen)
        for (let input of access.inputs.values()) {
            if (input.name.includes('Glockenspiel') || 
                input.name.includes('Pico')) {
                midiInput = input;
                midiInput.onmidimessage = handleMIDIMessage;
                console.log('Input gefunden:', input.name);
            }
        }
        
        if (midiOutput) {
            document.getElementById('status').textContent = 
                `Verbunden: ${midiOutput.name}`;
            return true;
        } else {
            alert('Kein Glockenspiel gefunden! Ist der Pico angeschlossen?');
            return false;
        }
    } catch (err) {
        console.error('MIDI Fehler:', err);
        alert('Web MIDI nicht verfügbar. Nutze Chrome/Edge!');
        return false;
    }
}

// MIDI Nachrichten empfangen
function handleMIDIMessage(event) {
    const data = Array.from(event.data);
    console.log('MIDI empfangen:', data);
}

// Config Mode aktivieren
function enterConfigMode() {
    if (!midiOutput) {
        alert('Erst verbinden!');
        return;
    }
    
    // SysEx: F0 7D 47 43 01 F7
    const sysex = [0xF0, 0x7D, 0x47, 0x43, 0x01, 0xF7];
    midiOutput.send(sysex);
    
    console.log('Config Mode aktiviert!');
    alert('Pico blinkt 10x und startet als USB-Laufwerk neu.');
}

// Test-Note senden
function sendTestNote() {
    if (!midiOutput) {
        alert('Erst verbinden!');
        return;
    }
    
    // Note On: C4 (Note 60), Velocity 100, Channel 1
    midiOutput.send([0x90, 60, 100]);
    
    // Note Off nach 200ms
    setTimeout(() => {
        midiOutput.send([0x80, 60, 0]);
    }, 200);
}

// Event Listener
document.getElementById('connect').addEventListener('click', connectMIDI);
document.getElementById('configMode').addEventListener('click', enterConfigMode);
document.getElementById('testNote').addEventListener('click', sendTestNote);
```

---

## Erweiterte Beispiele

### Note senden (LED/Solenoid triggern)

```javascript
function playNote(note, velocity = 100, channel = 1) {
    if (!midiOutput) return;
    
    const noteOn = 0x90 + (channel - 1);  // 0x90 = Note On Channel 1
    const noteOff = 0x80 + (channel - 1); // 0x80 = Note Off Channel 1
    
    // Note On
    midiOutput.send([noteOn, note, velocity]);
    
    // Note Off nach 100ms
    setTimeout(() => {
        midiOutput.send([noteOff, note, 0]);
    }, 100);
}

// Beispiel: Interne LED (Note 60 laut config.json)
playNote(60);

// Solenoid 1 (Note 61)
playNote(61);
```

### Control Change senden (PWM)

```javascript
function sendCC(cc, value, channel = 1) {
    if (!midiOutput) return;
    
    const controlChange = 0xB0 + (channel - 1);
    midiOutput.send([controlChange, cc, value]);
}

// Beispiel: CC 7 auf 50% (PWM auf Pin 10)
sendCC(7, 64);  // 64 von 127 ≈ 50%
```

### Melodie abspielen

```javascript
async function playMelody() {
    const melody = [
        { note: 60, duration: 200 },  // C
        { note: 62, duration: 200 },  // D
        { note: 64, duration: 200 },  // E
        { note: 60, duration: 400 }   // C (lang)
    ];
    
    for (let m of melody) {
        midiOutput.send([0x90, m.note, 100]); // Note On
        await sleep(m.duration);
        midiOutput.send([0x80, m.note, 0]);   // Note Off
        await sleep(50); // Pause
    }
}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}
```

---

## Python Beispiel

Falls du mit Python arbeiten möchtest:

```python
import mido
import time

# Pico finden
ports = mido.get_output_names()
pico_port = None

for port in ports:
    if 'Glockenspiel' in port or 'Pico' in port:
        pico_port = port
        break

if not pico_port:
    print("Pico nicht gefunden!")
    exit()

# Verbinden
output = mido.open_output(pico_port)

# Config Mode aktivieren
config_sysex = mido.Message('sysex', data=[0x7D, 0x47, 0x43, 0x01])
output.send(config_sysex)
print("Config Mode aktiviert!")

# Note senden
note_on = mido.Message('note_on', note=60, velocity=100)
output.send(note_on)
time.sleep(0.2)
note_off = mido.Message('note_off', note=60)
output.send(note_off)

output.close()
```

---

## Config.json Struktur

```json
{
  "mappings": [
    {
      "comment": "Beschreibung",
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

**Felder:**
- `type`: `"note_on"`, `"note_off"`, `"control_change"`
- `channel`: 1-16 (0 = alle Kanäle)
- `note` / `cc`: MIDI Note oder CC Nummer (0-127)
- `pin`: GPIO Pin (0-28, Pin 25 = interne LED)
- `action`: 
  - `"pulse"` = Kurzer Impuls (duration in ms)
  - `"toggle"` = Umschalten bei jedem Note On
  - `"gate"` = An bei Note On, Aus bei Note Off
  - `"pwm"` = PWM gesteuert durch Velocity/CC-Wert
- `duration`: Nur bei `action: "pulse"` - Dauer in ms

---

## Workflow: Config ändern

1. **Config Mode aktivieren:**
   ```javascript
   const sysex = [0xF0, 0x7D, 0x47, 0x43, 0x01, 0xF7];
   midiOutput.send(sysex);
   ```

2. **Pico startet neu** → erscheint als USB-Laufwerk

3. **config.json bearbeiten** im Finder

4. **Pico trennen** und neu verbinden

5. **Neue Config wird geladen**

---

## Debugging

### Serial Monitor nutzen

```bash
pio device monitor -b 115200
```

**Ausgabe:**
```
=== Config laden ===
Mapping 0: note_on Note/CC=60 Pin=25 Action=0
Mapping 1: note_on Note/CC=61 Pin=2 Action=0
=== 6 Mappings geladen ===
=== BEREIT ===
```

### LED-Signale

- **3x Blinken:** Bereit (MIDI Mode)
- **10x schnelles Blinken:** Config Mode aktiviert (Neustart)
- **1x/Sekunde langsam:** Config Mode aktiv (USB-Laufwerk)

---

## Troubleshooting

**Problem:** Web MIDI findet Pico nicht  
**Lösung:** 
- Chrome/Edge nutzen (nicht Safari!)
- HTTPS oder localhost nutzen
- Pico trennen & neu verbinden

**Problem:** Config Mode funktioniert nicht  
**Lösung:**
- SysEx korrekt? `[0xF0, 0x7D, 0x47, 0x43, 0x01, 0xF7]`
- Browser-Console prüfen (F12)
- Serial Monitor zeigt "SysEx: Config Mode aktiviert!"?

**Problem:** Mappings funktionieren nicht  
**Lösung:**
- Serial Monitor: Sind Mappings geladen?
- JSON-Syntax korrekt? (Online-Validator nutzen)
- Pin-Nummern korrekt? (Pin 25 = LED)

---

## Support

Bei Problemen:
1. Serial Monitor starten: `pio device monitor`
2. Debug-Output lesen
3. JSON in Validator prüfen: https://jsonlint.com
