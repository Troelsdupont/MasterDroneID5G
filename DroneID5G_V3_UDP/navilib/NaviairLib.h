/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef NaviairLib_h
#define NaviairLib_h

#include "Arduino.h"

class NaviairLib
{
public:
    NaviairLib(String _id, String _apiKey, String _altitudeRef, String _altitudeUnit);

    // Functions


    // Variables:
    long timestamp;
    float latitude;
    float longitude;
    float speed;
    float altitude;
    String altitudeRef; // Ny
    String altitudeUnit; // Ny
    float heading;
    float flightTime;

    float pDop; // Ny
    float hDop; // Ny
    float vDop; // Ny

private:
    String iD;

protected:
    String apiKey;
};

#endif
