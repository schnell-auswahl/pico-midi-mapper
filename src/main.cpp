#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// ============================================================================
// KONFIGURATION
// ============================================================================

#define CONFIG_BUTTON_PIN 22  // Button zum Aktivieren des Config-Modus
#define MAX_MAPPINGS 50       // Maximale Anzahl an Mappings

// Debug Print Macro (nur wenn DEBUG_MODE aktiv)
#ifdef DEBUG_MODE
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif

// ============================================================================
// STRUKTUREN
// ============================================================================

enum ActionType {
    ACTION_PULSE,      // Kurzer Puls (z.B. Solenoid)
    ACTION_TOGGLE,     // An/Aus wechseln
    ACTION_PWM,        // PWM-Steuerung über Velocity/CC-Wert
    ACTION_GATE        // An bei Note On, Aus bei Note Off
};

struct MIDIMapping {
    bool active;
    String type;           // "note_on", "note_off", "control_change"
    byte channel;          // MIDI Kanal (1-16, 0 = alle)
    byte note_or_cc;       // Note oder CC Nummer
    byte pin;              // GPIO Pin
    ActionType action;     // Was soll passiert
    uint16_t duration;     // Pulse-Dauer in ms
    bool toggle_state;     // Aktueller Toggle-Zustand
};

// ============================================================================
// GLOBALE VARIABLEN
// ============================================================================

MIDIMapping mappings[MAX_MAPPINGS];
int mappingCount = 0;

// USB MIDI & Mass Storage
Adafruit_USBD_MIDI usbMidi;
Adafruit_USBD_MSC usb_msc;

// MIDI Instanzen
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMidi, MIDI_USB);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI_HW);

// Pulse-Timing
unsigned long pulseTimes[MAX_MAPPINGS];

// ============================================================================
// FORWARD DEKLARATIONEN
// ============================================================================

void startMIDIMode();
void startUSBStorageMode();

// ============================================================================
// USB MASS STORAGE CALLBACKS
// ============================================================================

// RAMDisk für USB Mass Storage
// Größe: 128 Blöcke à 512 Bytes = 64 KB
#define DISK_BLOCK_NUM  128
#define DISK_BLOCK_SIZE 512
static uint8_t msc_disk[DISK_BLOCK_NUM][DISK_BLOCK_SIZE];

int32_t msc_read_cb(uint32_t lba, void* buffer, uint32_t bufsize) {
    // Lese Block aus RAMDisk
    if (lba >= DISK_BLOCK_NUM) return -1;
    
    uint8_t* addr = msc_disk[lba];
    memcpy(buffer, addr, bufsize);
    
    return bufsize;
}

int32_t msc_write_cb(uint32_t lba, uint8_t* buffer, uint32_t bufsize) {
    // Schreibe Block in RAMDisk
    if (lba >= DISK_BLOCK_NUM) return -1;
    
    uint8_t* addr = msc_disk[lba];
    memcpy(addr, buffer, bufsize);
    
    return bufsize;
}

void msc_flush_cb() {
    // Wird aufgerufen wenn Host "safely remove" macht
    // Hier könnten wir Daten zurück zu LittleFS schreiben
    DEBUG_PRINTLN("MSC Flush - Daten gespeichert");
}

// ============================================================================
// FAT12 FILESYSTEM HELPER
// ============================================================================

