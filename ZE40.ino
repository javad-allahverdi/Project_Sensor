#include <SoftwareSerial.h>

#define RX_PIN 17  
#define TX_PIN 16 

class ZE40 {
private:
    SoftwareSerial sensorSerial;

    uint8_t calculateChecksum(uint8_t *data) {
        uint8_t sum = 0;
        for (uint8_t i = 1; i < 8; i++) sum += data[i];
        return (~sum) + 1;
    }

    bool validateChecksum(uint8_t *data) {
        return data[8] == calculateChecksum(data);
    }

public:
    ZE40TVOC(uint8_t rx, uint8_t tx) : sensorSerial(rx, tx) {}

    void begin(long baudRate) {
        sensorSerial.begin(baudRate);
    }

    void sendCommand(uint8_t command[]) {
        for (uint8_t i = 0; i < 9; i++) sensorSerial.write(command[i]);
    }

    bool receiveData(uint8_t *buffer) {
        uint32_t startTime = millis();
        uint8_t index = 0;
        while (index < 9 && millis() - startTime < 1000) {
            if (sensorSerial.available()) {
                buffer[index++] = sensorSerial.read();
            }
        }
        return index == 9;
    }

    float getPPM(uint8_t *data) {
        uint16_t ppb = (data[4] << 8) | data[5];
        return ppb / 1000.0;
    }

    void switchQA() {
        uint8_t command[9] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46};
        sendCommand(command);
    }

    void switchInitiative() {
        uint8_t command[9] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};
        sendCommand(command);
    }

    void readInitiative() {
        uint8_t buffer[9];

        switchInitiative();
        
        if (receiveData(buffer)) {
            if (validateChecksum(buffer)) {
                float ppm = getPPM(buffer);
                Serial.print("TVOC PPM: ");
                Serial.println(ppm);
            }
        }
    }

    

    void readQA() {
        uint8_t buffer[9];

        switchQA();

        uint8_t command[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
        sendCommand(command);
        
        if (receiveData(buffer)) {
            if (validateChecksum(buffer)) {
                float ppm = getPPM(buffer);
                Serial.print("TVOC PPM: ");
                Serial.println(ppm);
            }
        }
    }
};

ZE40TVOC sensor(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(9600);
  airSensor.begin(9600);
  Serial.println("Initialized");
}

void loop() {
    sensor.readInitiative();
    delay(1000);
}
