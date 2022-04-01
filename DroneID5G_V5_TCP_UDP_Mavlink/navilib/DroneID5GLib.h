/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef DroneID5GLib_h
#define DroneID5GLib_h

#include "Arduino.h"

class DroneID5GLib
{
public:
    DroneID5GLib(String _id);

    // Functions
    void DecodeVehicle(String inputString);

    // Variables:
    long timestamp;
    float latitude;
    float longitude;
    float speed;
    float altitude;
    float heading;
    float flightTime;

    float pDop; // Ny
    float hDop; // Ny
    float vDop; // Ny

private:
    String iD;


};

#endif
