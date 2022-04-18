/*
  arduinoMavlink.cpp - Library for communication with flight controller through Mavlink.
*/

#include "ArduinoMavlink.h"

void adsbVehicle(int ICAO_address_, int lat_, int lon_, String altitude_type_, int altitude_, int heading_, int hor_velocity_, int ver_velocity_, String callsign_, int age_, int squawk_, int emitter_type1){
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

    uint8_t emitter_type = emitter_type1; 

    uint8_t tslc = age_;
    uint16_t flags = ADSB_FLAGS_VALID_COORDS + ADSB_FLAGS_VALID_ALTITUDE + ADSB_FLAGS_VALID_HEADING + ADSB_FLAGS_VALID_CALLSIGN + ADSB_FLAGS_VALID_VELOCITY;
    uint16_t squawk = 0;

    // Pack the message
    mavlink_msg_adsb_vehicle_pack(system_id, compont_id, &msg,ICAO_address,lat,lon,altitude_type,altitude,heading,hor_velocity,ver_velocity,callsign,emitter_type,tslc,flags,squawk);

    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    Serial.write(buf, len);

}


void uploadGeoFenceCircle(int lat_, int long_, int radius){
  SerialUSB.println("Uploading geofence circle...");

  uint8_t system_id = 44;
  uint8_t component_id = 44;

  uint8_t _target_system = 1; // Pixhawk id
  uint8_t _target_component = 0; // Pixhawk component id, 0 = all (seems to work fine)

  int count = 1; 

  // Send count
  mission_count(count, system_id,component_id);

  bool success = false;
  // Loop:
  while (success == false){
    // Listing for mission request
    int request = listingForMissionItems();

    // Answer request
    if (request == 0){
      uint16_t seq = 0;
      uint8_t frame = MAV_FRAME_GLOBAL; // Set target frame to global default
      uint16_t command = MAV_CMD_NAV_FENCE_CIRCLE_INCLUSION; // Specific command for PX4
      uint8_t current = 0; // false:0, true:1 - When downloading, whether the item is the current mission item.
      uint8_t autocontinue = 0; // Always 0
      float param1 = radius; // Radius
      float param2 = 0; // Inclusion Group
      float param3 = 0; // Reserved
      float param4 = 0; // Reserved
      float x = lat_; // Latitude - degrees
      float y = long_; // Longitude - degrees
      float z = 0; // Reserved

      // Initialize the required buffers
      mavlink_message_t msg;
      uint8_t buf[MAVLINK_MAX_PACKET_LEN];

      uint8_t mission_type = MAV_MISSION_TYPE_FENCE;

      // Pack the message
      mavlink_msg_mission_item_int_pack(system_id, component_id, &msg, _target_system, _target_component, seq, frame, command, current, autocontinue, param1, param2, param3, param4, x, y, z, mission_type);
      //uint16_t mavlink_msg_mission_item_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, uint8_t target_system, uint8_t target_component, uint16_t seq, uint8_t frame, uint16_t command, uint8_t current, uint8_t autocontinue, float param1, float param2, float param3, float param4, float x, float y, float z
      
      // Copy the message to the send buffer
      uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
      
      // Send the message (.write sends as bytes)
      Serial.write(buf, len);
    }

    // Listing mission ack
    if(listingForMissionAck()){
      success = true;
    }

  }
    
  SerialUSB.println("Finish uploading geofence");
}

void mission_count(int count_, int system_id, int component_id){
  uint8_t _target_system = 1; // Pixhawk id
  uint8_t _target_component = 0; // Pixhawk component id, 0 = all (seems to work fine)
  uint8_t mission_type = MAV_MISSION_TYPE_FENCE;

  uint16_t count = count_; // How many items to upload

  // Initialize the required buffers
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  // Pack the message
  mavlink_msg_mission_count_pack(system_id, component_id, &msg, _target_system, _target_component, count, mission_type);
  
  // Copy the message to the send buffer
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
  
  // Send the message (.write sends as bytes)
  Serial.write(buf, len);
}

int listingForMissionItems(){
  mavlink_message_t msg;
  mavlink_status_t status;

  while(Serial.available()){
    uint8_t c = Serial.read();

    // Check for new mavlink message
    if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)){

      if(msg.msgid == MAVLINK_MSG_ID_MISSION_REQUEST_INT){
        mavlink_mission_request_int_t missionreq;
        mavlink_msg_mission_request_int_decode(&msg, &missionreq);

        return missionreq.seq;
      }
    }
  }
}

bool listingForMissionAck(){
  mavlink_message_t msg;
  mavlink_status_t status;
  while(Serial.available()){
    uint8_t c = Serial.read();

    // Check for new mavlink message
    if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)){
      
      if(msg.msgid == MAVLINK_MSG_ID_MISSION_ACK){
        mavlink_mission_ack_t missionack;
        mavlink_msg_mission_ack_decode(&msg, &missionack);

        if (missionack.type == MAV_MISSION_ACCEPTED){
          return true;
        } else {
          return false;
        }
      }
    }
  }

}

