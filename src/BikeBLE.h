//
// Created by Gabriel on 22.12.20.
//

#ifndef BIKE_BIKEBLE_H
#define BIKE_BIKEBLE_H

#include <ArduinoBLE.h>
#include <Arduino.h>


class BikeBLE {
public:
    static void setupBLE();
    static void writeTemperature(float temperatureCelsius);
    static void writeHumidity(float humidity);
    static void writeDistanceLeft(double distance);
    static bool readBraking();
};


#endif //BIKE_BIKEBLE_H
