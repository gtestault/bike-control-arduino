//
// Created by Gabriel on 22.12.20.
//

#include "BikeBLE.h"

BLEService bikeService("bdb7d889-18b3-4342-b7d7-e3201e5fa3ef");
BLEFloatCharacteristic temperatureChar("f71b8d3f-eb1c-495f-9e61-b8a773f2867f", BLERead | BLENotify);
BLEFloatCharacteristic humidityChar("8bbb426f-c7a9-4add-8037-68d290fc3875", BLERead | BLENotify);
BLEDescriptor temperatureDescriptor("bdb7d889-18b3-4342-b7d7-e3201e5fa3ef", "Celsius");

void BikeBLE::setupBLE() {
    // begin initialization
    if (!BLE.begin()) {
        Serial.println("starting BikeBLE failed!");

        exit;
    }

    BLE.setLocalName("BikeControl");
    BLE.setAdvertisedService(bikeService); // add the service UUID
    bikeService.addCharacteristic(temperatureChar);
    bikeService.addCharacteristic(humidityChar);
    BLE.addService(bikeService);
    temperatureChar.addDescriptor(temperatureDescriptor);
    temperatureChar.writeValue(0); // set initial value for this characteristic

    /* Start advertising BikeBLE.  It will start continuously transmitting BikeBLE
       advertising packets and will be visible to remote BikeBLE central devices
       until it receives a new connection */

    // start advertising
    BLE.advertise();

    Serial.println("Bluetooth device active, waiting for connections...");
}

void BikeBLE::writeTemperature(float temperatureCelsius) {
    temperatureChar.writeValue(temperatureCelsius);
}

void BikeBLE::writeHumidity(float humidity) {
    humidityChar.writeValue(humidity);
}
