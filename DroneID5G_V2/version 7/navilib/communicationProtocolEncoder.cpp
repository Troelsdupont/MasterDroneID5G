#include <string.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <cstdint>


using std::cout; using std::endl;
using std::string; using std::hex;
using std::stringstream;

string serialNumber = "AS1234";
float verticalSpeed = 5.1;
float latitude = -81.1989298;
float longitude = -151.1989298;
float AMSL = 10.5;
float HDOP = 0; 
float VDOP = 0; 
int timestamp = 1644843650; 

string encoder(string serialNum, float verticalSpe, float lat, float longi, float AMSL1, float Hdop, float Vdop, int timeStamp1)
{
    string outputStr = "HEEJ"; 
    int8_t EncodedVerticalSpeed = verticalSpe/0.5; 
    int32_t EncodedLatitude = lat*10e7;
    int32_t EncodedLongitude = longi*10e7;
    uint16_t EncodedAMSL = (AMSL1/0.5)+1000; 
    
    return outputStr; 
}

int main() {
    printf("Hello world\n");
    string output = encoder(serialNumber, verticalSpeed,latitude,longitude, AMSL, HDOP, VDOP, timestamp);
    cout << "string: " << output << endl;
}