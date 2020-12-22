#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>
#include <BikeBLE.h>

long previousMillis = 0;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    BikeBLE::setupBLE();

    bool tempInit = HTS.begin();
    if (!tempInit) {
        Serial.println("failed to initialize temperature sensor");
    }
}



void loop() {
    float temp = HTS.readTemperature(CELSIUS);
    Serial.println(temp);
    delay(1000);
    // wait for a BLE central
    BLEDevice central = BLE.central();
    // if a central is connected to the peripheral:
    if (central) {
        Serial.print("Connected to central: ");
        // print the central's BT address:
        Serial.println(central.address());
        // turn on the LED to indicate the connection:
        digitalWrite(LED_BUILTIN, HIGH);

        // check the battery level every 200ms
        // while the central is connected:
        while (central.connected()) {
            long currentMillis = millis();
            if (currentMillis - previousMillis >= 200) {
                previousMillis = currentMillis;
                updateMeasuredDistance();
            }
        }
        // when the central disconnects, turn off the LED:
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print("Disconnected from central: ");
        Serial.println(central.address());
    }
}

