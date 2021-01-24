#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>
#include <BikeBLE.h>
#include <HCSR04.h>
#include <LED.h>
#include <Wire.h>
#include <LIDARLite_v4LED.h>


LIDARLite_v4LED myLidarLite;
uint8_t lidarBusyFlag = 0;


unsigned long currentMillis = 0;
unsigned long previousMillisHCSR = 0;
unsigned long previousMillisUltraSonic = 0;
unsigned long previousMillisLidar = 0;
unsigned long previousMillisBusyWait = 0;
unsigned long previousMilliBlinking = 0;
unsigned long blinkingSinceMilli = 0;

unsigned long braking = 0;
int ultraSonicTriggerPin = 11;
int ultraSonicEchoPin = 12;
int backLightPin = 10;
int sideLightPin = 9;
bool laserBlinking = false;
bool laserOn = false;
double distance = 0;
int blinkDuration = 3000;
UltraSonicDistanceSensor distanceLeftSensor(ultraSonicTriggerPin, ultraSonicEchoPin);
volatile unsigned long LastPulseTime = 0;
uint16_t lidarDistance = 0;


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
    pinMode(sideLightPin, OUTPUT);
    pinMode(backLightPin, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(12), EchoPinISR, CHANGE);


    // LIDAR SETUP: Initialize Arduino I2C (for communication to LidarLite)
    Wire.begin();
#ifdef FAST_I2C
    #if ARDUINO >= 157
            Wire.setClock(400000UL); // Set I2C frequency to 400kHz (for Arduino Due)
        #else
            TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
        #endif
#endif

    bool tempInit = HTS.begin();
    if (!tempInit) {
        Serial.println("failed to initialize temperature sensor");
    }
}
bool distanceSingle(uint16_t * distance) {
    lidarBusyFlag = myLidarLite.getBusyFlag();
    if (!lidarBusyFlag) {
        *distance = myLidarLite.readDistance();
        myLidarLite.takeRange();
        return true;
    }
    return false;
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
             if (currentMillis - previousMillisBusyWait >= 2000) {
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
                //Serial.printf("d: %f, bb: %lu, cm: %lu, bl: %d \n", distance, blinkingSinceMilli+blinkDuration, currentMillis, laserBlinking);
                previousMillisUltraSonic = currentMillis;
                distance = distanceLeftSensor.measureDistanceCm(LastPulseTime);
                if (distance <= 150 && distance != -1) {
                    laserBlinking = true;
                    blinkingSinceMilli = currentMillis;
                }
                if (currentMillis >= blinkingSinceMilli + blinkDuration) {
                    laserBlinking = false;
                }
            }
            if (currentMillis - previousMillisLidar >= 100) {
                previousMillisLidar = currentMillis;
                if (distanceSingle(&lidarDistance)) {
                }

            }
            if (currentMillis - previousMilliBlinking >= 500) {
                Serial.printf("braking: %lu \n", BikeBLE::readBraking());
                previousMilliBlinking = currentMillis;
                if (laserBlinking) {
                    if (laserOn) {
                        laserOn = !laserOn;
                        digitalWrite(sideLightPin, LOW);
                    } else {
                        laserOn = !laserOn;
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


