/*
  naviairLib.cpp - Library for sending HTTP commands to Naviair.

*/

#include "DroneID5GLib.h"
#include "ArduinoMavlink.h"
//#include "SoftwareSerial.h"
//SoftwareSerial debug(9,8); // RX , TX

//Barometer object 
Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);



DroneID5GLib::DroneID5GLib(String _id){

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

}

void DroneID5GLib::BarometerSetup()
{
  SerialUSB.println("Barometer Setup "); // 
  SerialUSB.println(F("BMP280 test"));
  unsigned status;
  //status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  status = bmp.begin();
  if (!status) {
    SerialUSB.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    SerialUSB.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    SerialUSB.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    SerialUSB.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    SerialUSB.print("        ID of 0x60 represents a BME 280.\n");
    SerialUSB.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  
  
  
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */


  for (int i=0; i <10; i++) {
    initialHeight = initialHeight + bmp.readAltitude(990); 
    SerialUSB.print(F("Approx altitude = "));
    SerialUSB.print(bmp.readAltitude(990)); /* Adjusted to local forecast! */
    SerialUSB.println(" m");
    delay(100); 
  }
  
  initialHeight = initialHeight/10; 
  SerialUSB.print(F("GNS altitude = "));
  SerialUSB.print(initialHeight); /* Adjusted to local forecast! */
  SerialUSB.println(" m");
  
}

void DroneID5GLib::BarometerRead()
{
  SerialUSB.println("Barometer Read "); // 

  // SerialUSB.print(F("Temperature = "));
  // SerialUSB.print(bmp.readTemperature());
  // SerialUSB.println(" *C");

  // SerialUSB.print(F("Pressure = "));
  // SerialUSB.print(bmp.readPressure());
  // SerialUSB.println(" Pa");

  // SerialUSB.print(F("Approx altitude = "));
  // SerialUSB.print(bmp.readAltitude(990)); /* Adjusted to local forecast! */
  // SerialUSB.println(" m");
  AGL = bmp.readAltitude(990)-initialHeight;
  SerialUSB.print(F("Height above takeoff = "));
  SerialUSB.print(AGL); 
  SerialUSB.println(" m");
}

int DroneID5GLib::HexDigit(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  return 0;
}
char DroneID5GLib::HexByte(char *p)
{
  char value = 0;

  value += HexDigit(*p++);

  if (*p != '\0')
  {
    value = value * 16 +  HexDigit(*p);
  }
  return value;
}

void DroneID5GLib::DecodeVehicle(String inputString)
{
  //ab1234800002104e58f06324815080f070c51e
  int vehicleCounter = inputString.length()/48; 
  
  SerialUSB.println("vehicleCounter " + String(vehicleCounter)); //

  String vehicleString = "";

  if(vehicleCounter<1)
  {
    SerialUSB.println("NO Flying vehicle to decode" + inputString); //
  }
  else
  {
    SerialUSB.println("Flying vehicle(s) to decode" + inputString);
  }


  for (int i = 0; i < vehicleCounter; i++) {
    vehicleString = inputString.substring(0+(48*i),48+(48*i));

    if(vehicleString.length()<1)
    {
      SerialUSB.println("No flying vehicle to decode" + vehicleString); //
    }
    else 
    {
      SerialUSB.println("Flying vehicle to decode" + vehicleString); //
    }

    if(vehicleString.length()>47)
      {

        String callSignString = vehicleString.substring(0,18);
        String flagsString = vehicleString.substring(18,21);
        String headingString = vehicleString.substring(21,23);
        String speedString = vehicleString.substring(23,25); 
        String latString  = vehicleString.substring(25,33);
        String longString = vehicleString.substring(33,41); 
        String altitudeString =vehicleString.substring(41,45);
        String timestampString = vehicleString.substring(45,48);
        
        // PRINT HEX VALUES 
        // SerialUSB.println("Callsign " + callSignString); //
        // SerialUSB.println("Flags " + flagsString); //
        // SerialUSB.println("Heading " + headingString); //
        // SerialUSB.println("Horizontal Speed " + speedString); //
        // SerialUSB.println("Latitude " + latString); //
        // SerialUSB.println("Longitude " + longString); //
        // SerialUSB.println("Altitude " + altitudeString); //
        // SerialUSB.println("Timestamp " + timestampString); //  
      

        int ICAO_address = 0; // IS SET TO ZERO, callsign is used instead 
        
        // LATITUDE -------------------------------------------------
        int str_len = latString.length() + 1; 
        char arrayChar[str_len];
        latString.toCharArray(arrayChar, str_len);
        signed int lat_ = strtol(arrayChar, NULL, 16);
        
        // LONGITUDE --------------------------------------------------
        str_len = longString.length() + 1; 
        arrayChar[str_len];
        longString.toCharArray(arrayChar, str_len);
        signed int long_ = strtol(arrayChar,NULL,16); 
        
        // FLAGS = 10011 ----------------------------------------
        str_len = flagsString.length() + 1; 
        arrayChar[str_len];
        flagsString.toCharArray(arrayChar, str_len);
        signed int flags_ = strtol(arrayChar,NULL,16); 
        String flagsBinary = String(flags_,BIN);

        while (flagsBinary.length() < 5){
          flagsBinary = "0" + flagsBinary;
        }

        uint8_t emitter_type1 = 0; 

        String aircraftType = flagsBinary.substring(0,2);
        if(aircraftType == "00")
        {
          aircraftType = "Unknown";
          emitter_type1 = ADSB_EMITTER_TYPE_NO_INFO;
        }
        else if(aircraftType == "01")
        {
          aircraftType = "Drone";
          emitter_type1 = ADSB_EMITTER_TYPE_UAV;
        }
        else if(aircraftType == "10")
        {
          aircraftType = "Aircraft";
          emitter_type1 = ADSB_EMITTER_TYPE_LARGE;
        }
        else if(aircraftType == "11")
        {
          aircraftType = "Glider";
          emitter_type1 = ADSB_EMITTER_TYPE_GLIDER;
        }


        String altitude_type_ = "";
        if(flagsBinary.substring(2,3) == "0")
        {
          altitude_type_ = "AMSL";
        }
        else
        {
          altitude_type_ = "AGL" ;
        }

        
        // HEADING --------------------------------------------------
        str_len = headingString.length() + 1; 
        arrayChar[str_len];
        headingString.toCharArray(arrayChar, str_len);
        signed int heading_ = strtol(arrayChar,NULL,16); 
        
        if(flagsBinary.substring(3,4) == "1")
        {
          heading_ = heading_ + 180; 
        }

        // Horizontal SPEED ------------------------------------- ---------------
        str_len = speedString.length() + 1; 
        arrayChar[str_len];
        speedString.toCharArray(arrayChar, str_len);
        signed int hor_velocity_ = strtol(arrayChar,NULL,16); 

        if(flagsBinary.substring(4,5) == "1")
        {
          hor_velocity_ = hor_velocity_*0.25; 
        }
        else 
        {
          hor_velocity_= (hor_velocity_*0.75) + (255*0.25); 
        }

        // ALTITUDE --------------------------------------------------
        str_len = altitudeString.length() + 1; 
        arrayChar[str_len];
        altitudeString.toCharArray(arrayChar, str_len);
        signed int altitude_ = strtol(arrayChar,NULL,16); 
        altitude_ = ((altitude_*0.5)-1000)*1000;

        // Vertical Speed -- NOT valid ----------------------------------------------
        int ver_velocity_ = 0; 
        
        // CALLSIGN IN ASCII - VALID FROM HEX STRING TO ASCII STRING ----------------------------------------------------    
        //String callSignString = vehicleString.substring(0,18);
        str_len = callSignString.length() + 1; 
        arrayChar[str_len];
        callSignString.toCharArray(arrayChar, str_len);
        String callsign_ = ""; 

        for (unsigned i = 0; i < str_len; i += 2)
        {
          //SerialUSB.print(HexByte(&arrayChar[i]));
          callsign_ = callsign_ + HexByte(&arrayChar[i]); 
        }

        while(callsign_[0]=='0')
        {
          callsign_ = callsign_.substring(1,callsign_.length());
        }
        // CALL SIGN in INT 
        String callSignSub = callSignString.substring(12,callSignString.length());
        
        str_len = callSignSub.length() + 1; 
        arrayChar[str_len];
        callSignSub.toCharArray(arrayChar, str_len);
        signed int callSignInt = strtol(arrayChar,NULL,16); 
        ICAO_address = callSignInt;

        

        // SQUAWK NOT VALID ----------------------------------
        int squawk_ = 0;

        // AGE - 
        str_len = timestampString.length() + 1; 
        arrayChar[str_len];
        timestampString.toCharArray(arrayChar, str_len);
        signed int age_ = strtol(arrayChar,NULL,16); 
        

        // String droneIDtimestamp = String(timestamp);
        // str_len = droneIDtimestamp.length() + 1; 
        // arrayChar[str_len];
        // droneIDtimestamp.toCharArray(arrayChar, str_len);
        // signed int droneIDtimestampInt = strtol(arrayChar,NULL,16); 
        
        age_ = abs(age_ - timestamp); 

        //PRINT ---------------------------------------------------------------------
        SerialUSB.println("Callsign "+ callsign_ + " Latitude int " + String(lat_) + " LONG INT : " + String(long_) + " FLAGS BIN " + flagsBinary + " AircraftType : " + aircraftType + " altitude_type_ " + altitude_type_ + " heading_ " + String(heading_)); //
        SerialUSB.println("hor_velocity_" + String(hor_velocity_) + " Altitude "  + String(altitude_) + " AGE " + String(age_) + " ICAO " + String(ICAO_address)); 

        // Transfer the Aircraft info to the PX4 
        adsbVehicle(ICAO_address, lat_, long_, altitude_type_, altitude_, heading_, hor_velocity_, ver_velocity_, callsign_, 1, squawk_, emitter_type1);
      
      }

  }
}

void DroneID5GLib::uploadGeoFence(){
  int lat = 554698380;
  int long_ = 103293140;
  int radius = 20;
  uploadGeoFenceCircle(lat, long_, radius);
}



