//
// Created by Gabriel on 22.01.21.
//

#include <Arduino.h>
#ifndef BIKE_LED_H
#define BIKE_LED_H


class LED {
    static const PinName GREEN = PinName::p16;
    static const PinName RED = PinName::p24;
    static const PinName BLUE = PinName::p6;
public:
    static void init();
    static void setRed();
    static void setBlue();
    static void setGreen();
    static void setWhite();
};


#endif //BIKE_LED_H
