/*
  naviairLib.cpp - Library for sending HTTP commands to Naviair.

*/

#include "NaviairLib.h"
//#include "SoftwareSerial.h"
//SoftwareSerial debug(9,8); // RX , TX

NaviairLib::NaviairLib(String _id, String _apiKey, String _altitudeRef, String _altitudeUnit){

    iD = _id;
    timestamp = 0;
    latitude = 0.0;
    longitude = 0.0;
    speed = 0.0;
    altitude = 0.0;
    heading = 0.0;
    flightTime = 0.0;

    pDop = 0.0;
    hDop = 0.0;
    vDop = 0.0;

    altitudeRef = _altitudeRef; // Possible values: "AGL" or "AMSL". Look in swagger
    altitudeUnit = _altitudeUnit; // Possible values: "m", "ft" or "FL". Look in swagger

    apiKey = _apiKey;
}