/*
  arduinoMavlink.cpp - Library for communication with flight controller through Mavlink.
*/

#include "ArduinoMavlink.h"

void adsbVehicle(int ICAO_address_, int lat_, int lon_, String altitude_type_, int altitude_, int heading_, int hor_velocity_, int ver_velocity_, String callsign_, int age_, int squawk_){
    Serial.begin(57600);

    SerialUSB.println("ADS-B Vehicle");


    uint8_t system_id = 44;
    uint8_t compont_id = 44;


    // Initialize the required buffers
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];

    uint32_t ICAO_address = ICAO_address_;
    int32_t lat = lat_;
    int32_t lon = lon_;
    uint8_t altitude_type = ADSB_ALTITUDE_TYPE_PRESSURE_QNH;
    int32_t altitude = altitude_;
    uint16_t heading = heading_*100;
    uint16_t hor_velocity = hor_velocity_;
    int16_t ver_velocity = ver_velocity_;
    char callsign[9];
    callsign_.toCharArray(callsign,9);
    uint8_t emitter_type = ADSB_EMITTER_TYPE_UAV;
    uint8_t tslc = age_;
    uint16_t flags = ADSB_FLAGS_VALID_COORDS + ADSB_FLAGS_VALID_ALTITUDE + ADSB_FLAGS_VALID_HEADING + ADSB_FLAGS_VALID_CALLSIGN + ADSB_FLAGS_SIMULATED;
    uint16_t squawk = 0;

    // Pack the message
    mavlink_msg_adsb_vehicle_pack(system_id, compont_id, &msg,ICAO_address,lat,lon,altitude_type,altitude,heading,hor_velocity,ver_velocity,callsign,emitter_type,tslc,flags,squawk);

    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    Serial.write(buf, len);

}