void createFAT12Image() {
    // Minimales FAT12 Filesystem erstellen (64KB)
    memset(msc_disk, 0, sizeof(msc_disk));
    
    // Boot Sector (Block 0)
    uint8_t* boot = msc_disk[0];
    boot[0] = 0xEB; boot[1] = 0x3C; boot[2] = 0x90;
    memcpy(&boot[3], "MSDOS5.0", 8);
    boot[11] = 0x00; boot[12] = 0x02;  // 512 bytes/sector
    boot[13] = 1;                       // 1 sector/cluster
    boot[14] = 0x01; boot[15] = 0x00;  // 1 reserved sector
    boot[16] = 2;                       // 2 FATs
    boot[17] = 0x10; boot[18] = 0x00;  // 16 root entries
    boot[19] = 0x80; boot[20] = 0x00;  // 128 total sectors
    boot[21] = 0xF8;                    // Media descriptor
    boot[22] = 0x01; boot[23] = 0x00;  // 1 sector/FAT
    boot[510] = 0x55; boot[511] = 0xAA;
    
    // FAT Tables (Cluster 0=Media, 1=Reserved, Cluster 2+ wird nach Datei-Copy gesetzt)
    uint8_t* fat1 = msc_disk[1];
    fat1[0] = 0xF8; fat1[1] = 0xFF; fat1[2] = 0xFF;  // Cluster 0-1 (Media + Reserved)
    uint8_t* fat2 = msc_disk[2];
    fat2[0] = 0xF8; fat2[1] = 0xFF; fat2[2] = 0xFF;  // Cluster 0-1 (Kopie)
    
    // Root Directory
    uint8_t* root = msc_disk[3];
    
    // Volume Label
    memcpy(&root[0], "PICO FS    ", 11);
    root[11] = 0x08;
    
    // config.json Entry
    uint8_t* entry = &root[32];
    memcpy(&entry[0], "CONFIG  JSO ", 11);
    entry[11] = 0x20;  // Archive
    entry[26] = 2;     // Cluster 2
    entry[27] = 0;
    
    // Datei direkt aus LittleFS in RAMDisk kopieren (OHNE String!)
    uint32_t fileSize = 0;
    int dataBlock = 4;  // Cluster 2 = Block 4 (nach Boot+FAT1+FAT2+RootDir)
    
    if (LittleFS.exists("/config.json")) {
        File f = LittleFS.open("/config.json", "r");
        if (f) {
            // Direkt blockweise kopieren
            while (f.available() && dataBlock < DISK_BLOCK_NUM) {
                uint8_t* destBlock = msc_disk[dataBlock];
                size_t bytesRead = f.read(destBlock, DISK_BLOCK_SIZE);
                fileSize += bytesRead;
                
                if (bytesRead < DISK_BLOCK_SIZE) {
                    // Letzter Block - Rest mit 0 füllen
                    memset(destBlock + bytesRead, 0, DISK_BLOCK_SIZE - bytesRead);
                    break;
                }
                dataBlock++;
            }
            f.close();
        }
    }
    
    // Fallback wenn keine Datei oder Fehler
    if (fileSize == 0) {
        const char* fallback = "{\"device_name\":\"Glockenspiel\",\"version\":\"1.0\",\"mappings\":[]}";
        fileSize = strlen(fallback);
        memcpy(msc_disk[4], fallback, fileSize);
    }
    
    // File size in Directory Entry schreiben
    entry[28] = fileSize & 0xFF;
    entry[29] = (fileSize >> 8) & 0xFF;
    entry[30] = (fileSize >> 16) & 0xFF;
    entry[31] = (fileSize >> 24) & 0xFF;
    
    // FAT-Kette aufbauen für alle verwendeten Cluster
    int numClusters = (fileSize + DISK_BLOCK_SIZE - 1) / DISK_BLOCK_SIZE; // Aufrunden
    if (numClusters == 0) numClusters = 1; // Mindestens 1 Cluster
    
    for (int i = 0; i < numClusters; i++) {
        int cluster = 2 + i;  // Cluster 2, 3, 4, ...
        int nextCluster = (i == numClusters - 1) ? 0xFFF : (cluster + 1);
        
        // FAT12: 12-bit Eintrag schreiben (je 1.5 Bytes pro Cluster)
        int byteOffset = (cluster * 3) / 2;
        if (cluster % 2 == 0) {
            // Gerade Cluster: Low 8 bits + Low 4 bits von High
            fat1[byteOffset] = nextCluster & 0xFF;
            fat1[byteOffset + 1] = (fat1[byteOffset + 1] & 0xF0) | ((nextCluster >> 8) & 0x0F);
        } else {
            // Ungerade Cluster: High 4 bits von Low + High 8 bits
            fat1[byteOffset] = (fat1[byteOffset] & 0x0F) | ((nextCluster & 0x0F) << 4);
            fat1[byteOffset + 1] = (nextCluster >> 4) & 0xFF;
        }
    }
    // FAT2 kopieren
    memcpy(fat2, fat1, DISK_BLOCK_SIZE);
    
    DEBUG_PRINTF("FAT12 Image erstellt: config.json (%d bytes, %d cluster)\n", fileSize, numClusters);
}

