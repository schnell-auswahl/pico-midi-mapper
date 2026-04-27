# Schaltplan und Verkabelungsdetails

## Komplette MOSFET-Schaltung

### Einzelner Solenoid-Kanal

```
Raspberry Pi Pico                     Externe Stromversorgung (12V)
                                               │
     GPx (z.B. GP2)                            │
       │                                       │
       │                                       │
      [R1]  220Ω-1kΩ Gate-Widerstand          │
       │                                       │
       │                                    ┌──┴──┐
       └───────── G                         │     │  Solenoid
                  │                         │  L  │  (z.B. 12V, 1A)
              ┌───┴───┐                     │     │
              │       │  N-Ch MOSFET        └──┬──┘
              │       │  (IRLZ44N)             │
           D ─┤       ├─ S                     │
              │       │                        │
              └───┬───┘                        ├────┐
                  │                            │    │
                  │                          ──▼──  │  D1 (Flyback)
                  │                          ──┬──  │  1N4007 / 1N5819
                  │                            │    │
                  └────────────────────────────┴────┤
                                                    │
                                                   GND ◄── Gemeinsam mit Pico GND!
```

### Komponenten-Details

#### MOSFET Auswahl
- **IRLZ44N**: 55V, 35A, Logic-Level (empfohlen)
- **IRL540N**: 100V, 28A, Logic-Level
- **IRF520**: Nicht geeignet (kein Logic-Level)

**Spezifikationen prüfen:**
- VGS(th) < 3V (Logic-Level!)
- ID(max) > Solenoid-Strom
- VDS(max) > Versorgungsspannung

#### Flyback-Diode (D1)
- **1N4007**: Langsam, aber robust (1A, 1000V)
- **1N5819**: Schnell, Schottky (1A, 40V) - **empfohlen**
- **SS34**: Auch gut (3A, 40V)

**Polarität beachten:**
- Kathode (Streifen) → +V Versorgung
- Anode → Drain/Solenoid

#### Gate-Widerstand (R1)
- **220Ω - 1kΩ**
- Schützt GPIO vor Stromspitzen
- Begrenzt Gate-Ladestrom

## UART MIDI-Eingangsschaltung

### Standard MIDI IN (mit Optokoppler)

```
MIDI IN (5-Pin DIN Buchse)                Raspberry Pi Pico
                                          
    Pin 2: Shield/Gehäuse ───────┐        
    Pin 4: +5V via 220Ω ──┐      │        
    Pin 5: Signal via 220Ω ┤     └──────── GND (optional)
                           │               
                          [R1]             
                          220Ω             
                           │               
                           ├────────┐      
                           │        │      
                          [R2]    ┌─┴─┐    
                          220Ω    │ 1 │ 6N138 Optokoppler
                           │      │   │    
                          ┌┴─┐    │ 2 │    
    Anode (Pin 5) ────────┤  ├────┤   │    
                          └┬─┘    └─┬─┘    
    Kathode (Pin 4) ──────┘│        │      
                            │      Pin 6 ───[R3 470Ω]─── +3.3V (Pico)
                           GND     Pin 5 ───────────────── GP1 (UART RX)
                                   Pin 8 ───────────────── +3.3V (Pico)
                                   Pin 7 ───────────────── GND (Pico)
```

### Komponenten MIDI-Eingang

- **Optokoppler**: 6N138 (schnell) oder 6N137
- **R1, R2**: 220Ω (MIDI-Standard)
- **R3**: 470Ω (Pull-up für Ausgang)
- **5-Pin DIN Buchse**: Weiblich, 180° oder 270°

### Pinout 6N138

```
     ┌──┴──┐
  1  │o    │  8  Vcc (+3.3V)
  2  │     │  7  GND
  3  │     │  6  Vout (zu Pico + R3 zu +3.3V)
  4  │     │  5  Output (zu Pico GP1)
     └─────┘
```

## Stromversorgung

### Externe 12V Versorgung für Solenoids

```
                    ┌─────────────┐
12V Netzteil ───────┤  +12V  GND  ├───┐
(z.B. 5A)           └──┬───────┬──┘   │
                       │       │      │
                       │       └──────┼────── GND (Pico)
                       │              │
                       └──────────────┼────── +12V zu allen Solenoids
                                      │
                                     ─┴─
```

