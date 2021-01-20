#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>
#include <BikeBLE.h>
#include <HCSR04.h>


const PinName GREEN = PinName::p16;
const PinName RED = PinName::p24;
const PinName BLUE = PinName::p6;



unsigned long previousMillisHCSR = 0;
unsigned long previousMillisUltraSonic = 0;
int ultraSonicTriggerPin = 11;
int ultraSonicEchoPin = 12;
UltraSonicDistanceSensor distanceLeftSensor(ultraSonicTriggerPin, ultraSonicEchoPin);


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
            if (currentMillis - previousMillisHCSR >= 1000) {
                previousMillisHCSR = currentMillis;
                float temp = HTS.readTemperature(CELSIUS);
                float humidity = HTS.readHumidity();
                BikeBLE::writeTemperature(temp);
                BikeBLE::writeHumidity(humidity);
            }
            if (currentMillis - previousMillisUltraSonic >= 1000) {
                previousMillisUltraSonic = currentMillis;
                double distance = distanceLeftSensor.measureDistanceCm();
                BikeBLE::writeDistanceLeft(distance);
            }
        }
        // when the central disconnects, turn off the LED:
        Serial.print("Disconnected from central: ");
        Serial.println(central.address());
    }
}