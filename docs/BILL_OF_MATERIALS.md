# Bill of Materials

Complete parts list for building your Pico MIDI Mapper project.

---

## Core Components

| Qty | Component | Specification | Example Part | Est. Price |
|-----|-----------|---------------|--------------|------------|
| 1 | Raspberry Pi Pico | RP2040, with or without headers | Raspberry Pi Pico | $4-6 |
| 1 | USB Cable | Micro-USB, data-capable | Generic | $2-5 |

**Total Core:** ~$6-11

---

## Per-Channel Components (for each GPIO output)

| Qty | Component | Specification | Example Part | Price Each |
|-----|-----------|---------------|--------------|------------|
| 1 | N-Channel MOSFET | Logic-level, TO-220 | IRLZ44N, IRL540N | $0.50-1.50 |
| 1 | Flyback Diode | Schottky, 1A+, DO-41 | 1N5819, SS34 | $0.10-0.30 |
| 1 | Gate Resistor | 470Ω, 1/4W, through-hole | Metal film | $0.05-0.10 |

**Per-Channel Total:** ~$0.65-1.90

**For 19 channels (solenoid glockenspiel):** $12-36

---

## Load Components (Choose Your Use Case)

### Option A: Solenoids

| Qty | Component | Specification | Example Part | Price Each |
|-----|-----------|---------------|--------------|------------|
| 19 | Solenoid | 12V, push/pull type | JF-0530B, ZYE1-0420 | $2-5 |

**19× Solenoids:** $38-95

### Option B: LEDs (Direct Drive)

| Qty | Component | Specification | Price Each |
|-----|-----------|---------------|------------|
| 8 | LED | 5mm, various colors | $0.10-0.30 |
| 8 | Current-limiting resistor | Calculated (see guide) | $0.05 |

**8× LEDs:** $1-3

### Option C: High-Power LEDs / LED Strips

| Qty | Component | Specification | Price Each |
|-----|-----------|---------------|------------|
| 5m | LED Strip | 12V, addressable or single color | $10-30 |

### Option D: Relays

| Qty | Component | Specification | Price Each |
|-----|-----------|---------------|------------|
| 8 | Relay Module | 5V, optocoupler isolated | $1-2 |

**8× Relay Modules:** $8-16

---

## Power Supply

| Component | Specification | Example | Est. Price |
|-----------|---------------|---------|------------|
| Power Supply | **For Solenoids:** 12V, 5-10A, switching | Mean Well RS-100-12 | $15-30 |
|  | **For LEDs:** 12V, 2-5A | Generic 12V adapter | $8-15 |
|  | **For Relays:** 5V, 2A | USB power supply | $5-10 |

**Choose based on your load!**

---

## UART MIDI Input (Optional)

| Qty | Component | Specification | Example Part | Est. Price |
|-----|-----------|---------------|--------------|------------|
| 1 | Optocoupler | 6N138 or 6N137, DIP-8 | 6N138 | $0.50-1.00 |
| 2 | Resistor 220Ω | 1/4W, through-hole | Metal film | $0.05 |
| 1 | Resistor 470Ω | 1/4W, through-hole | Metal film | $0.05 |
| 1 | 5-Pin DIN Jack | Female, PCB or panel mount | MIDI jack | $1-2 |

**MIDI Input Total:** $2-4

---

## Circuit Board & Assembly

| Qty | Component | Description | Est. Price |
|-----|-----------|-------------|------------|
| 1 | Perfboard | 10×15cm or larger | $3-5 |
| - | Wire | 22-24 AWG, various colors | $5-10 |
| 2m | Heat Shrink Tubing | Assorted sizes | $5 |
| 10-20 | Screw Terminals | 2-3 pole, 5mm pitch | $5-10 |
| 2 | Pin Headers | 20-pin, 2.54mm pitch (for Pico) | $2 |
| - | Solder | Lead-free recommended | $5-10 |

**Assembly Total:** $25-50

---

## Optional Components

### Config Button

| Qty | Component | Specification | Est. Price |
|-----|-----------|---------------|------------|
| 1 | Pushbutton | Normally Open (NO), momentary | $0.50-2 |

### Cooling (for high-duty solenoids)

| Qty | Component | Specification | Est. Price |
|-----|-----------|---------------|------------|
| 8-19 | Heatsink | TO-220 compatible | $0.20-0.50 |
| 1 | Thermal Paste | Small tube | $3-5 |

### Enclosure

| Qty | Component | Specification | Est. Price |
|-----|-----------|---------------|------------|
| 1 | Project Box | Plastic, sized for your board | $10-20 |
| - | Mounting Hardware | Standoffs, screws | $3-5 |

### Protection

| Qty | Component | Specification | Est. Price |
|-----|-----------|---------------|------------|
| 1 | Fuse Holder | Inline, panel mount | $1-2 |
| 1 | Fuse | 5A or 10A, appropriate voltage | $0.50-1 |
| 1 | Power Switch | SPST, rated for PSU current | $1-3 |

---

## Tools (if not already owned)

| Tool | Specification | Est. Price |
|------|---------------|------------|
| Soldering Iron | 60W, temperature controlled | $25-50 |
| Solder Sucker | Desoldering pump | $5-10 |
| Wire Strippers | Automatic, 22-28 AWG | $10-15 |
| Flush Cutters | Small, for component leads | $8-12 |
| Multimeter | Digital, voltage/current/continuity | $15-30 |
| Third Hand | With magnifier | $10-15 |
| Heat Gun | For heat shrink (optional) | $15-25 |

**Basic Tool Set:** $88-167

---

## Cost Breakdowns

### Minimal Test Setup (LEDs only)

