#include "SaraR5.h"
#include "ArduinoMavlink.h"

String id = "1"; // ID of the drone

String apiKey = "1234";

String altitudeRef = "AMSL"; // altitude is meussured in reference to mean sea level
String altitudeUnit = "m"; // altitude is provided in meters

NaviairLib* droneID = new NaviairLib(id, apiKey, altitudeRef, altitudeUnit);
SaraR5 saraR5(droneID);

void setup() 
{
    saraR5.init();
    
}
 
 
void loop() 
{ 

    int ICAO_address = 1234;
    int lat = 550000000;
    int lon = 100000000;
    String alti_type = "QNH";
    int altitude = 10000;
    int heading = 45;
    int hor_velocity = 0;
    int ver_velocity = 0;
    String callSign = "DRONEID";
    int age = 1;
    int squawk = 0;

    adsbVehicle(ICAO_address,lat,lon,alti_type,altitude,heading,hor_velocity,ver_velocity,callSign,age,squawk);
    delay(500);

    ICAO_address = 12345;
    lat = 551000000;
    lon = 101000000;
    alti_type = "QNH";
    altitude = 20000;
    heading = 180;
    hor_velocity = 0;
    ver_velocity = 0;
    callSign = "DRONEID1";
    age = 1;
    squawk = 0;

    adsbVehicle(ICAO_address,lat,lon,alti_type,altitude,heading,hor_velocity,ver_velocity,callSign,age,squawk);

    
    delay(500);
    
    //saraR5.main();
    //saraR5.printDroneID();
}
