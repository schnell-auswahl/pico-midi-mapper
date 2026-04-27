#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

// Zwei MIDI Instanzen: USB und Hardware Serial
Adafruit_USBD_MIDI usbMidi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMidi, MIDI_USB);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI_HW);

// Gemeinsame Handler für beide MIDI-Quellen
void handleNoteOn(byte channel, byte note, byte velocity) {
    // LED an (später: Solenoid triggern)
    digitalWrite(LED_BUILTIN, HIGH);
}

void handleNoteOff(byte channel, byte note, byte velocity) {
    // LED aus (später: Solenoid release)
    digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    
    // === USB MIDI Setup ===
    TinyUSBDevice.setManufacturerDescriptor("Glockenspiel");
    TinyUSBDevice.setProductDescriptor("Pico MIDI Solenoid");
    usbMidi.begin();
    MIDI_USB.begin(MIDI_CHANNEL_OMNI);
    
    // USB MIDI Handler
    MIDI_USB.setHandleNoteOn(handleNoteOn);
    MIDI_USB.setHandleNoteOff(handleNoteOff);
    
    // === Hardware MIDI Setup (Serial1 = GP0/GP1) ===
    Serial1.setTX(0);  // GP0 = MIDI Out
    Serial1.setRX(1);  // GP1 = MIDI In
    MIDI_HW.begin(MIDI_CHANNEL_OMNI);
    
    // Hardware MIDI Handler
    MIDI_HW.setHandleNoteOn(handleNoteOn);
    MIDI_HW.setHandleNoteOff(handleNoteOff);
    
    // 3x blinken = Ready
    for(int i = 0; i < 3; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
    }
}

void loop() {
    // USB MIDI empfangen
    if (MIDI_USB.read()) {
        // Alles was über USB kommt, an Serial1 weiterleiten
        auto type = MIDI_USB.getType();
        auto channel = MIDI_USB.getChannel();
        auto data1 = MIDI_USB.getData1();
        auto data2 = MIDI_USB.getData2();
        
        // Weiterleitung an zweiten Pico
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
    
    // Hardware MIDI empfangen (vom ersten Pico oder MIDI Keyboard)
    MIDI_HW.read();
}