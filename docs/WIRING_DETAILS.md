# Detailed Wiring Reference

## Complete MOSFET Driver Circuit

### Single Channel (Basic)

```
Raspberry Pi Pico                     External Power Supply (+12V)
                                               │
     GPx (e.g., GP2)                          │
       │                                      │
       │                                   ┌──┴──┐
      [R1]  470Ω Gate resistor            │     │  Load
       │                                   │  L  │  (Solenoid, Motor, etc.)
       │                                   │     │
       └──────────── G                     └──┬──┘
                     │                        │
                 ┌───┴───┐                    │
                 │       │  N-Ch MOSFET       ├──────┐
                 │       │  (IRLZ44N)         │      │
              D ─┤       ├─ S              ──▼──    │  D1 (Flyback)
                 │       │                 ──┬──    │  1N5819 Schottky
                 └───┬───┘                   │      │
                     │                       │      │
                     └───────────────────────┴──────┤
                                                    │
                                                   GND ◄── Common with Pico GND
```

**Component Values:**
- **R1 (Gate resistor):** 220Ω - 1kΩ (470Ω recommended)
- **D1 (Flyback diode):** 1N5819 (fast, preferred) or 1N4007 (slower but OK)
- **MOSFET:** IRLZ44N, IRL540N, or equivalent logic-level

---

## UART MIDI Input Circuit

### Standard MIDI Input (Optocoupler)

```
MIDI IN Jack (5-Pin DIN Female)           Raspberry Pi Pico
                                          
    Pin 1: Not connected                  
    Pin 2: Shield ─────────┐               
    Pin 3: Not connected   │               
    Pin 4: +Current ───┐   │               
    Pin 5: -Current ───┼───┘── GND (optional, shield)
                       │                    
                      [R1]                  
                      220Ω                  
                       │                    
                       ├───────────┐        
                       │           │        
                      [R2]       ┌─┴─┐      
                      220Ω       │ 1 │ Anode    6N138 or 6N137
                       │         │   │ Cathode  Optocoupler
                      ┌┴─┐       │ 2 │          
    Pin 5 ────────────┤  ├───────┤   │          
    (Current sink)    └┬─┘       └─┬─┘          
                       │            │            
    Pin 4 ─────────────┘          Pin 8 ───[R3 470Ω]─── +3.3V (Pico)
    (Current source)              Pin 6 ───[R3 470Ω]─── +3.3V (Pico)
                                  Pin 5 ───────────────── GP1 (UART RX)
                                  Pin 7 ───────────────── GND (Pico)
                                  Pin 3,4: Not connected
```

**Component List:**
- **Optocoupler:** 6N138 (recommended, fast) or 6N137
- **R1, R2:** 220Ω ±5% (MIDI standard)
- **R3:** 470Ω ±5% (pull-up resistor)
- **DIN connector:** 5-pin female, PCB mount or panel mount

### 6N138 Pinout

```
     ┌──┴──┐
  1  │●    │  8  Vcc (+3.3V)
  2  │     │  7  GND
  3  │ NC  │  6  Output (to +3.3V via R3)
  4  │ NC  │  5  Output (to GPIO 1)
     └─────┘
```

### Optional: MIDI THRU

To pass MIDI to another device:

```
Pico GP0 (TX) ───[220Ω]─── MIDI OUT Pin 5
          GND ───[220Ω]─── MIDI OUT Pin 4
```

(Simplified circuit; full isolation requires additional optocoupler)

---

## Multi-Channel Layouts

### Perfboard Layout Example (8 Channels)

```
Component Side View:

   +12V Rail ═══════════════════════════════════
                │   │   │   │   │   │   │   │
   ┌─Solenoid──┘   │   │   │   │   │   │   │
   │               ┌┘  ┌┘  ┌┘  ┌┘  ┌┘  ┌┘  ┌┘
   D1  D2  D3  D4  D5  D6  D7  D8  ← Flyback Diodes
   │   │   │   │   │   │   │   │
   M1  M2  M3  M4  M5  M6  M7  M8  ← MOSFETs (TO-220)
   │   │   │   │   │   │   │   │
   R1  R2  R3  R4  R5  R6  R7  R8  ← Gate resistors
   │   │   │   │   │   │   │   │
   GP2 GP3 GP4 GP5 GP6 GP7 GP8 GP9 ← From Pico
   
   GND Rail ═══════════════════════════════════
```

**Spacing:**
- MOSFET spacing: 0.3" (7.62mm) for TO-220
- Keep gate traces short
- Separate power and signal traces

---

## PCB Design Guidelines

### Trace Widths

| Current | Trace Width (1oz copper) |
|---------|-------------------------|
| < 0.5A | 10 mil (0.25mm) |
| 0.5-1A | 15 mil (0.38mm) |
| 1-2A | 25 mil (0.64mm) |
| 2-5A | 50 mil (1.27mm) |

### Layer Stack

```
Top Layer:
- Signal traces (GPIO to gates)
- Component pads

Bottom Layer:
- GND plane (solid copper pour)
- Power traces (+12V, +5V)
```

