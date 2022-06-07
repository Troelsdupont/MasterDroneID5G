#include "SaraR5.h"
#include "ArduinoMavlink.h"

String id = "ab1234"; // ID of the drone


DroneID5GLib* droneID = new DroneID5GLib(id);
SaraR5 saraR5(droneID);

void setup() {
    saraR5.init();
}
 
 
void loop() {
    saraR5.main();
}
