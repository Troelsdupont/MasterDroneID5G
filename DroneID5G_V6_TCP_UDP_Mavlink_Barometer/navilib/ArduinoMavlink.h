#ifndef arduinoMavlink_h
#define arduinoMavlink_h

#include "Arduino.h"
#include "mavlink/common/mavlink.h" // Mavlink interface

void adsbVehicle(int ICAO_address_, int lat_, int long_, String altitude_type_, int altitude_, int heading_, int hor_velocity_, int ver_velocity_, String callsign_, int age_, int squawk_);

#endif