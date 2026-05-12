# Release-Anleitung: Firmware für GitHub Releases bauen

Schritt-für-Schritt Anleitung zum Erstellen und Veröffentlichen von Firmware-Releases.

---

## Was ist ein GitHub Release?

**GitHub Releases** ist eine Funktion, um versionierte Software-Pakete zu veröffentlichen.

### Wie funktioniert das?

```
Git Repository (Source Code)
    ↓
Git Tag (z.B. "v1.0.0") → markiert einen bestimmten Commit
    ↓
GitHub Release → erstellt eine öffentliche Download-Seite
    ↓
Release Assets → hier lädst du kompilierte Binaries hoch (z.B. firmware.uf2)
    ↓
User laden herunter → ohne das ganze Repo zu clonen
```

### Beispiel-Struktur eines Releases:

```
Release: v1.0.0
├── Tag: v1.0.0 (zeigt auf Commit abc1234)
├── Release Notes: "Was ist neu in dieser Version"
├── Assets:
│   ├── pico-midi-mapper-v1.0.0.uf2  ← Deine kompilierte Firmware
│   └── Source code (zip)             ← Automatisch von GitHub
└── Published: 12. Mai 2026
```

### Warum GitHub Releases nutzen?

✅ **User-freundlich** - Einfacher Download ohne Git-Kenntnisse  
✅ **Versionierung** - Klare Versionshistorie mit Changelogs  
✅ **CDN-Hosting** - GitHub hostet Dateien kostenlos und schnell  
✅ **Automatische Links** - Direktlinks wie `releases/latest/download/firmware.uf2`  
✅ **Badges** - Zeige neueste Version im README: ![Version](badge)

### Warum UF2-Dateien NICHT ins Git-Repo committen?

#### ❌ Probleme beim Committen von Binaries:

1. **Git ist für Text-Dateien optimiert**
   - Binärdateien (wie .uf2) werden nicht effizient gespeichert
   - Jede Änderung = komplette neue Datei im Git-Verlauf

2. **Repository wird riesig**
   ```
   Version 1: firmware.uf2 (800 KB)
   Version 2: firmware.uf2 (820 KB)
   Version 3: firmware.uf2 (850 KB)
   ...
   Nach 20 Versionen: 16+ MB nur für UF2s!
   
   Git speichert ALLE Versionen → Repo-Größe explodiert
   ```

3. **Langsames Clonen**
   - User müssen alle historischen Binaries downloaden
   - Auch wenn sie nur den neuesten Code wollen

4. **Merge-Konflikte**
   - Binärdateien können nicht gemerged werden
   - Bei parallel arbeitenden Entwicklern: Chaos

5. **Unnötig für Entwickler**
   - Entwickler bauen ihre eigene firmware.uf2 mit `pio run`
   - Committete Binaries sind meist veraltet

#### ✅ Richtige Lösung: GitHub Releases

```
Git Repo:
├── src/main.cpp          ← Source Code (committen!)
├── platformio.ini        ← Build-Config (committen!)
└── .gitignore            ← Ignoriert .pio/ und *.uf2

GitHub Releases:
├── v1.0.0/
│   └── pico-midi-mapper-v1.0.0.uf2  ← Binary hier!
├── v1.1.0/
│   └── pico-midi-mapper-v1.1.0.uf2
└── v2.0.0/
    └── pico-midi-mapper-v2.0.0.uf2
```

**Vorteile:**
- Git-Repo bleibt klein und schnell
- Nur Source Code im Versionsverlauf
- Binaries werden extern gehostet (GitHub CDN)
- Alte Versionen bleiben verfügbar, belasten Repo aber nicht

---

## Voraussetzungen

- PlatformIO installiert (VS Code Extension oder CLI)
- Git konfiguriert
- Zugriff auf GitHub Repository
- GitHub Account mit Push-Rechten

---

## Release erstellen (Schritt-für-Schritt)

### Schritt 1: Code vorbereiten

```bash
# Stelle sicher, dass alles committed ist
git status

# Sollte zeigen: "working tree clean"
# Falls nicht: Änderungen committen
git add .
git commit -m "Prepare for v1.0.0 release"
git push
```

**Optional:** Version im Code anpassen
```cpp
// In src/main.cpp hinzufügen:
#define FIRMWARE_VERSION "1.0.0"

void setup() {
    Serial.begin(115200);
    Serial.println("Pico MIDI Mapper v" FIRMWARE_VERSION);
    // ...
}
```

### Schritt 2: Firmware bauen

