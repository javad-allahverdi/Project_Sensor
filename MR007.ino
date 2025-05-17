#include <Arduino.h>

class MR007{
  private:
    int pin;
    float vMin, vMax;
    float lelMax;
    String gasName;

  public:
    MR007(int p, float minV, float maxV, float maxLEL, String name) : pin(p), vMin(minV), vMax(maxV), lelMax(maxLEL), gasName(name) {}

    float readVoltage() {
      int sensorValue = analogRead(pin);
      return sensorValue * (5.0 / 1023.0);
    }

    float calculateLEL(float voltage) {
      float lel = ((voltage - vMin) / (vMax - vMin)) * 20.0;
      return lel > 0 ? lel : 0;
    }

    void checkLEL(float lel) {
      if (lel >= lelMax) {
        Serial.print("WARNING: ");
        Serial.print(gasName);
        Serial.println(" concentration at explosive level!");
      }
    }

    void displayStatus() {
      float voltage = readVoltage();
      float lel = calculateLEL(voltage);
      Serial.print(gasName);
      Serial.print(" Voltage: ");
      Serial.print(voltage);
      Serial.print(" V, LEL: ");
      Serial.print(lel);
      Serial.println("%");
      checkLEL(lel);
    }
};

MR007 methaneSensor(A0, 0.012, 0.045, 100.0, "Methane");
MR007 propaneSensor(A1, 0.010, 0.030, 100.0, "Propane");

void setup() {
  Serial.begin(9600);
  delay(2000);
}

void loop() {
  methaneSensor.displayStatus();
  propaneSensor.displayStatus();
  delay(1000);
}