### Design Checklist

- [ ] GND plane continuous
- [ ] Flyback diodes close to loads
- [ ] Gate resistors close to MOSFETs
- [ ] Power traces adequately sized
- [ ] Mounting holes for MOSFETs
- [ ] Screw terminals for loads
- [ ] Polarity markings
- [ ] Test points

---

## Cable Management

### Solenoid Wiring

**Wire Gauge:**
- 0.5A load: 24 AWG
- 1A load: 22 AWG
- 2A load: 20 AWG

**Connectors:**
- Screw terminals (5mm pitch)
- JST-XH (for quick disconnect)
- Molex KK (polarized)

**Best Practices:**
- Twist pairs (+ and -)
- Keep runs under 3m if possible
- Add strain relief
- Label both ends

### Power Distribution

**Star Ground:**
```
External PSU GND ─┬─ Pico GND
                  ├─ MOSFET Sources
                  └─ Load returns
```

**NOT daisy-chain ground!**

**Bus Bars:**
- Use for +12V distribution
- Use for GND return
- Add bulk capacitors (100-470µF) at PSU

---

## Example Schematics

### 1. Single Solenoid Test Rig

```
Minimal setup for testing:

[Pico]              [MOSFET]         [Solenoid]    [PSU 12V 1A]
  GP2 ──[470Ω]──── Gate               │              │
  GND ───────────── Source ──┬─────────┴──────────────┤
                             │                        │
                    Drain ───┴──[1N5819]──────────────┘
```

### 2. LED PWM Control

```
[Pico]                           [LED Strip]    [PSU 12V]
  GP10 ──[470Ω]── MOSFET Gate       │              │
  GND ──────────── Source ──┬────────┴──────────────┤
                            │                       │
                  Drain ────┴───────────────────────┘

No flyback diode needed for non-inductive loads
```

### 3. Relay Board Interface

```
[Pico]               [Relay Module]
  GP5 ────────────── IN1
  GP6 ────────────── IN2
  GND ────────────── GND
  VBUS (5V) ──────── VCC (if module needs 5V)

Many modules have built-in optocouplers and drivers
```

---

## Troubleshooting Wiring Issues

### MOSFET Not Switching

**Check:**
1. Gate resistor connected?
2. Source connected to GND?
3. Drain connected to load?
4. Common GND between Pico and PSU?
5. MOSFET orientation correct?

**Test:**
- Measure gate voltage when active (should be ~3.3V)
- Measure drain-source voltage when active (should be <0.5V)

### Flyback Diode Issues

**Symptoms:**
- Voltage spikes
- MOSFET failure
- Pico crashes/resets

**Check:**
- Diode polarity (stripe to +V)
- Diode rated for load current
- Diode physically close to load

### Ground Loops

**Symptoms:**
- Erratic behavior
- Noise in MIDI
- Random triggering

**Solution:**
- Single-point grounding (star configuration)
- Keep signal and power grounds separate until star point
- Use shielded cable for long MIDI runs

---

## Testing Procedure

### 1. Visual Inspection

- [ ] No solder bridges
- [ ] Correct component values
- [ ] Polarities correct (diodes, electrolytic caps)
- [ ] No cold solder joints
- [ ] Adequate clearance (no shorts)

### 2. Continuity Test (Power Off)

- [ ] GND continuity between Pico and PSU
- [ ] No short between +V and GND
- [ ] Gate resistors present
- [ ] Flyback diodes connected

### 3. Power-On Test (No Load)

- [ ] PSU voltage correct
- [ ] Pico powers up
- [ ] No excessive current draw
- [ ] No hot components

### 4. Signal Test (No Load)

- [ ] Gate voltage changes when GPIO activated
- [ ] MOSFET switches (measure drain voltage)
- [ ] No false triggering

### 5. Load Test

- [ ] Start with low-power load (LED)
- [ ] Progress to actual load
- [ ] Monitor temperature
- [ ] Check for voltage drops

---

## Advanced: Switching Speed Optimization

For fast pulses (<10ms):

**Add gate drive:**
```
Pico GPx ──[100Ω]──┬── MOSFET Gate
                   │
                 [10nF]
                   │
                  GND
```

**Result:**
- Faster turn-on/turn-off
- Reduced switching losses
- Better for high-frequency PWM

---

## Reference Documents

- **MIDI Specification:** [midi.org](https://www.midi.org/specifications)
- **Pico Datasheet:** [Raspberry Pi Pico Datasheet](https://datasheets.raspberrypi.com/pico/pico-datasheet.pdf)
- **MOSFET Datasheets:** Check manufacturer websites
- **Circuit symbols:** [Electronics symbols reference](https://en.wikipedia.org/wiki/Electronic_symbol)

---

## Safety Warning

⚠️ **High Current / High Voltage Warning:**

- Always disconnect power before wiring
- Double-check polarity
- Use appropriate wire gauge
- Add overcurrent protection (fuses)
- Ensure proper insulation
- Keep AC wiring (if used with relays) in separate enclosure
- Follow local electrical codes