void saveFAT12ToLittleFS() {
    // Extrahiere config.json aus FAT12 Image zurück zu LittleFS
    uint8_t* root = msc_disk[3];
    uint8_t* entry = &root[32]; // config.json Entry
    
    // File size lesen
    uint32_t size = entry[28] | (entry[29] << 8) | (entry[30] << 16) | (entry[31] << 24);
    
    if (size > 0 && size < 65536) { // Sicherheitscheck
        // Start-Cluster lesen (sollte immer 2 sein)
        uint16_t cluster = entry[26] | (entry[27] << 8);
        
        // FAT-Kette folgen und Daten sammeln
        uint8_t* fat1 = msc_disk[1];
        String jsonContent = "";
        uint32_t bytesRead = 0;
        
        while (cluster >= 2 && cluster < 0xFF0 && bytesRead < size) {
            // Block = Cluster + 2 (wegen Boot, FAT1, FAT2, Root)
            int dataBlock = cluster + 2;
            if (dataBlock >= DISK_BLOCK_NUM) break;
            
            size_t copySize = min((size_t)DISK_BLOCK_SIZE, size - bytesRead);
            char temp[DISK_BLOCK_SIZE + 1];
            memcpy(temp, msc_disk[dataBlock], copySize);
            temp[copySize] = 0;
            jsonContent += String(temp);
            bytesRead += copySize;
            
            // Nächsten Cluster aus FAT lesen
            int byteOffset = (cluster * 3) / 2;
            if (cluster % 2 == 0) {
                cluster = fat1[byteOffset] | ((fat1[byteOffset + 1] & 0x0F) << 8);
            } else {
                cluster = ((fat1[byteOffset] >> 4) & 0x0F) | (fat1[byteOffset + 1] << 4);
            }
        }
        
        // Zurück zu LittleFS schreiben
        File f = LittleFS.open("/config.json", "w");
        f.print(jsonContent);
        f.close();
        
        DEBUG_PRINTF("Config zurück gespeichert: %d bytes\n", size);
    }
}

// ============================================================================
// CONFIG LADEN
// ============================================================================

void loadConfig() {
    DEBUG_PRINTLN("=== Config laden ===");
    
    if (!LittleFS.begin()) {
        DEBUG_PRINTLN("FEHLER: LittleFS mount fehlgeschlagen!");
        return;
    }
    
    if (!LittleFS.exists("/config.json")) {
        DEBUG_PRINTLN("FEHLER: /config.json nicht gefunden!");
        return;
    }
    
    File file = LittleFS.open("/config.json", "r");
    if (!file) {
        DEBUG_PRINTLN("FEHLER: Konnte config.json nicht öffnen!");
        return;
    }
    
    // JSON parsen (max 8KB für größere configs)
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        DEBUG_PRINT("JSON Parse Fehler: ");
        DEBUG_PRINTLN(error.c_str());
        return;
    }
    
    // Mappings einlesen
    JsonArray maps = doc["mappings"];
    mappingCount = 0;
    
    for (JsonObject map : maps) {
        if (mappingCount >= MAX_MAPPINGS) break;
        
        MIDIMapping &m = mappings[mappingCount];
        
        m.active = true;
        m.type = map["type"].as<String>();
        m.channel = map["channel"] | 0;
        m.note_or_cc = map.containsKey("note") ? map["note"].as<byte>() : map["cc"].as<byte>();
        m.pin = map["pin"];
        m.duration = map["duration"] | 30;
        m.toggle_state = false;
        
        // Action Type parsen
        String action = map["action"] | "pulse";
        if (action == "pulse") m.action = ACTION_PULSE;
        else if (action == "toggle") m.action = ACTION_TOGGLE;
        else if (action == "pwm") m.action = ACTION_PWM;
        else if (action == "gate") m.action = ACTION_GATE;
        else m.action = ACTION_PULSE;
        
        // Pin konfigurieren
        pinMode(m.pin, OUTPUT);
        digitalWrite(m.pin, LOW);
        
        DEBUG_PRINTF("Mapping %d: %s Note/CC=%d Pin=%d Action=%d\n", 
                      mappingCount, m.type.c_str(), m.note_or_cc, m.pin, m.action);
        
        mappingCount++;
    }
    
    DEBUG_PRINTF("=== %d Mappings geladen ===\n", mappingCount);
}

// ============================================================================
// MIDI HANDLER
// ============================================================================