```bash
# Navigiere zum Projekt
cd ~/Library/Mobile\ Documents/com~apple~CloudDocs/Basteln/Glockenspiel

# Sauberen Build (optional, empfohlen für Releases)
pio run --target clean

# Projekt kompilieren
pio run

# ✅ Erfolg! UF2-Datei wurde erstellt in:
# .pio/build/pico/firmware.uf2
```

**Diese firmware.uf2 enthält:**
- ✅ Kompilierter Code (src/main.cpp + alle Bibliotheken)
- ✅ LittleFS Filesystem mit data/config.json
- ✅ Bootloader-Informationen für RP2040
- ✅ Alles in einer einzigen Datei

**Technische Details:**
```
firmware.uf2 Struktur:
┌─────────────────────────────────────┐
│ 0x10000000: Program Code (~600 KB) │
│ 0x10100000: LittleFS (1 MB)        │
└─────────────────────────────────────┘
Gesamtgröße: ~800 KB - 1.5 MB
```

### Schritt 3: UF2 umbenennen und testen

```bash
# Kopiere und benenne mit Version
cp .pio/build/pico/firmware.uf2 pico-midi-mapper-v1.0.0.uf2

# Prüfe Dateigröße (sollte ~600KB - 1.5MB sein)
ls -lh pico-midi-mapper-v1.0.0.uf2

# Optional: Teste auf echtem Pico
# 1. BOOTSEL drücken + USB verbinden
# 2. UF2 auf RPI-RP2 Laufwerk kopieren
# 3. Pico startet neu und LED sollte blinken
```

### Schritt 4: Git Tag erstellen

**Git Tags** markieren einen bestimmten Punkt in der Git-Historie.

```bash
# Erstelle annotierten Tag (empfohlen)
git tag -a v1.0.0 -m "Release 1.0.0 - Initial public release"

# Zeige alle Tags
git tag

# Push Tag zu GitHub (wichtig!)
git push origin v1.0.0
```

**Tag-Naming-Konvention:**
- `v1.0.0` - Semantic Versioning (SemVer)
  - `v1` = Major (breaking changes)
  - `.0` = Minor (neue Features)
  - `.0` = Patch (Bugfixes)

### Schritt 5: GitHub Release erstellen

#### Option A: Manuell über GitHub Web-Interface (Empfohlen für Anfang)

1. **Öffne dein Repository auf GitHub**
   ```
   https://github.com/USERNAME/pico-midi-mapper
   ```

2. **Gehe zu Releases**
   - Klicke auf "Releases" (rechte Sidebar)
   - Oder direkt: `https://github.com/USERNAME/pico-midi-mapper/releases`

3. **Erstelle neuen Release**
   - Klicke "Draft a new release" oder "Create a new release"

4. **Fülle Release-Formular aus:**

   **Choose a tag:** `v1.0.0` (aus Dropdown)
   
   **Release title:** `v1.0.0 - Pico MIDI Mapper Initial Release`
   
   **Release Notes:**
   ```markdown
   ## ✨ Features
   
   - 🎹 Dual MIDI Input (USB + UART)
   - 📍 Flexible GPIO mapping (bis zu 50 Mappings)
   - ⚡ Action Types: Pulse, Toggle, PWM
   - 🌐 Web-based visual configuration editor
   - 💾 USB Mass Storage config mode
   - 🎚️ Advanced filtering (velocity ranges, sustain)
   
   ## 📥 Installation
   
   **Firmware flashen:**
   1. Download `pico-midi-mapper-v1.0.0.uf2` (unten bei Assets)
   2. Halte BOOTSEL-Button auf dem Pico gedrückt
   3. Verbinde USB-Kabel (während BOOTSEL gedrückt)
   4. Pico erscheint als USB-Laufwerk "RPI-RP2"
   5. Ziehe die .uf2 Datei auf das Laufwerk
   6. Fertig! Pico startet automatisch neu
   
   **Config erstellen:**
   - Nutze den [Web-basierten MIDI Mapper Editor](https://elektrologue.com/midi-mapper-editor.html)
   - Oder siehe `examples/` Ordner für Beispiel-Configs
   
   ## 📚 Dokumentation
   
   - [Installation Guide](docs/INSTALLATION.md)
   - [Configuration Format](docs/CONFIGURATION_FORMAT.md)
   - [Hardware Setup](docs/HARDWARE_SETUP.md)
   - [USB Mass Storage Mode](docs/USB_MASS_STORAGE.md)
   
   ## 🐛 Known Issues
   
   - Keine bekannten Probleme in dieser Version
   
   ## 💬 Support
   
   Bei Fragen oder Problemen: [GitHub Issues](https://github.com/USERNAME/pico-midi-mapper/issues)
   ```

