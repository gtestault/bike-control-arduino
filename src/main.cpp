#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>
#include <BikeBLE.h>


const PinName GREEN = PinName::p16;
const PinName RED = PinName::p24;
const PinName BLUE = PinName::p6;



unsigned long previousMillis = 0;

void setup() {
    Serial.begin(9600);
    BikeBLE::setupBLE();

    //LED INIT
    pinMode(RED, OUTPUT);
    pinMode(BLUE, OUTPUT);
    pinMode(GREEN, OUTPUT);
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);

    bool tempInit = HTS.begin();
    if (!tempInit) {
        Serial.println("failed to initialize temperature sensor");
    }
}



void loop() {
    // wait for a BLE central
    BLEDevice central = BLE.central();
    // if a central is connected to the peripheral:
    digitalWrite(RED, LOW);
    digitalWrite(BLUE, HIGH);
    if (central) {
        digitalWrite(RED, HIGH);
        digitalWrite(BLUE, LOW);
        Serial.print("Connected to central: ");
        // print the central's BT address:
        Serial.println(central.address());
        // turn on the LED to indicate the connection:

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
        Serial.print("Disconnected from central: ");
        Serial.println(central.address());
    }
}