**Stromberechnung:**
- 1 Solenoid @ 12V, 1A
- 19 Solenoids = theoretisch 19A
- **Aber:** Nicht alle gleichzeitig aktiv
- **Empfehlung:** Mindestens 5A Netzteil (für ~5 gleichzeitige Solenoids)

### Pico Stromversorgung

**Option 1: USB-Powered**
- Pico per USB verbunden
- USB MIDI funktioniert
- Einfachste Lösung

**Option 2: Externe 5V**
- VSYS Pin: 5V Eingang
- GND: Masse
- Für Standalone-Betrieb ohne PC

## Pin-Übersicht Raspberry Pi Pico

```
                    Raspberry Pi Pico
                    
         GP0  [ 1]        [40]  VBUS (5V USB)
         GP1  [ 2]        [39]  VSYS (5V extern)
         GND  [ 3]        [38]  GND
         GP2  [ 4]        [37]  3V3_EN
         GP3  [ 5]        [36]  3V3(OUT)
         GP4  [ 6]        [35]  ADC_VREF
         GP5  [ 7]        [34]  GP28
         GND  [ 8]        [33]  GND
         GP6  [ 9]        [32]  GP27
         GP7  [10]        [31]  GP26
         GP8  [11]        [30]  RUN
         GP9  [12]        [29]  GP22
         GND  [13]        [28]  GND
        GP10  [14]        [27]  GP21
        GP11  [15]        [26]  GP20  ← Solenoid 18
        GP12  [16]        [25]  GP19  ← Solenoid 17
        GP13  [17]        [24]  GP18  ← Solenoid 16
         GND  [18]        [23]  GND
        GP14  [19]        [22]  GP17  ← Solenoid 15
        GP15  [20]        [21]  GP16  ← Solenoid 14
        
        GP0  = UART TX (MIDI OUT, optional)
        GP1  = UART RX (MIDI IN)
        GP2-GP20 = Solenoid 0-18
```

## Lötplan für 19-Kanal-Board

### Empfohlenes Layout

**Platine mit:**
- 19x MOSFET-Positionen (TO-220 Package)
- 19x Flyback-Dioden
- 19x Gate-Widerstände
- Schraubklemmen für Solenoid-Anschlüsse
- Pin-Header für Pico
- Schraubklemmen für 12V Eingang

### Alternativen

1. **Perfboard/Lochraster:**
   - Eine Reihe MOSFETs
   - Darüber die Dioden
   - Ordentlich verdrahten

2. **PCB Design:**
   - KiCad oder EasyEDA verwenden
   - Fertigung bei JLCPCB/PCBWay

3. **Breakout-Boards:**
   - Fertige MOSFET-Module verwenden
   - Z.B. "IRF520 MOSFET Module"
   - Teurer, aber einfacher

## Sicherheits-Checkliste vor dem ersten Test

- [ ] Alle Masse-Verbindungen geprüft (Pico GND = Netzteil GND)
- [ ] Flyback-Dioden richtig gepolt (Stripe = Kathode zu +12V)
- [ ] MOSFETs richtig eingebaut (D-G-S Pins nicht vertauscht)
- [ ] Gate-Widerstände vorhanden
- [ ] Externe Versorgung getestet (12V, Polarität!)
- [ ] Pico NICHT an 12V angeschlossen
- [ ] Solenoids können sich frei bewegen
- [ ] Erste Tests nur mit 1-2 Solenoids
- [ ] Serial Monitor bereit für Debugging

## Werkzeuge und Materialien

### Benötigt
- Lötkolben + Lötzinn
- Abisolierzange
- Seitenschneider
- Multimeter
- Schrumpfschlauch
- Litze (verschiedene Farben)

### Empfohlen
- Dritte Hand / Löthilfe
- Entlötlitze
- Flussmittel
- Kabelschuhe / Aderendhülsen
- Beschriftung / Etiketten

## Erweiterungen

### PWM für variable Stärke
Aktiviere in `config.h`:
```cpp
#define USE_VELOCITY_MAPPING true
```

### Mehr Solenoids (>19)
- Port-Expander verwenden (z.B. MCP23017 via I2C)
- Oder zweiten Pico nutzen

### MIDI OUT
- GP0 bereits für TX vorbereitet
- MIDI OUT Schaltung ähnlich wie IN
- MIDI Thru möglich

---

**Bei Fragen: Siehe README.md oder prüfe Serial Monitor!**
