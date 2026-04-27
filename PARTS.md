# Teileliste - MIDI Solenoid Controller

## Grundkomponenten

| Anzahl | Komponente | Spezifikation | Beispiel/Quelle | ca. Preis |
|--------|-----------|---------------|-----------------|-----------|
| 1 | Raspberry Pi Pico | RP2040, ohne Header oder mit | Raspberry Pi Foundation | 4€ |
| 19 | N-Channel MOSFET | Logic-Level, >1A, TO-220 | IRLZ44N, IRL540N | 0,50€/St. |
| 19 | Flyback-Diode | Schottky, 1A+, DO-41 | 1N5819, SS34 | 0,10€/St. |
| 19 | Gate-Widerstand | 220Ω - 1kΩ, 1/4W | Metallfilm | 0,05€/St. |
| 19 | Solenoid | 12V, Push/Pull | JF-0530B, ZYE1-0420 | 2-5€/St. |
| 1 | Netzteil | 12V, min. 5A | Schaltnetzteil | 15€ |

## UART MIDI-Eingang (optional)

| Anzahl | Komponente | Spezifikation | Beispiel | ca. Preis |
|--------|-----------|---------------|----------|-----------|
| 1 | Optokoppler | 6N138 oder 6N137 | 6N138 | 0,50€ |
| 2 | Widerstand 220Ω | 1/4W | Metallfilm | 0,05€/St. |
| 1 | Widerstand 470Ω | 1/4W | Metallfilm | 0,05€ |
| 1 | 5-Pin DIN Buchse | Weiblich, Lötversion | MIDI Buchse | 1€ |

## Platine und Verkabelung

| Anzahl | Komponente | Beschreibung | ca. Preis |
|--------|-----------|--------------|-----------|
| 1 | Lochrasterplatine | 10x15cm oder größer | 3€ |
| - | Litze | Verschiedene Farben, 0,5-1mm² | 5€ |
| 2m | Schrumpfschlauch | Verschiedene Größen | 5€ |
| 20 | Schraubklemmen | 2-polig, 5mm Raster | 5€ |
| 2 | Pin-Header | 20-polig, 2,54mm | 2€ |
| - | Lötzinn | Bleifrei empfohlen | 5€ |

## Optional: Gehäuse und Montage

| Anzahl | Komponente | Beschreibung | ca. Preis |
|--------|-----------|--------------|-----------|
| 1 | Kunststoffgehäuse | Passend für Platine | 10€ |
| 19 | Kühlkörper | TO-220, falls nötig | 0,20€/St. |
| - | Montageblech | Für Solenoid-Befestigung | variabel |
| - | Schrauben/Muttern | M3, verschiedene | 5€ |
| 1 | Sicherung | 5A, Automat oder Schmelz | 2€ |

## Werkzeug (falls nicht vorhanden)

| Komponente | Beschreibung | ca. Preis |
|-----------|--------------|-----------|
| Lötkolben | 60W, temperaturgeregelt | 25€ |
| Multimeter | Digital, min. 20V, 10A | 15€ |
| Abisolierzange | Automatisch | 10€ |
| Seitenschneider | Klein | 8€ |
| Dritte Hand | Mit Lupe | 10€ |

## Gesamtkosten (Schätzung)

### Minimal-Konfiguration (USB MIDI only)
- Pico: 4€
- 19x MOSFET: 10€
- 19x Diode: 2€
- 19x Widerstand: 1€
- 19x Solenoid: 38-95€
- Netzteil 12V/5A: 15€
- Platine & Material: 15€
- **Gesamt: ca. 85-140€** (ohne Solenoids günstiger)

### Voll-Konfiguration (USB + UART MIDI)
- Minimal + MIDI-Eingang: +2€
- **Gesamt: ca. 87-142€**

### Mit Werkzeug (einmalig)
- +70€ falls komplett neu

## Bezugsquellen

### Europa
- **Reichelt Elektronik** (reichelt.de)
- **Conrad Electronic** (conrad.de)
- **TME** (tme.eu)
- **Mouser** (mouser.de)

### International
- **DigiKey** (digikey.com)
- **Mouser** (mouser.com)
- **AliExpress** (günstig, lange Lieferzeit)
- **Amazon** (schnell, aber teurer)

## Empfohlene Bestellliste

### Bei Reichelt/Conrad
```
1x Raspberry Pi Pico
20x IRLZ44N (1 Reserve)
20x 1N5819 (1 Reserve)
25x Widerstand 470Ω (Reserve)
1x 6N138 (falls UART MIDI)
2x Widerstand 220Ω (falls UART MIDI)
1x 5-Pin DIN Buchse (falls UART MIDI)
1x Lochrasterplatine 10x15cm
1x Schraubklemmen-Set 2-polig
1x Pin-Header 2,54mm
1x Litze-Set verschiedene Farben
```

### Bei Spezial-Händler (Solenoids)
```
19x 12V Solenoid (z.B. JF-0530B)
   - Push oder Pull je nach Anwendung
   - Spezifikation prüfen: Strom, Hub, Kraft
```

### Netzteil
```
1x Schaltnetzteil 12V/5A
   - Schutzklasse II
   - Mit Hohlstecker oder Schraubklemmen
```

## Alternative: Fertig-Module

Statt einzelner MOSFETs kannst du auch fertige Module verwenden:

| Modul | Vorteile | Nachteile | Preis |
|-------|----------|-----------|-------|
| IRF520 MOSFET Board | Fertig gelötet, LED | Größer, teurer | 1-2€/St. |
| MOSFET Array Board | Mehrere Kanäle | Weniger Auswahl | variabel |

**Hinweis:** IRF520 ist KEIN Logic-Level MOSFET! Module mit IRLZ44N suchen.

## Hinweise beim Einkauf

### MOSFETs
- ✅ **Logic-Level** (VGS < 3V)
- ✅ Ausreichend Strom für deine Solenoids
- ❌ NICHT: IRF520, IRF540 (Standard-Level)

### Solenoids
- Spannung beachten (12V empfohlen)
- Strom messen oder Datenblatt prüfen
- Push vs. Pull je nach Anwendung
- Mechanischer Hub passend?

### Netzteil
- Mindestens 5A bei 12V
- Besser: 10A falls viele gleichzeitig aktiv
- Mit CE/UL Zertifikat
- Überlast- und Kurzschlussschutz

### Dioden
- Schottky (1N5819) schneller als Standard (1N4007)
- Sperrspannung > Versorgungsspannung
- Durchlassstrom > Solenoid-Strom

---

**Tipp:** Bestelle 10-20% Reserve-Teile ein, falls beim Löten etwas schief geht!
