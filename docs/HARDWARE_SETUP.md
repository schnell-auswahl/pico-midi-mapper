# Hardware Setup Guide

## Component Overview

The Pico MIDI Mapper can control various types of loads through GPIO pins. The hardware setup varies depending on what you're controlling.

## Output Types

### 1. Direct LED Control (Low Current)

**For LEDs < 20mA** (standard indicator LEDs):

```
Pico GPx ───[Resistor]─── LED ─── GND
```

**Resistor calculation:**
```
R = (3.3V - LED_Vf) / LED_Current
Example: (3.3V - 2.0V) / 0.02A = 65Ω (use 68Ω or 100Ω)
```

**No MOSFET needed!**

---

### 2. MOSFETControl (High Current)

**For solenoids, motors, high-power LEDs, relays:**

```
                     +Vsupply (12V typical)
                       |
                    [LOAD]
                       |
                       +──────|◁|──────+ Flyback Diode
                       |               |
                       D               |
                       |               |
Pico GPx ──[470Ω]──── G   MOSFET      |
                       |               |
                       S               |
                       |               |
                      GND ────────── GND (Common with Pico)
```

**Components:**
- **MOSFET**: Logic-level N-channel (IRLZ44N, IRL540N, IRL520)
- **Gate resistor**: 220Ω - 1kΩ (protects GPIO)
- **Flyback diode**: 1N5819 Schottky or 1N4007 (for inductive loads)
- **External PSU**: Appropriate voltage/current for your load

---

### 3. Relay Control

**For switching AC or isolated DC:**

```
Pico GPx ──[1kΩ]─── Transistor Base
                    Transistor Collector ─── Relay Coil ─── +5V
                    Transistor Emitter ───────┬──────────── GND
                                              │
                                      |◁| Flyback Diode
```

**Or use optocoupler relay module:**
```
Pico GPx ───── Module IN
Pico GND ───── Module GND
+5V ────────── Module VCC
```

---

## MOSFET Selection Guide

### Logic-Level MOSFETs (3.3V compatible)

| Part Number | Vds | Id | RDS(on) | Use Case |
|-------------|-----|----|---------| ---------|
| **IRLZ44N** | 55V | 35A | 22mΩ | General purpose (recommended) |
| **IRL540N** | 100V | 28A | 44mΩ | Higher voltage |
| **IRL520** | 100V | 10A | 200mΩ | Low current |
| **AO3400** | 30V | 5.8A | 48mΩ | SMD, low current |

### ❌ NOT Compatible (need >5V gate voltage)

- IRF520
- IRF540  
- 2N7000 (technically logic-level but low current)

### Selection Criteria

1. **VGS(th) < 2.5V** (turns on fully with 3.3V)
2. **ID(max) ≥ Load current** (with safety margin)
3. **VDS(max) > Supply voltage**
4. **RDS(on)** - Lower is better (less heat)

---

## Solenoid-Specific Setup

### Solenoid Types

**Push-type:**
- Plunger extends when energized
- Spring returns when off
- Good for striking/hitting

**Pull-type:**
- Plunger retracts when energized
- Spring extends when off
- Good for pulling/latching

### Typical Specifications

| Voltage | Current | Force | Stroke | Part Example |
|---------|---------|-------|--------|--------------|
| 12V | 0.4A | 1N | 10mm | JF-0530B |
| 12V | 0.8A | 5N | 10mm | ZYE1-0530 |
| 24V | 0.4A | 5N | 10mm | SFO-0520 |

### Power Supply Sizing

**Formula:**
```
Required Current = N_simultaneous × Solenoid_Current × 1.2

Example: 5 solenoids × 0.8A × 1.2 = 4.8A minimum
```

**Recommended:** 12V, 5-10A power supply for typical projects

### Pulse Duration Guidelines

| Duration | Effect | Use Case |
|----------|--------|----------|
| 20-30ms | Light tap | Gentle strikes |
| 30-50ms | Normal strike | Standard percussion |
| 50-100ms | Hard strike | Maximum force |
| >100ms | ⚠️ Overheat risk | Not recommended |

---

## MIDI Input Options

### Option 1: USB MIDI Only

**Simplest setup - no additional hardware!**

```
Computer/MIDI Controller ──USB── Pico
```

**Pros:**
- No additional components
- Powered via USB
- Easy debugging via serial

**Cons:**
- Requires USB host
- Not standalone

---

### Option 2: UART MIDI (5-Pin DIN)

**Standard MIDI hardware connection:**

```
MIDI Device (5-Pin DIN) ──MIDI Cable── Optocoupler Circuit ── Pico GPIO 1
```

**Circuit Diagram:**

