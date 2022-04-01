/*
  naviairLib.cpp - Library for sending HTTP commands to Naviair.

*/

#include "DroneID5GLib.h"
//#include "SoftwareSerial.h"
//SoftwareSerial debug(9,8); // RX , TX

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



void DroneID5GLib::DecodeVehicle(String inputString)
{
  //ab1234800002104e58f06324815080f070c51e
  int vehicleCounter = inputString.length()/48; 
  
  SerialUSB.println("vehicleCounter " + String(vehicleCounter)); //Korrekt   

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

        String aircraftType = flagsBinary.substring(0,2);
        if(aircraftType == "00")
        {
          aircraftType = "Unknown";
        }
        else if(aircraftType == "01")
        {
          aircraftType = "Drone";
        }
        else if(aircraftType == "10")
        {
          aircraftType = "Aircraft";
        }
        else if(aircraftType == "11")
        {
          aircraftType = "Glider";
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
        altitude_ = (altitude_*0.5)-1000;

        // Vertical Speed -- NOT valid ----------------------------------------------
        int ver_velocity_ = 0; 
        
        // CALLSIGN - NOT VALID ----------------------------------------------------    
        String callsign_ = ""; 

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
        SerialUSB.println("Latitude int " + String(lat_) + " LONG INT : " + String(long_) + " FLAGS BIN " + flagsBinary + " AircraftType : " + aircraftType + " altitude_type_ " + altitude_type_ + " heading_ " + String(heading_)); //
        SerialUSB.println("hor_velocity_" + String(hor_velocity_) + " Altitude "  + String(altitude_) + " AGE " + String(age_) ); 

        // Transfer the Aircraft info to the PX4 
        // adsbVehicle(int ICAO_address_, int lat_, int long_, String altitude_type_, int altitude_, int heading_, int hor_velocity_, int ver_velocity_, String callsign_, int age_, int squawk_);
      
      }

  }

  
  
  


  


  
}



