#include "SaraR5.h"

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
    saraR5.main();
    saraR5.printDroneID();
}