| Item | Cost |
|------|------|
| Pico + USB cable | $6-11 |
| 8× LEDs with resistors | $1-3 |
| Perfboard & wire | $5-10 |
| **TOTAL** | **$12-24** |

---

### 8-Channel Solenoid Project

| Item | Cost |
|------|------|
| Pico + USB cable | $6-11 |
| 8× MOSFET + diode + resistor | $5-15 |
| 8× Solenoids | $16-40 |
| Power supply 12V/5A | $15-30 |
| Assembly materials | $25-50 |
| **TOTAL** | **$67-146** |

---

### 19-Channel Glockenspiel (Full Build)

| Item | Cost |
|------|------|
| Pico + USB cable | $6-11 |
| 19× MOSFET + diode + resistor | $12-36 |
| 19× Solenoids | $38-95 |
| Power supply 12V/10A | $20-40 |
| UART MIDI input | $2-4 |
| Assembly materials | $30-60 |
| Enclosure | $10-20 |
| **TOTAL** | **$118-266** |

---

### LED Matrix / PWM Project

| Item | Cost |
|------|------|
| Pico + USB cable | $6-11 |
| 16× MOSFET + diode + resistor | $10-30 |
| LED strips (5m) | $10-30 |
| Power supply 12V/5A | $15-30 |
| Assembly materials | $25-50 |
| **TOTAL** | **$66-151** |

---

### 8-Channel Relay Board

| Item | Cost |
|------|------|
| Pico + USB cable | $6-11 |
| 8× Relay modules | $8-16 |
| Power supply 5V/2A | $5-10 |
| Assembly materials | $15-30 |
| Enclosure | $10-20 |
| **TOTAL** | **$44-87** |

---

## Where to Buy

### Electronics Components

**North America:**
- [DigiKey](https://www.digikey.com) - Comprehensive, fast shipping
- [Mouser](https://www.mouser.com) - Similar to DigiKey
- [Adafruit](https://www.adafruit.com) - Maker-friendly, good tutorials
- [SparkFun](https://www.sparkfun.com) - Maker-focused
- [Amazon](https://www.amazon.com) - Fast but check specs

**Europe:**
- [Reichelt](https://www.reichelt.de) - Germany
- [Conrad Electronic](https://www.conrad.de) - Germany
- [TME](https://www.tme.eu) - Poland, EU-wide shipping
- [Mouser](https://www.mouser.de) - International
- [Farnell](https://www.farnell.com) - UK

**Asia/Pacific:**
- [AliExpress](https://www.aliexpress.com) - Cheap, slow shipping
- [Banggood](https://www.banggood.com) - Similar to AliExpress
- [Taobao](https://www.taobao.com) - China domestic
- [Element14](https://www.element14.com) - Australia/NZ

**Local:**
- Check for electronics stores in your area
- University surplus stores
- Makerspace group buys

---

## Shopping List Generator

### Basic 8-Channel Solenoid Setup

**DigiKey/Mouser Cart:**
```
1× Raspberry Pi Pico (SC0915)
8× IRLZ44N MOSFET (IRLZ44NPBF)
8× 1N5819 Diode (1N5819-TP)
10× 470Ω Resistor 1/4W (CFR-25JB-52-470R)
1× Perfboard 10×15cm
1× Screw terminal strip, 2-pole, 10 position
1× 12V 5A power supply
Wire kit, 22 AWG, 6 colors
```

**Estimated Total:** $40-60 (without solenoids)

### Testing/Development Kit

```
1× Raspberry Pi Pico
1× Breadboard, 830 tie-points
1× Jumper wire kit
10× 5mm LED, assorted colors
10× 220Ω resistor
1× USB cable, micro-USB
```

**Estimated Total:** $15-25

---

## Component Selection Tips

### MOSFETs

✅ **Recommended:**
- IRLZ44N (best value, widely available)
- IRL540N (higher voltage rating)
- IRL520 (lower current, cheaper)

❌ **Avoid:**
- IRF520 (not logic-level!)
- IRF540 (not logic-level!)
- 2N7000 (too low current for most loads)

### Diodes

✅ **Recommended:**
- 1N5819 Schottky (fast, low voltage drop)
- 1N5817 Schottky (3A version)
- SS34 Schottky (alternative)

⚠️ **OK but slower:**
- 1N4007 (standard rectifier, works but slower)

### Solenoids

**Check datasheet for:**
- Operating voltage (12V most common)
- Current draw (affects PSU sizing)
- Duty cycle (usually <10% - short pulses!)
- Stroke length (how far it moves)
- Force (how strong)

**Common types:**
- Push solenoid: Plunger extends when energized
- Pull solenoid: Plunger retracts when energized

---

## Bulk Discounts

**For larger projects:**
- Buy MOSFETs in packs of 10-25: ~30% savings
- Diodes in strips of 50-100: ~40% savings
- Resistor kits (all values): ~50% vs individual
- Chinese suppliers (AliExpress): 50-80% savings, but verify specs!

---

## Salvage & Recycling

**Free/cheap sources:**
- Old printers: Stepper motors, solenoids
- Computer PSUs: 12V, 5V, 3.3V rails
- Dead electronics: MOSFETs, diodes, resistors
- Scrap wire: Plenty in old electronics

**⚠️ Note:** Verify component specs before using salvaged parts!

---

## Budget Optimization

**To reduce costs:**
1. Start with LED project (cheapest)
2. Use breadboard before perfboard
3. Buy from AliExpress (but allow 3-6 weeks)
4. Join local makerspace (tools available)
5. Group buys with friends (bulk discounts)
6. Skip enclosure initially (test on bench)
7. Reuse old USB cables, wire

---

## Next Steps

- [Installation Guide →](INSTALLATION.md)
- [Hardware Setup →](HARDWARE_SETUP.md)
- [Wiring Details →](WIRING_DETAILS.md)