5. **Attach binaries:**
   - Ziehe `pico-midi-mapper-v1.0.0.uf2` in das "Attach binaries" Feld
   - Oder klicke "Attach files" und wähle die Datei

6. **Publish Release**
   - **Draft:** Entwurf (noch nicht öffentlich)
   - **Pre-release:** Beta-Version
   - **Set as latest release:** ✅ Aktivieren
   - Klicke "Publish release"

**Fertig!** 🎉

Der Release ist nun öffentlich verfügbar unter:
```
https://github.com/USERNAME/pico-midi-mapper/releases/tag/v1.0.0
```

Direct Download Link zur UF2:
```
https://github.com/USERNAME/pico-midi-mapper/releases/download/v1.0.0/pico-midi-mapper-v1.0.0.uf2
```

#### Option B: Mit GitHub CLI (Fortgeschritten)

```bash
# GitHub CLI installieren (falls nicht vorhanden)
brew install gh

# Bei GitHub authentifizieren (einmalig)
gh auth login

# Release erstellen mit einem Befehl
gh release create v1.0.0 \
  pico-midi-mapper-v1.0.0.uf2 \
  --title "v1.0.0 - Pico MIDI Mapper Initial Release" \
  --notes-file RELEASE_NOTES.md \
  --latest

# Oder mit direkten Notes:
gh release create v1.0.0 \
  pico-midi-mapper-v1.0.0.uf2 \
  --title "v1.0.0 - Initial Release" \
  --notes "See README.md for details"
```

---

## Wichtige Hinweise

### Was gehört ins Git-Repo?

✅ **JA - Committen:**
- `src/` - Source Code
- `data/config.json` - Minimale Default-Config
- `platformio.ini` - Build-Konfiguration
- `README.md`, `docs/` - Dokumentation
- `examples/` - Beispiel-Configs
- `LICENSE` - Lizenz
- `.gitignore` - Git-Ignore-Regeln

❌ **NEIN - Nicht committen:**
- `.pio/` - Build-Outputs (automatisch generiert)
- `*.uf2` - Kompilierte Binaries (→ GitHub Releases)
- `*.bin`, `*.elf`, `*.hex` - Andere Binaries
- `.vscode/` - IDE-spezifische Einstellungen (optional)
- `.DS_Store` - macOS-Müll

### Deine aktuelle `.gitignore`:

```gitignore
# PlatformIO
.pio/                  # Build-Outputs, Libraries Cache
.vscode/.browse.c_cpp.db*
.vscode/c_cpp_properties.json
.vscode/launch.json
.vscode/ipch

# Build artifacts
*.bin                  # Binärdateien
*.hex                  # Intel HEX Format
*.elf                  # ELF Binary
*.map                  # Memory Map
*.uf2                  # UF2 Firmware ← WICHTIG!

# OS
.DS_Store             # macOS Thumbnails
Thumbs.db             # Windows Thumbnails

# IDE
*.swp                 # Vim Swap
*.swo
*~
.idea/                # JetBrains IDEs

# Logs
*.log
```

### Versionierungs-Schema (Semantic Versioning)

```
v MAJOR . MINOR . PATCH
  │       │       │
  │       │       └─ Bugfixes, keine API-Änderungen
  │       └───────── Neue Features, abwärtskompatibel
  └───────────────── Breaking Changes, nicht kompatibel

Beispiele:
v1.0.0 → v1.0.1  (Bugfix, Config-Format unverändert)
v1.0.1 → v1.1.0  (Neues Feature: PWM-Mode)
v1.1.0 → v2.0.0  (Breaking: Config-Format geändert!)
```

### Checkliste vor Release

Vor jedem Release durchgehen:

- [ ] Code auf echtem Pico getestet
- [ ] `data/config.json` ist minimal/leer oder funktionierendes Beispiel
- [ ] CHANGELOG.md aktualisiert (falls vorhanden)
- [ ] README.md auf neue Features geprüft
- [ ] Docs aktualisiert (INSTALLATION.md, etc.)
- [ ] `pio run --target clean && pio run` erfolgreich
- [ ] firmware.uf2 manuell auf Pico getestet
- [ ] Dateigröße plausibel (800KB - 1.5MB)
- [ ] Git Tag erstellt und gepusht
- [ ] Release Notes vorbereitet
- [ ] UF2 mit korrektem Versionsnamen

---

## Schnell-Referenz

### Kompletter Release-Workflow (Copy-Paste)

