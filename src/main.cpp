#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>
#include <BikeBLE.h>

unsigned long previousMillis = 0;

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
    // wait for a BLE central
    BLEDevice central = BLE.central();
    // if a central is connected to the peripheral:
    if (central) {
        Serial.print("Connected to central: ");
        // print the central's BT address:
        Serial.println(central.address());
        // turn on the LED to indicate the connection:
        digitalWrite(LED_BUILTIN, HIGH);

        while (central.connected()) {
            unsigned long currentMillis = millis();
            if (currentMillis - previousMillis >= 1000) {
                previousMillis = currentMillis;
                float temp = HTS.readTemperature(CELSIUS);
                float humidity = HTS.readHumidity();
                BikeBLE::writeTemperature(temp);
                BikeBLE::writeHumidity(humidity);
                Serial.println(temp);
            }
        }
        // when the central disconnects, turn off the LED:
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print("Disconnected from central: ");
        Serial.println(central.address());
    }
}


