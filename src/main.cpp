#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>
#include <BikeBLE.h>
#include <HCSR04.h>
#include <LED.h>



unsigned long currentMillis = 0;
unsigned long previousMillisHCSR = 0;
unsigned long previousMillisUltraSonic = 0;
unsigned long previousMillisBusyWait = 0;
unsigned long previousMilliBlinking = 0;
unsigned long blinkingSinceMilli = 0;
int ultraSonicTriggerPin = 11;
int ultraSonicEchoPin = 12;
int backLightPin = 10;
int sideLightPin = 9;
bool blinking = false;
bool on = false;
double distance = 0;
int blinkDuration = 3000;
UltraSonicDistanceSensor distanceLeftSensor(ultraSonicTriggerPin, ultraSonicEchoPin);
volatile unsigned long LastPulseTime = 0;


void EchoPinISR() {
    static unsigned long startTime;

    if (digitalRead(12)) // Gone HIGH
        startTime = micros();
    else  // Gone LOW
        LastPulseTime = micros() - startTime;
}

void setup() {
    Serial.begin(9600);
    BikeBLE::setupBLE();
    LED::init();
    attachInterrupt(digitalPinToInterrupt(12), EchoPinISR, CHANGE);



    bool tempInit = HTS.begin();
    if (!tempInit) {
        Serial.println("failed to initialize temperature sensor");
    }
}


void loop() {
    // wait for a BLE central
    LED::setRed();
    BLEDevice central = BLE.central();
    // if a central is connected to the peripheral:
    if (central) {
        Serial.print("Connected to central: ");
        // print the central's BT address:
        Serial.println(central.address());
        // turn on the LED to indicate the connection:
        LED::setWhite();

        previousMillisBusyWait = millis();
        while (central.connected()) {
             currentMillis = millis();
             if (currentMillis - previousMillisBusyWait >= 4000) {
                 LED::setBlue();
                 break;
             }
        }

        while (central.connected()) {
            currentMillis = millis();
            if (currentMillis - previousMillisHCSR >= 1000) {
                previousMillisHCSR = currentMillis;
                float temp = HTS.readTemperature(CELSIUS);
                float humidity = HTS.readHumidity();
                BikeBLE::writeTemperature(temp);
                BikeBLE::writeHumidity(humidity);
            }
            if (currentMillis - previousMillisUltraSonic >= 100) {
                previousMillisUltraSonic = currentMillis;
                distance = distanceLeftSensor.measureDistanceCm(LastPulseTime);
                if (distance <= 150 && distance != -1) {
                    blinking = true;
                    blinkingSinceMilli = currentMillis;
                }
                if (currentMillis >= blinkingSinceMilli + blinkDuration) {
                    blinking = false;
                }
            }
            if (currentMillis - previousMilliBlinking >= 500) {
                previousMilliBlinking = currentMillis;
                if (blinking) {
                    if (on) {
                        on = !on;
                        digitalWrite(sideLightPin, LOW);
                    } else {
                        on = !on;
                        digitalWrite(sideLightPin, HIGH);
                    }
                } else {
                    digitalWrite(sideLightPin, HIGH);
                }
            }
        }
        // when the central disconnects, turn off the LED:
        Serial.print("Disconnected from central: ");
        Serial.println(central.address());
    }
}