```bash
# 1. Code vorbereiten
git status                              # Prüfen
git add .                               # Falls Änderungen
git commit -m "Prepare v1.0.0 release"
git push

# 2. Firmware bauen
cd ~/Library/Mobile\ Documents/com~apple~CloudDocs/Basteln/Glockenspiel
pio run --target clean
pio run

# 3. UF2 umbenennen
cp .pio/build/pico/firmware.uf2 pico-midi-mapper-v1.0.0.uf2
ls -lh pico-midi-mapper-v1.0.0.uf2     # Prüfen

# 4. Git Tag erstellen
git tag -a v1.0.0 -m "Release 1.0.0"
git push origin v1.0.0

# 5a. GitHub Release manuell erstellen
# → https://github.com/USERNAME/pico-midi-mapper/releases/new
# → UF2 hochladen, Release Notes schreiben, Publish

# 5b:
gh release create v1.0.0 \
  pico-midi-mapper-v1.0.0.uf2 \
  --title "v1.0.0 - Initial Release" \
  --notes "See README.md for details"

```

### Versions-Update für nächsten Release

```bash
# Von v1.0.0 → v1.0.1 (Bugfix)
pio run
cp .pio/build/pico/firmware.uf2 pico-midi-mapper-v1.0.1.uf2
git tag -a v1.0.1 -m "Bugfix release"
git push origin v1.0.1

# Von v1.0.1 → v1.1.0 (Neues Feature)
pio run
cp .pio/build/pico/firmware.uf2 pico-midi-mapper-v1.1.0.uf2
git tag -a v1.1.0 -m "Added PWM support"
git push origin v1.1.0
```

---

## Troubleshooting

### Build schlägt fehl

**Problem:** `pio run` zeigt Fehler

```bash
# Lösung 1: Platform und Packages updaten
pio pkg update
pio platform update

# Lösung 2: Clean build
pio run --target clean
pio run

# Lösung 3: Kompletter Reset
rm -rf .pio
pio run
```

### UF2 ist zu groß (>2MB)

**Problem:** firmware.uf2 ist größer als erwartet

```bash
# Prüfe Filesystem-Größe in platformio.ini
# board_build.filesystem_size = 1m  ← Aktuell 1MB

# Reduziere falls nötig:
# board_build.filesystem_size = 512k
```

### Filesystem nicht inkludiert

**Problem:** config.json fehlt nach Flash

```bash
# Stelle sicher dass data/ Ordner existiert
ls -la data/

# Sollte zeigen:
# data/
#   └── config.json

# Falls fehlend:
mkdir -p data
echo '{"version":1,"maps":[]}' > data/config.json
```

### Git Tag existiert bereits

**Problem:** `tag 'v1.0.0' already exists`

```bash
# Lösung 1: Tag löschen und neu erstellen
git tag -d v1.0.0                    # Lokal löschen
git push origin :refs/tags/v1.0.0    # Remote löschen
git tag -a v1.0.0 -m "Release 1.0.0"
git push origin v1.0.0

# Lösung 2: Nächste Version nutzen
git tag -a v1.0.1 -m "Release 1.0.1"
```

### Release auf GitHub nicht sichtbar

**Problem:** Tag gepusht, aber kein Release

```bash
# Tags werden automatisch NICHT zu Releases
# Du musst manuell den Release erstellen auf:
# https://github.com/USERNAME/REPO/releases/new

# Oder mit GitHub CLI:
gh release create v1.0.0 firmware.uf2
```

---

## Weitere Ressourcen

- [PlatformIO Docs](https://docs.platformio.org)
- [GitHub Releases Guide](https://docs.github.com/en/repositories/releasing-projects-on-github)
- [Semantic Versioning](https://semver.org)
- [UF2 Format Spec](https://github.com/microsoft/uf2)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)

---

## Zusammenfassung

### Warum dieser Workflow?

| Aspekt | Ohne Releases | Mit GitHub Releases |
|--------|---------------|---------------------|
| **Repo-Größe** | Wächst mit jedem Build | Bleibt klein (nur Code) |
| **Download-Speed** | Langsam (alle Binaries) | Schnell (nur Source) |
| **User-Erfahrung** | Kompliziert (PlatformIO nötig) | Einfach (UF2 Download) |
| **Versionierung** | Unklar | Klar (Tags, Changelogs) |
| **Hosting** | Selbst hosten | GitHub CDN (kostenlos) |
| **Verlauf** | Vermüllt | Sauber getrennt |

### Die goldene Regel

**Git = Source Code**  
**GitHub Releases = Binaries**

Trenne beides strikt, und du hast ein wartbares, professionelles Projekt! 🚀
