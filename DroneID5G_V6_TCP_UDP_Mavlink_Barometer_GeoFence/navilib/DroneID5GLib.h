/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef DroneID5GLib_h
#define DroneID5GLib_h

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include "Arduino.h"

#define BMP_SCK  (24)//(13) //17 // D13
#define BMP_MISO (22)//(12) //19 //D12
#define BMP_MOSI (23)//(11) //16 //D11
#define BMP_CS   (9)//(10) //18 //D10 

class DroneID5GLib
{
public:
    DroneID5GLib(String _id);

    // Functions
    void DecodeVehicle(String inputString);
    void BarometerRead();
    void BarometerSetup(); 
    int HexDigit(char c);
    char HexByte(char *p);

    void uploadGeoFence();

    // Variables:
    long timestamp;
    float latitude;
    float longitude;
    float speed;
    float altitude;
    float heading;
    float flightTime;
    float AGL_Baro; 
    float AMSL_Baro; 
    float SeaLevelPressure;

    float pDop; // Ny
    float hDop; // Ny
    float vDop; // Ny

    float HATBaro = 0.0; // Height above takeoff
    float initialHeight = 0.0; // Initial height read of barometer 


private:
    String iD;
    

};

#endif
