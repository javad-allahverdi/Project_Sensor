#include <SoftwareSerial.h>

#define RX_PIN 19  
#define TX_PIN 18 

class ZPHS01B {
private:
  SoftwareSerial sensorSerial;
  uint8_t buffer[25];

  uint8_t calculateChecksum(uint8_t *data) {
    uint8_t sum = 0;
    for (int i = 1; i < 24; i++) {
      sum += data[i];
    }
    return (~sum) + 1;
  }

  bool validateChecksum(uint8_t *data) {
    return data[24] == calculateChecksum(data);
  }

public:
  ZPHS01B(uint8_t rx, uint8_t tx) : sensorSerial(rx, tx) {}

  void begin(long baudRate) {
    sensorSerial.begin(baudRate);
  }

  bool readData() {
    if (sensorSerial->available() >= 25) {
      for (int i = 0; i < 25; i++) {
        buffer[i] = sensorSerial->read();
      }
      return validateChecksum(buffer);
    }
    return false;
  }

  int getPM1_0() { return (buffer[2] * 256 + buffer[3]); }

  int getPM2_5() { return (buffer[4] * 256 + buffer[5]); }

  int getPM10() { return (buffer[6] * 256 + buffer[7]); }

  int getCO2() { return (buffer[8] * 256 + buffer[9]); }

  int getVOC() { return buffer[10]; }

  float getTemperature() { return ((buffer[11] * 256 + buffer[12]) - 500) * 0.1; }

  int getHumidity() { return (buffer[13] * 256 + buffer[14]); }

  float getCH2O() { return (buffer[15] * 256 + buffer[16]) * 0.001; }

  float getCO() { return (buffer[17] * 256 + buffer[18]) * 0.1; }

  float getO3() { return (buffer[19] * 256 + buffer[20]) * 0.01; }

  float getNO2() { return (buffer[21] * 256 + buffer[22]) * 0.01; }

  void printData() {
    Serial.print("PM1.0: "); Serial.print(getPM1_0()); Serial.println(" ug/m3");
    Serial.print("PM2.5: "); Serial.print(getPM2_5()); Serial.println(" ug/m3");
    Serial.print("PM10: "); Serial.print(getPM10()); Serial.println(" ug/m3");
    Serial.print("CO2: "); Serial.print(getCO2()); Serial.println(" ppm");
    Serial.print("VOC: "); Serial.print(getVOC()); Serial.println(" grade");
    Serial.print("Temperature: "); Serial.print(getTemperature()); Serial.println(" °C");
    Serial.print("Humidity: "); Serial.print(getHumidity()); Serial.println(" %RH");
    Serial.print("CH2O: "); Serial.print(getCH2O()); Serial.println(" mg/m3");
    Serial.print("CO: "); Serial.print(getCO()); Serial.println(" ppm");
    Serial.print("O3: "); Serial.print(getO3()); Serial.println(" ppm");
    Serial.print("NO2: "); Serial.print(getNO2()); Serial.println(" ppm");
    Serial.println("-----------------------------");
  }
};

ZPHS01B airSensor(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(9600);
  airSensor.begin(9600);
  Serial.println("Initialized");
}

void loop() {
  if (airSensor.readData()) {
    airSensor.printData();
  } else {
    Serial.println("data error or not received.");
  }
  delay(2000);
}