void handleNoteOn(byte channel, byte note, byte velocity) {
    // Durchsuche Mappings
    for (int i = 0; i < mappingCount; i++) {
        MIDIMapping &m = mappings[i];
        
        if (!m.active) continue;
        if (m.type != "note_on") continue;
        if (m.channel != 0 && m.channel != channel) continue;
        if (m.note_or_cc != note) continue;
        
        // Mapping gefunden!
        switch (m.action) {
            case ACTION_PULSE:
                digitalWrite(m.pin, HIGH);
                pulseTimes[i] = millis();
                break;
                
            case ACTION_TOGGLE:
                m.toggle_state = !m.toggle_state;
                digitalWrite(m.pin, m.toggle_state ? HIGH : LOW);
                break;
                
            case ACTION_GATE:
                digitalWrite(m.pin, HIGH);
                break;
                
            case ACTION_PWM:
                analogWrite(m.pin, map(velocity, 0, 127, 0, 255));
                break;
        }
    }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
    for (int i = 0; i < mappingCount; i++) {
        MIDIMapping &m = mappings[i];
        
        if (!m.active) continue;
        if (m.channel != 0 && m.channel != channel) continue;
        if (m.note_or_cc != note) continue;
        
        // Gate-Mode: Bei Note Off ausschalten
        if (m.action == ACTION_GATE) {
            digitalWrite(m.pin, LOW);
        }
    }
}

void handleControlChange(byte channel, byte cc, byte value) {
    for (int i = 0; i < mappingCount; i++) {
        MIDIMapping &m = mappings[i];
        
        if (!m.active) continue;
        if (m.type != "control_change") continue;
        if (m.channel != 0 && m.channel != channel) continue;
        if (m.note_or_cc != cc) continue;
        
        // PWM mit CC-Wert
        if (m.action == ACTION_PWM) {
            analogWrite(m.pin, map(value, 0, 127, 0, 255));
        }
    }
}

void handleSysEx(byte *data, unsigned size) {
    // Config Mode Befehl: F0 7D 47 43 01 F7
    if (size == 4 && 
        data[0] == 0x7D && 
        data[1] == 0x47 && 
        data[2] == 0x43 && 
        data[3] == 0x01) {
        
        DEBUG_PRINTLN("SysEx: Config Mode aktiviert!");
        
        // Flag setzen
        LittleFS.begin();
        File f = LittleFS.open("/.config_mode", "w");
        f.write("1");
        f.close();
        
        // LED-Feedback: 10x schnell blinken
        for (int i = 0; i < 10; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(50);
            digitalWrite(LED_BUILTIN, LOW);
            delay(50);
        }
        
        // Neustart
        rp2040.reboot();
    }
}

// ============================================================================
// USB MASS STORAGE MODE
// ============================================================================

void startUSBStorageMode() {
    pinMode(LED_BUILTIN, OUTPUT);
    
    // LittleFS initialisieren
    if (!LittleFS.begin()) {
        LittleFS.format();
        LittleFS.begin();
    }
    
    DEBUG_PRINTLN("\n=== USB MASS STORAGE MODE ===");
    DEBUG_PRINTLN("Erstelle FAT12 RAMDisk...");
    
    // FAT12 Image mit config.json erstellen
    createFAT12Image();
    
    DEBUG_PRINTLN("Starte USB Mass Storage Device...");
    
    // USB Mass Storage Device konfigurieren
    usb_msc.setID("Raspberry", "Pico FS", "1.0");
    usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
    usb_msc.setCapacity(DISK_BLOCK_NUM, DISK_BLOCK_SIZE);
    usb_msc.setUnitReady(true);
    usb_msc.begin();
    
    DEBUG_PRINTLN("\n*** Pico erscheint jetzt als USB-Laufwerk ***");
    DEBUG_PRINTLN("LED blinkt langsam im Config-Modus...\n");
    
    // LED blinkt langsam = Config Mode aktiv
    unsigned long lastSave = 0;
    while (true) {
        // Config speichern alle 5 Sekunden
        if (millis() - lastSave > 5000) {
            saveFAT12ToLittleFS();
            lastSave = millis();
        }
        
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
    }
}

// ============================================================================
// MIDI MODE
// ============================================================================