```
MIDI IN Jack (Female DIN-5)
    Pin 2 ──────────────────────── Shield/GND (chassis, optional)
    Pin 4 ───[220Ω]──┬────────┐
    Pin 5 ───[220Ω]──┘        │
                           ┌──┴──┐
                        1  │  O  │  8 ── +3.3V
                        2  │ 6N  │  7 ── GND
                    NC ─ 3 │ 138 │  6 ──[470Ω]── +3.3V
                           │     │  5 ────────── GPIO 1 (RX)
                           └─────┘
```

**Components:**
- **6N138** or 6N137 optocoupler (high-speed)
- **2× 220Ω** resistors (MIDI input)
- **1× 470Ω** resistor (pull-up)
- **5-pin DIN jack** (female, 180° or 270°)

**Pros:**
- Industry standard
- Galvanic isolation
- Can receive MIDI from synthesizers, drum machines, etc.

**Cons:**
- Requires additional components
- More complex wiring

---

### Option 3: Both USB and UART

**Best of both worlds!**

The firmware supports simultaneous USB and UART MIDI. Both inputs are merged and processed identically.

---

## Pin Assignments

### Reserved Pins

| GPIO | Function | Note |
|------|----------|------|
| 0 | UART TX | (optional MIDI out) |
| 1 | UART RX | MIDI in if using 5-pin DIN |
| 22 | CONFIG Button | Hold during boot for USB mass storage |
| 25 | Onboard LED | Good for testing |

### Available for Output

**GPIO 2-21, 23-24, 26-28** = 24 pins available

**Note:** Some pins may be used for special functions (ADC, I2C, SPI). Check [Pico pinout](https://datasheets.raspberrypi.com/pico/Pico-R3-A4-Pinout.pdf).

---

## Example Wiring: 8-Solenoid Setup

```
Raspberry Pi Pico              MOSFETs & Solenoids
┌─────────────┐
│ GP2  ○    ○ │───[470Ω]─── M1 ─── Solenoid 1
│ GP3  ○    ○ │───[470Ω]─── M2 ─── Solenoid 2
│ GP4  ○    ○ │───[470Ω]─── M3 ─── Solenoid 3
│ GP5  ○    ○ │───[470Ω]─── M4 ─── Solenoid 4
│ GP6  ○    ○ │───[470Ω]─── M5 ─── Solenoid 5
│ GP7  ○    ○ │───[470Ω]─── M6 ─── Solenoid 6
│ GP8  ○    ○ │───[470Ω]─── M7 ─── Solenoid 7
│ GP9  ○    ○ │───[470Ω]─── M8 ─── Solenoid 8
│      ○    ○ │
│ GND  ●────┼────── Common GND
└─────────────┘    │
                   │
External 12V PSU ──┴── All MOSFET Sources & Solenoid -
                       All Solenoid + to +12V
```

---

## Power Considerations

### Pico Power

**Input:** 5V via USB or VSYS pin (up to 5.5V)
**GPIO current:** Max 12mA per pin, 50mA total

**⚠️ Never power loads directly from Pico 3.3V or 5V pins!**

### External Power Supply

**Requirements:**
- Voltage: Match your load (12V for most solenoids)
- Current: Sum of all loads + 20% margin
- Regulation: Stable output
- Protection: Overcurrent/short circuit

**Common GND:**
```
⚠️ CRITICAL: Pico GND MUST connect to external PSU GND
```

Without common ground, MOSFETs won't switch!

---

## Safety & Best Practices

### Thermal Management

**MOSFETs:**
- Add heatsink if RDS(on) × I² > 0.5W
- Use TO-220 package for easier heatsinking
- Keep traces short and thick

**Solenoids:**
- Never exceed duty cycle limits (typically <10%)
- Add cooling fans for high-frequency operation
- Monitor temperature during testing

### Protection

**Flyback Diodes:**
- ALWAYS use with inductive loads (solenoids, relays, motors)
- Cathode (stripe) to positive supply
- Anode to MOSFET drain

**Fuses:**
- Add inline fuse on PSU output
- Size 1.5× expected current

### Assembly Tips

1. **Test on breadboard first**
2. **Use perfboard or PCB for permanent**
3. **Label all wires** (color code!)
4. **Strain relief** for solenoid wires
5. **Cable management** - keep power and signal separated

---

## Testing Checklist

Before connecting loads:

- [ ] MOSFET orientation correct?
- [ ] Gate resistor installed?
- [ ] Flyback diode polarity correct?
- [ ] Common GND connected?
- [ ] Power supply voltage correct?
- [ ] No shorts between pins?

Test sequence:

1. Power on with only one MOSFET connected
2. Test with LED test (GPIO 25)
3. Test one GPIO at a time
4. Monitor current draw
5. Check for overheating

---

## Next Steps

- [Complete wiring diagrams →](WIRING_DETAILS.md)
- [Bill of materials →](BILL_OF_MATERIALS.md)
- [Configuration guide →](CONFIGURATION_FORMAT.md)
