#ifndef arduinoMavlink_h
#define arduinoMavlink_h

#include "Arduino.h"
#include "mavlink/common/mavlink.h" // Mavlink interface

void adsbVehicle(int ICAO_address_, int lat_, int long_, String altitude_type_, int altitude_, int heading_, int hor_velocity_, int ver_velocity_, String callsign_, int age_, int squawk_, int emitter_type1);
void uploadGeoFenceCircle(int lat_, int long_, int radius_);
void mission_count(int count_, int system_id, int component_id);
int listingForMissionItems();
bool listingForMissionAck();

#endif