void startMIDIMode() {
    DEBUG_PRINTLN("\n=== MIDI MODE ===");
    
    pinMode(LED_BUILTIN, OUTPUT);
    
    // Config laden
    loadConfig();
    
    // USB MIDI Setup
    TinyUSBDevice.setManufacturerDescriptor("Glockenspiel");
    TinyUSBDevice.setProductDescriptor("MIDI Controller");
    usbMidi.begin();
    MIDI_USB.begin(MIDI_CHANNEL_OMNI);
    MIDI_USB.turnThruOff();
    
    // MIDI Handler registrieren
    MIDI_USB.setHandleNoteOn(handleNoteOn);
    MIDI_USB.setHandleNoteOff(handleNoteOff);
    MIDI_USB.setHandleControlChange(handleControlChange);
    MIDI_USB.setHandleSystemExclusive(handleSysEx);
    
    // Hardware MIDI Setup (GP0/GP1)
    Serial1.setTX(0);
    Serial1.setRX(1);
    MIDI_HW.begin(MIDI_CHANNEL_OMNI);
    MIDI_HW.turnThruOff();
    
    MIDI_HW.setHandleNoteOn(handleNoteOn);
    MIDI_HW.setHandleNoteOff(handleNoteOff);
    MIDI_HW.setHandleControlChange(handleControlChange);
    
    // Bereit-Signal: 3x blinken
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
    }
    
    DEBUG_PRINTLN("=== BEREIT ===");
}

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    // DEBUGGING: Wenn du Serial Monitor brauchst, setze DEBUG_MODE in platformio.ini
    #ifdef DEBUG_MODE
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n\n========================================");
    Serial.println("Glockenspiel MIDI Controller v1.0");
    Serial.println("========================================");
    #endif
    
    // Config-Button prüfen
    pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);
    delay(10);
    bool buttonPressed = (digitalRead(CONFIG_BUTTON_PIN) == LOW);
    
    #ifdef DEBUG_MODE
    Serial.print("Button (GP22) Status: ");
    Serial.println(buttonPressed ? "GEDRUECKT (LOW)" : "NICHT GEDRUECKT (HIGH)");
    Serial.print("LittleFS mounten... ");
    #endif
    
    if (!LittleFS.begin()) {
        #ifdef DEBUG_MODE
        Serial.println("FEHLGESCHLAGEN - formatiere...");
        #endif
        LittleFS.format();
        LittleFS.begin();
    }
    
    #ifdef DEBUG_MODE
    Serial.println("OK");
    #endif
    
    bool configFlag = LittleFS.exists("/.config_mode");
    
    #ifdef DEBUG_MODE
    Serial.print("Config-Flag: ");
    Serial.println(configFlag ? "GESETZT" : "NICHT GESETZT");
    #endif
    
    if (configFlag) {
        #ifdef DEBUG_MODE
        Serial.println("Loesche Config-Flag...");
        #endif
        LittleFS.remove("/.config_mode");
    }
    
    // Entscheidung: Config oder MIDI Mode
    #ifdef DEBUG_MODE
    Serial.println();
    #endif
    
    if (buttonPressed) {
        #ifdef DEBUG_MODE
        Serial.println(">>> GRUND: Button gedrueckt");
        #endif
        startUSBStorageMode();
    } else if (configFlag) {
        #ifdef DEBUG_MODE
        Serial.println(">>> GRUND: Config-Flag gesetzt");
        #endif
        startUSBStorageMode();
    } else {
        #ifdef DEBUG_MODE
        Serial.println(">>> Starte MIDI Mode");
        #endif
        startMIDIMode();
    }
}

// ============================================================================
// LOOP
// ============================================================================

void loop() {
    // MIDI Messages verarbeiten
    if (MIDI_USB.read()) {
        // Weiterleitung an Hardware TX
        auto type = MIDI_USB.getType();
        auto channel = MIDI_USB.getChannel();
        auto data1 = MIDI_USB.getData1();
        auto data2 = MIDI_USB.getData2();
        
        switch(type) {
            case midi::NoteOn:
                MIDI_HW.sendNoteOn(data1, data2, channel);
                break;
            case midi::NoteOff:
                MIDI_HW.sendNoteOff(data1, data2, channel);
                break;
            case midi::ControlChange:
                MIDI_HW.sendControlChange(data1, data2, channel);
                break;
        }
    }
    
    // Hardware MIDI verarbeiten
    MIDI_HW.read();
    
    // Pulse-Timings prüfen (für ACTION_PULSE)
    unsigned long now = millis();
    for (int i = 0; i < mappingCount; i++) {
        if (mappings[i].action == ACTION_PULSE && pulseTimes[i] > 0) {
            if (now - pulseTimes[i] >= mappings[i].duration) {
                digitalWrite(mappings[i].pin, LOW);
                pulseTimes[i] = 0;
            }
        }
    }
}
