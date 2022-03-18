/*
  SaraR5.cpp - Library for sending HTTP commands to Naviair via u-blox SARA-R5 chip.

*/

#include "SaraR5.h"

//SoftwareSerial debug(9,8); // RX , TX
//SoftwareSerial debug(9,8); // RX , TX

/// SET SERIAL_BUFFER_SIZE 320 in RingBuffer.h

int commaList[UmetricCommas];

SaraR5::SaraR5(NaviairLib* _droneID){
    droneID = _droneID;
    GNSSStatus = 'V'; // Set to void

    rsrq = 255.0; // dB
    rsrp= 255; // dBm

    total_cell_reselections = "0";
    total_radioLinkLoss = "0";
    connEstablishAttemptCount = "0";
    connEstablishSuccessCount = "0";
    connEstablishFailureCount = "0";
    reestablishmentAttemptCount = "0";
    reestablishmentSuccessCount = "0";
    reestablishmentFailureCount = "0";
    HO_AttemptCount = "0";
    HO_SuccessCount = "0";
    HO_FailureCount = "0";

    PSDprofile = 0;
    HTTPprofile = 0;
    
    serialNumber = "AB1234";

    command = "{\"write_api_key\": \"RJ9P1U1PKXIYX8MS\",\"updates\": [{\"delta_t\": 0,\"field1\": 1.0,\"field2\": \"2.0";
    response = "{\"write_api_key\": \"RJ9P1U1PKXIYX8MS\",\"updates\": [{\"delta_t\": 0,\"field1\": 1.0,\"field2\": \"2.0";

   // tmElements_t my_time;  // time elements structure

    //              -- Thingsspeak API --
    //serverName = "api.thingspeak.com";
    

    //      NODE JS server 
    serverName = "genius-lte-m.sdu.dk";


    //serverName = "10.133.123.12:8080"

    //apiKey1 = "f88825c1e55047f9aedd7fe301a01202"; //apiKey1
    apiKey3 = "KC7KWV8YE4D7DBFR";  //ThingsSpeakAPI TestChannel2
    apiKey2 = "MI0OS6L1ZSU3FKC8"; //ThingsSpeakAPI MyFirstChannel
    apiKey1 = "RJ9P1U1PKXIYX8MS"; //ThingsspealAPI MySecondChannel


    // payload1 for JSON thingsSpeak
    payload1 = "{\"write_api_key\": \"RJ9P1U1PKXIYX8MS\",\"updates\": [{\"delta_t\": 0,\"field1\": 1.0,\"field2\": \"2.0\"}]}";
    payload2 = "{\"write_api_key\": \"RJ9P1U1PKXIYX8MS\",\"updates\": [{\"delta_t\": 0,\"field1\": 1.0,\"field2\": \"2.0\"}]}";
    payload3 = "{\"write_api_key\": \"RJ9P1U1PKXIYX8MS\",\"updates\": [{\"delta_t\": 0,\"field1\": 1.0,\"field2\": \"2.0\"}]}";
    
    // payload1 for naviAir, Dosent work as it is too long.
    //payload1 = "{\"position\": {\"position\": { \"lat\": 50,\"long\": 10, \"accuracy\": { \"value\": 0, \"unit\": \"m\"}}, \"altitude\": {\"value\": 0, \"ref\": \"AGL\", \"unit\": \"m\", \"accuracy\": {\"value\": 0,\"unit\": \"m\" } }, \"heading\": { \"value\": 0,\"unit\": \"degree\" }, \"timestamp\": 0, \"speed\": { \"x\": {\"value\": 0, \"unit\": \"m/s\" }, \"y\": { \"value\": 0,  \"unit\": \"m/s\" }, \"z\": {  \"value\": 0, \"unit\": \"m/s\"  } }},\"vehicleInfo\": { \"callSign\": \"string\", \"type\": \"AIRCRAFT\", \"ssrCode\": \"string\",";
}

void SaraR5::printDroneID() {

    if(GNSSStatus == 'A'){
        SerialUSB.print(droneID->timestamp); SerialUSB.println(" - Timestamp");
        SerialUSB.print(droneID->latitude,5); SerialUSB.println(" - Latitude");
        SerialUSB.print(droneID->longitude,5); SerialUSB.println(" - Longitude");
        SerialUSB.print(droneID->speed,1); SerialUSB.println(" - Speed");
        SerialUSB.print(droneID->heading,3); SerialUSB.println(" - Heading");
        SerialUSB.print(droneID->altitude,1); SerialUSB.println(" - Altitude");
        SerialUSB.print(droneID->pDop); SerialUSB.println(" - PDOP");
        SerialUSB.print(droneID->hDop); SerialUSB.println(" - HDOP");
        SerialUSB.print(droneID->vDop); SerialUSB.println(" - VDOP");

        SerialUSB.println("\n");
    }
    else
    {
        SerialUSB.print("No GPS fix \n" ) ; 
    }
}

void SaraR5::turnLED(char output) {
    digitalWrite(LED_BUILTIN, output);
}

void SaraR5::beginSerial()
{
    Serial1.begin(115200);
    //while (!Serial1) {  ;  }

    Serial1.setTimeout(10);

    SerialUSB.begin(115200);
   // while (!SerialUSB) {  ;  }

    SerialUSB.println("New upload!");
}

void SaraR5::waitForLTESignal() {
    SerialUSB.println("Waiting for operator connection...");
    int counter = 0;
    String response;
    while(counter < 300) {
        response = getOperatorInfo();
        counter++;
        SerialUSB.println(response);
        //Serial.println("Start of response:");
        //Serial.println(response);
        //Serial.println("End of response:");
        if(response.indexOf("TDC") > 0){ // Check if message contains "TDC"
            //Serial.println(response);
            SerialUSB.println("Connection to TDC network established!");
            break;
        }
        //Serial.println(counter);
    }
}

void SaraR5::waitForBoardOK() {
    int counter = 0;
    String response;
    while(counter < 20) {
        response = checkStatus();
        counter++;
        //Serial.println(response);
        if(response.indexOf("OK") > 0){ // Check if message contains "OK"
            SerialUSB.println("OK recieved! Board is powered and online.");
            break;
        }
        //Serial.println(counter);
    }
}

String SaraR5::checkStatus() {

    //serialWriteToSara.println("AT\r");
    Serial1.println("AT\r");
    String response = "no response";
    delay(2500);
    /*
    while (serialWriteToSara.available())
        Serial.write(serialWriteToSara.read());
    while (Serial.available())
        serialWriteToSara.write(Serial.read());
        */

    while (Serial1.available()){
        response = Serial1.readString();
    }
    return response;
}

void SaraR5::enableGPS() {
    String response = "no response";

    // Read GPS status
    Serial1.println("AT+UGPS?\r");
    delay(10);
    while (Serial1.available()){
        response = Serial1.readString();
    }
    SerialUSB.println(response);

    if(response.indexOf("1") > 0) // Check if message contains "OK"
        SerialUSB.println("GNSS module already on.");
    else{// Enable the GPS module
            SerialUSB.println("Turning on GNSS module...\r");
            Serial1.println("AT+UGPS=1,15,127\r");
            delay(2000);
            while (Serial1.available()){
                response = Serial1.readString();
            }
            if(response.indexOf("OK") > 0) // Check if message contains "OK"
                SerialUSB.println("OK recieved! GNSS module is on and using all GNSS systems.");
            else
                SerialUSB.println(response);
    }

    //Serial.println(response);

    // Enable storing of the last value of NMEA $RMC messages
    Serial1.println("AT+UGRMC=1\r");
    response = "no response";

    delay(10);
    while (Serial1.available()){
        response = Serial1.readString();
    }
    if(response.indexOf("OK") > 0) // Check if message contains "OK"
        SerialUSB.println("OK recieved! Storing of NMEA $RMC messages is enable.");

    // Enable storing of the last value of NMEA $GGA messages
    Serial1.println("AT+UGGGA=1\r");
    response = "no response";

    delay(10);
    while (Serial1.available()){
        response = Serial1.readString();
    }
    if(response.indexOf("OK") > 0) // Check if message contains "OK"
        SerialUSB.println("OK recieved! Storing of NMEA $GGA messages is enable.");
    //Serial.println(response);

    // Enable storing of the last value of NMEA $GSA messages
    Serial1.println("AT+UGGSA=1\r");
    response = "no response";

    delay(10);
    while (Serial1.available()){
        response = Serial1.readString();
    }
    if(response.indexOf("OK") > 0) // Check if message contains "OK"
        SerialUSB.println("OK recieved! Storing of NMEA $GSA messages is enable.");
    //Serial.println(response);
}


void SaraR5::enableCellInformation() {
    String response = "no response";

    Serial1.println("AT+UMETRIC=4,2048"); // enable read without tag and getting LTE UE STATS -> 12. bit to 1 = 2048
    delay(10);
    while (Serial1.available()){
        response = Serial1.readString();
    }
    //SerialUSB.println(response);

}

String SaraR5::getOperatorInfo() {
    Serial1.println("AT+COPS?\r");
    String response = "no response";
    delay(1000);
    while (Serial1.available()){
        response = Serial1.readString();
    }
    return response;
}

void SaraR5::checkSimCardId() {
    String response = "no response";


    SerialUSB.println("Sim Card Circuit Card ID:");


    Serial1.println("AT+CCID");

    response = "no response";
    delay(50);
    while (Serial1.available()){
        response = Serial1.readString();
    }
    SerialUSB.println(response);

}

void SaraR5::searchForNetworks() {
    String response = "no response";

    Serial1.println("AT+COPN");

    response = "no response";
    delay(100);
    while (Serial1.available()){
        response = Serial1.readString();
    }
    SerialUSB.println(response);

}



void SaraR5::getGPSPosition() {
    // https://www.sparkfun.com/datasheets/GPS/NMEA%20Reference%20Manual1.pdf
    
    int counterGPS = 0; 
    String RMCmessage = "";
    String GGAmessage = "";
    String GSAmessage = "";


    SerialUSB.println("Print 1 ");
    // In Sparkfun expemle uses +UGRMC
    Serial1.flush();
    delay(5);
    Serial1.println("AT+UGRMC?\r"); /// First RMC
    delay(5);
    while(!Serial1.available() && counterGPS<30){
        delay(10); 
        counterGPS = counterGPS + 1; 
    }
    if (Serial1.available())
    {
        RMCmessage = Serial1.readString();
    }
    SerialUSB.println("RMC : " + RMCmessage);
 

    Serial1.println("AT+UGGGA?\r"); /// Second GGA
    
    counterGPS = 0;
    while(!Serial1.available() && counterGPS<30){
        delay(10); 
        counterGPS = counterGPS + 1; 
    }
    if (Serial1.available())
    {
        GGAmessage = Serial1.readString();
    }
    SerialUSB.println("GGA " + GGAmessage);
    
    
    Serial1.println("AT+UGGSA?\r"); /// Thrid GSA
    counterGPS = 0; 
    while(!Serial1.available() && counterGPS<30){
        delay(10); 
        counterGPS = counterGPS + 1; 
    }
    if (Serial1.available())
    {
        GSAmessage = Serial1.readString();
    }
    SerialUSB.println("GSA " + GSAmessage);


    
    if(RMCmessage.indexOf("OK") > 0 && 1 > RMCmessage.indexOf("AT+UGRMC?") > -1){ // Check if message contains "OK"

            SerialUSB.println("index RMC " + String(RMCmessage.indexOf("AT+UGRMC?")));
            parseGRMCMessage(RMCmessage);
            SerialUSB.println("Print 3.3.1 ");

            if (GNSSStatus == 'A' && GGAmessage.indexOf("OK") > 0 && 1 > GGAmessage.indexOf("AT+UGGGA?") > -1)
                //SerialUSB.println("Len GGA " + String(GGAmessage.length()));
                parseGGGAMessage(GGAmessage);
                SerialUSB.println("Print 3.3.3 ");
            if (GNSSStatus == 'A' && GSAmessage.indexOf("OK") > 0 && 1 > GSAmessage.indexOf("AT+UGGSA?") > -1)
                //SerialUSB.println("Len GSA " + String(GSAmessage.length()));
                parseGGSAMessage(GSAmessage);
                SerialUSB.println("Print 3.3.4 ");

    } 
    else 
    {
        turnLED(LOW); /// Glemmer at slukke hvis timestamp ikke bliver opdateret
        SerialUSB.print("No GPS ");
    }

    
    //while (!Serial1.available()); /// Wait on data
    SerialUSB.println("Print 4 ");
}

void SaraR5::parseGRMCMessage(String &str) {

    //debug.println(str);
    SerialUSB.println("Start parseGRMCMessage");
    int commaPositions[RMCCommas];
    int index = 0;
    char c;
    for (int i = 0; i < str.length(); i++) {
        c = str[i];
        if (checkIfComma(c)){
            commaPositions[index] = i;
            index++;
        }
    }
    /*
    for(int i = 0; i < RMCCommas; i++)
    {
        int h = commaPositions[i];
        Serial.println(h);
        delay(50);
    }
     */
    c = str[commaPositions[2]+1];
    changeInGNSSFIX(c);

    //GNSSStatus = str[commaPositions[1]+1];
    //debug.print("GNSS fix status: "); debug.println(GNSSStatus);


    if (commaPositions[2] - commaPositions[1] > 1){ // Check if time is aviabele

        String time = str.substring(commaPositions[1]+1,commaPositions[2]);
        //Serial.println(time);

        if (GNSSStatus == 'A'){ // Check if the GNSS has an active

            String date = str.substring(commaPositions[9]+1,commaPositions[10]);

            setupTimestamp(time, date);

            turnLED(HIGH);

            String lat = str.substring(commaPositions[3]+1,commaPositions[4]);
            char latDirection = str[commaPositions[4]+1];
            setupLatitude(lat,latDirection);

            String lon = str.substring(commaPositions[5]+1,commaPositions[6]);
            char lonDirection = str[commaPositions[6]+1];
            setupLongitude(lon,lonDirection);

            String speed = str.substring(commaPositions[7]+1,commaPositions[8]); // in knots
            setupSpeed(speed);

            String heading = str.substring(commaPositions[8]+1,commaPositions[9]);
            if (1 < speed.length())
                setupHeading(heading);

        }
    }
    SerialUSB.println("Slut parseGRMCMessage");
}

void SaraR5::changeInGNSSFIX(char &status) {
    //debug.println(status);
    if (status != GNSSStatus){
        if (status == 'A')
            SerialUSB.println("GNSS fix acquired!");
        else
            SerialUSB.println("GNSS fix lost");
    }
    GNSSStatus = status;
}


void SaraR5::setupTimestamp(String &time, String &date) {
    // https://arduino.stackexchange.com/questions/69618/converting-human-readable-date-to-a-unix-timestamp

    tmElements_t my_time;  // time elements structure

    //debug.println(time);
    //debug.println(date);

    // convert a date and time into unix time, offset 1970
    my_time.Hour = time.substring(0,2).toInt();
    my_time.Minute = time.substring(2,4).toInt();
    my_time.Second = time.substring(4,6).toInt();

    my_time.Day = date.substring(0,2).toInt();
    my_time.Month = date.substring(2,4).toInt();      // months start from 0, so deduct 1
    my_time.Year = ("20" + date.substring(4,6)).toInt() - 1970; // years since 1970, so deduct 1970

    //droneID->timestamp = makeTime(my_time); // a unix timestamp
    droneID->timestamp = my_time.Minute*60 + my_time.Second; //makeTime(my_time); // a unix timestamp
}

void SaraR5::setupLatitude(String &str, char &direction) {
    //debug.println(str);

    String strLatDegree = str.substring(0,2);
    float lat_degree = strLatDegree.toFloat();
    //Serial.println(lat_degree);

    String strLatMinutes = str.substring(2,str.length());
    float lat_minutes = strLatMinutes.toFloat();

    lat_degree += lat_minutes / 60;
    droneID->latitude = lat_degree;

    //Serial.println(lat_degree,5);

    /// Mangler direction
}

void SaraR5::setupLongitude(String &str, char &direction) {
    //debug.println(str);

    String strLonDegree = str.substring(0,3);
    float long_degree = strLonDegree.toFloat();
    //Serial.println(lat_degree);

    String strLongMinutes = str.substring(3,str.length());
    float long_minutes = strLongMinutes.toFloat();

    long_degree += long_minutes / 60;
    droneID->longitude = long_degree;

    //Serial.println(long_degree,5);

    /// Mangler direction
}

void SaraR5::setupSpeed(String &str) {
    droneID->speed = str.toFloat()* 0.514444; // convert knots to m/s
}

void SaraR5::setupHeading(String &str) {
    droneID->heading = str.toFloat();
}

void SaraR5::setupAltitude(String &str) {
    droneID->altitude = str.toFloat();
}

void SaraR5::parseGGGAMessage(String &str) {
    /// Get number of Satellites, mean sea level altitude, geoid separarion
    // GGA,113834.00,5528.22581,N,01019.79931,E,1,09,1.09,18.2,M,43.1,M,,*78

    //debug.println(str);
    SerialUSB.println("Start parseGGGAMessage");

    int commaPositions[GGACommas];
    int index = 0;
    char c;
    for (int i = 0; i < str.length(); i++) {
        c = str[i];
        if (checkIfComma(c)){
            commaPositions[index] = i;
            index++;
        }
    }

    //String sat = str.substring(commaPositions[7]+1,commaPositions[8]); // Number of satellites
    String mslAltitude = str.substring(commaPositions[9]+1,commaPositions[10]); // mean sea level altitude
    //debug.println(mslAltitude);
    setupAltitude(mslAltitude);
    //String gsAltitude = str.substring(commaPositions[11]+1,commaPositions[12]); // geoid separarion altitude

    //Serial.println(sat);
    //Serial.println(mslAltitude);
    //Serial.println(gsAltitude);
    SerialUSB.println("Slut parseGGGAMessage");

}

void SaraR5::parseGGSAMessage(String &str) {
    /// Get PDOP, HDOP & VDOP
    // str = GSA,A,3,03,22,06,31,21,01,12,04,19,17,,,1.49,0.80,1.25*12
    //                                              ,PDOP,HDOP,VDOP

    //Serial.println(str);
    //debug.println(str);


    // Find commas starting from behind.
    int commaPositions[GSACommas];
    int index = 2;
    char c;
    for (int i = str.length(); 0 < i; i--) {
        c = str[i];
        if (checkIfComma(c)){
            commaPositions[index] = i;
            index--;
            if (index < 0)
                break;
        }
    }

    /*
    for(int i = 0; i < GSACommas; i++)
        Serial.println(commaPositions[i]);
    */

    String PDOP = str.substring(commaPositions[0]+1,commaPositions[1]);
    String HDOP = str.substring(commaPositions[1]+1,commaPositions[2]);
    String VDOP = str.substring(commaPositions[2]+1,commaPositions[2]+5); //str.indexOf('*'));
    
    //SerialUSB.println("VDOP" + VDOP);
    droneID->pDop = PDOP.toFloat();
    droneID->hDop = HDOP.toFloat();
    droneID->vDop = VDOP.toFloat();

}

bool SaraR5::checkIfComma(char &c) {
    if (c == ',')
        return true;
    return false;
}

bool SaraR5::checkIfAsterisk(char &c) {
    if (c == '*')
        return true;
    return false;
}

void SaraR5::logUMetric(){
    
    // int temp;
    // String str;
    // SerialUSB.print("logUmetric");

    // Serial1.println("AT+UMETRIC?\r");
    // response = "no response";
    // delay(1000);

   
    // while (!Serial1.available()); /// Wait on data

    // if (Serial1.available()){
    //     response = Serial1.readString();
    // }

    // SerialUSB.println(response);





    command = "AT+UMETRIC?" ;
    //  /// 14:22:12.248 -> +UMETRIC: 3,UL_NumberOfRbIds:0,CE_Level:1,DL_NumberOfRbIds:0,DL_NumOfPcktsDropMac:0,total_cell_reselections:0,total_radioLinkLoss:0,powerSavingMode:1,high_Mobility:1,connEstablishAttemptCount:7,
    //connEstablishSuccessCount:7,connEstablishFailureCount:0,reestablishmentAttemptCount:0,reestablishmentSuccessCount:0,reestablishmentFailureCount:0
    //,HO_AttemptCount:0,HO_SuccessCount:0,HO_FailureCount:0,EUTRAN_ConnReleaseCount:0,nas_num_of_attach:0, nas_num_of_tau:0, nas_num_of_service_request:4, nas_num_of_pdn_disconnect_req:0, nas_num_of_pdn_register_req:0, nas_num_of_auth:0, nas_num_of_bearer_resource_modify_req:0, nas_num_of_detach:0, nas_num_of_internal_detach_limit:0, nas_num_of_bearer_resource_alloc_req:0, nas_num_of_mt_sms_retry:0, nas_num_of_mo_sms_retry:0, nas_num_of_lpp_retransmission:0,nas_num_of_performance_attach:1,nas_num_of_performance_detach:0,nas_num_of_performance_deactivate:0,nas_num_of_performance_tracking:2,nas_num_of_performance_defaultBearer:0,nas_num_of_performance_dedicatedBearer:0,nas_num_of_performance_resourceModify:0,nas_num_of_performance_resourceSetup:0
    
    //UMETRIC: 4,0,1,0,0,0,0,1,1,10,10,0,0,0,0,0,0,0,0,0,0,9,7,8,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0

    //total_cell_reselections // nummer 6 -         15
    //total_radioLinkLoss // nummer 7 
    //connEstablishAttemptCount // nummer 10 
    //connEstablishSuccessCount //nummer 11
    //connEstablishFailureCount //nummer 12 
    // reestablishmentAttemptCount //nummer 13 
    // reestablishmentSuccessCount /nummer 14
    // reestablishmentFailureCount // nummer 15 
    // HO_AttemptCount  // nummer 16
    // HO_SuccessCount // nummer 17
    // HO_FailureCount // nummer 18 


    //RIGTIG 1. Gang 
    // 3,UL_NumberOfRbIds:0,CE_Level:1,DL_NumberOfRbIds:0,DL_NumOfPcktsDropMac:0,total_cell_reselections:0,total_radioLinkLoss:0,powerSavingMode:1,high_Mobility:1,connEstablishAttemptCount:52,connEstablishSuccessCount:51,connEstablishFailureCount:1,reestablishmentAttemptCount:0,reestablishmentSuccessCount:0,reestablishmentFailureCount:0,HO_AttemptCount:0,HO_SuccessCount:0,HO_FailureCount:0,EUTRAN_ConnReleaseCount:0,nas_num_of_attach:0, nas_num_of_tau:0, nas_num_of_service_request:50, nas_num_of_pdn_disconnect_req:0, nas_num_of_pdn_register_req:1, nas_num_of_auth:0, nas_num_of_bearer_resource_modify_req:0, nas_num_of_detach:0, nas_num_of_internal_detach_limit:0, nas_num_of_bearer_resource_alloc_req:0, nas_num_of_mt_sms_retry:0, nas_num_of_mo_sms_retry:0, nas_num_of_lpp_retransmission:0,nas_num_of_performance_attach:1,nas_num_of_performance_detach:0,nas_num_of_performance_deactivate:0,nas_num_of_performance_tracking:0,nas_num_of_performance_defaultBearer:0,nas_num_of_performance_dedicatedBearer:0,nas_num_of_performance_resourceModify:0,nas_num_of_performance_resourceSetup:0
    //FORKERT 2. Gang 
    // 3,UL_NumberOfRbIds:1,    CE_Level:1,UL_RbId_Index:0, UL_RbID:3,UL_BuffSduCount:0,    UL_RLC_Mode:0,DL_NumberOfRbIds:1,DL_NumOfPcktsDropMac:0,DL_RbId_Index:0,DL_RbId:3,DL_NumOfMissingSNs:0,DL_NumOfInvldPkts:0,DL_RLF_Mode:0,total_cell_reselections:0,total_radioLinkLoss:0,powerSavingMode:1,high_Mobility:1,connEstablishAttemptCount:53,connEstablishSuccessCount:52,connEstablishFailureCount:1,reestablishmentAttemptCount:0,reestablishmentSuccessCount:0,reestablishmentFailureCount:0,HO_AttemptCount:0,HO_SuccessCount:0,HO_FailureCount:0,EUTRAN_ConnReleaseCount:0,nas_num_of_attach:0, nas_num_of_tau:0, nas_num_of_service_request:51, nas_num_of_pdn_disconnect_req:0, nas_num_of_pdn_register_req:1, nas_num_of_auth:0, nas_num_of_bearer_resource_modify_req:0, nas_num_of_detach:0, nas_num_of_internal_detach_limit:0, nas_num_of_bearer_resource_alloc_req:0, nas_num_of_mt_sms_retry:0, nas_num_of_mo_sms_retry:0, nas_num_of_lpp_retransmission:0,nas_num_of_performance_attach:1,nas_num_of_performance_detach:0,nas_num_of_performance_deactivate:0,nas_num_of_performance_tracking:0,nas_num_of_performance_defaultBearer:0,nas_num_of_performance_dedicatedBearer:0,nas_num_of_performance_resourceModify:0,nas_num_of_performance_resourceSetup:0


    int counter = 0;
    while(counter < 4 )
    {
        delay(2000);
        Serial1.flush();

        Serial1.println(command);
        
        SerialUSB.println("U metric  ");//

        //delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf("OK");
            
            if(index > -1) //IF "OK" is received
            {
                response = response.substring(response.indexOf("+UMETRIC:")+1,response.length()); 
                
                //response = "OK "; //
                SerialUSB.println("OK len : " + String(response.length()));
                //SerialUSB.println(response);     

                response = response.substring(response.indexOf("total_cell_reselections:")+1,response.length()); 
                //SerialUSB.println(response);

                total_cell_reselections = response.substring(response.indexOf(":")+1,response.indexOf(","));
                SerialUSB.println("total_cell_reselections:"+ total_cell_reselections);
                //SerialUSB.println(response);
                
                response = response.substring(response.indexOf("total_radioLinkLoss:")+1,response.length());  
                total_radioLinkLoss =response.substring(response.indexOf(":")+1,response.indexOf(","));
                SerialUSB.println("total_radioLinkLoss:" + total_radioLinkLoss);
                //SerialUSB.println(response);

                response = response.substring(response.indexOf("connEstablishAttemptCount")+1,response.length());  
                connEstablishAttemptCount = response.substring(response.indexOf(":")+1,response.indexOf(","));
                SerialUSB.println("connEstablishAttemptCount:" + connEstablishAttemptCount);
                //SerialUSB.println(response);
                
                response = response.substring(response.indexOf("connEstablishSuccessCount")+1,response.length()); //14,0,0,0,0,0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 -- 11
                connEstablishSuccessCount = response.substring(response.indexOf(":")+1,response.indexOf(","));
                SerialUSB.println("connEstablishSuccessCount:"+ connEstablishSuccessCount);
                //SerialUSB.println(response);

                response = response.substring(response.indexOf("connEstablishFailureCount")+1,response.length()); //0,0,0,0,0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --12
                connEstablishFailureCount = response.substring(response.indexOf(":")+1,response.indexOf(","));
                SerialUSB.println("connEstablishFailureCount:"+ connEstablishFailureCount);
                //SerialUSB.println(response);

                response = response.substring(response.indexOf("reestablishmentAttemptCount")+1,response.length()); //0,0,0,0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --13
                reestablishmentAttemptCount = response.substring(response.indexOf(":")+1,response.indexOf(","));
                SerialUSB.println("reestablishmentAttemptCount:"+ reestablishmentAttemptCount);
                //SerialUSB.println(response);

                response = response.substring(response.indexOf("reestablishmentSuccessCount")+1,response.length()); //0,0,0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --14
                reestablishmentSuccessCount = response.substring(response.indexOf(":")+1,response.indexOf(","));
                SerialUSB.println("reestablishmentSuccessCount:"+ reestablishmentSuccessCount);
                //SerialUSB.println(response);

                response = response.substring(response.indexOf("reestablishmentFailureCount")+1,response.length()); //0,0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --15
                reestablishmentFailureCount = response.substring(response.indexOf(":")+1,response.indexOf(","));
                SerialUSB.println("reestablishmentFailureCount:"+ reestablishmentFailureCount);
                //SerialUSB.println(response);

                response = response.substring(response.indexOf("HO_AttemptCount")+1,response.length()); //0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --16
                HO_AttemptCount = response.substring(response.indexOf(":")+1,response.indexOf(","));
                SerialUSB.println("HO_AttemptCount:"+ HO_AttemptCount);
                //SerialUSB.println(response);

                response = response.substring(response.indexOf("HO_SuccessCount")+1,response.length()); //0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --17
                HO_SuccessCount = response.substring(response.indexOf(":")+1,response.indexOf(","));
                SerialUSB.println("HO_SuccessCount:"+ HO_SuccessCount);
                //SerialUSB.println(response);
                
                response = response.substring(response.indexOf("HO_FailureCount")+1,response.length()); //0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --18
                HO_FailureCount = response.substring(response.indexOf(":")+1,response.indexOf(","));
                SerialUSB.println("HO_FailureCount:"+ HO_FailureCount);
                //SerialUSB.println(response);

                break;
            }
            else
            {

                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::logLTEStats() {
    
    
    int temp;
    String str;

    Serial1.println("AT+CESQ\r");
    response = "no response";
    delay(50);

    int responseCounter= 0; 

    while(!Serial1.available() && responseCounter<100)
    {
        delay(10);
        responseCounter = responseCounter + 1; 
    }

    if (Serial1.available()){
        response = Serial1.readString();
    }

    //SerialUSB.println(response);

    if(response.indexOf("O") > 0) { // Check if message contains "OK"
        // Find commas starting from behind.
        int index = 0;
        char c;
        for (int i = response.length(); 0 < i; i--) {
            c = response[i];
            if (checkIfComma(c)){
                commaList[index] = i;
                //SerialUSB.println(i);
                index++;
                
            }
        }

        str = response.substring(commaList[1]+1,commaList[0]); // RSRQ
        //SerialUSB.println(str);
        temp = str.toInt();
        if(temp < 1)
            rsrq = -19.5;
        else if (1 <= temp && temp <= 33)
            rsrq = temp * 0.5 - 20;
        else if(temp == 34)
            rsrq = -3;
        else if (temp == 255)
            rsrq = 255;

        SerialUSB.print(rsrq); SerialUSB.println(" - RSRQ_float");

        str = response.substring(commaList[0]+1,response.length()); // RSRP
        temp = str.toInt();

        if(temp < 1)
            rsrp = -140;
        else if (1 <= temp && temp <= 96)
            rsrp = temp  - 141;
        else if(temp == 97)
            rsrp = -44;
        else if (temp == 255)
            rsrp = 255;

        SerialUSB.print(rsrp); SerialUSB.println(" - RSRP");

        SerialUSB.println(response);
        
        

        // https://www.twilio.com/docs/iot/supersim/how-determine-good-cellular-signal-strength
    }
}
void SaraR5::RSSI()
{
    int temp;
    String str;
    int responseCounter;


    command = "AT+CSQ" ;

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(command); //
        //SerialUSB.println("Test command active ");//
        responseCounter= 0; 
        while(!Serial1.available() && responseCounter<100)
        {
            delay(10);
            responseCounter = responseCounter + 1; 
        }

        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf("OK");
            //response = response.substring(index,response.length());

            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                str = response.substring(response.indexOf(":")+1,commaList[4]); // RSSI
                temp = str.toInt();


                    // SerialUSB.println(str);
                    
                    // SerialUSB.println(temp);
                    
                if (temp <= 32)
                    rssi = temp*2  - 113;
                else if(temp == 99)
                    rssi = 255;

                SerialUSB.print(rssi); SerialUSB.println(" - RSSI");
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }

}
/// HTTP
void SaraR5::performPDPaction(int profile, int action)
{

    command = SARA_R5_MESSAGE_PDP_ACTION;
    command += "=" + String(profile) + "," + String(action); // 0 is deactivate

    int counter = 0;
    // VI skal deaktivere profil nul
    while(counter < 4 )
    {
        Serial1.println(command); //Read the Packet Data protocol data
        delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index1 = response.indexOf("UUPSDA: ");

            int index = response.indexOf("OK");
            //response = response.substring(index,response.length());
            if(index1 > -1)
            {
                response = response.substring(index1 + 7, response.length());
                SerialUSB.println("Profile is activated with IP_address" + response);
                break;
            }
            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                SerialUSB.println(response);
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;

    }

}

void SaraR5::setupPSDprofile() // Packet Switch Data Profile
{
    int cid = 0; //Context identifier is zero 0-11

    // Deactivate the PSD profile
    SerialUSB.println("Deactivate the PSD profile ");
    performPDPaction(PSDprofile, SARA_R5_PSD_ACTION_DEACTIVATE);


    // Load the profile to NVM - so we can load it again in the later examples
    SerialUSB.println("LOAD the profile from NVM ");
    performPDPaction(PSDprofile, SARA_R5_PSD_ACTION_LOAD);

    // Activate the profile
    SerialUSB.println("Activate the profile");
    performPDPaction(PSDprofile, SARA_R5_PSD_ACTION_ACTIVATE);
    
}
void SaraR5::resetHTTPprofile(int profile)
{

    command = SARA_R5_HTTP_PROFILE ;
    command += "=" + String(profile); // profile 0-4
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(command); //
        delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf("OK");
            //response = response.substring(index,response.length());

            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                SerialUSB.println("The profile is reset");
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }
}
void SaraR5::setHTTPserver(int profile, int OP_CODE, String serverName)
{

    command = SARA_R5_HTTP_PROFILE ;
    command += "=" + String(profile) + "," + String(OP_CODE) +","+ "\""+ serverName + "\""; // profile 0-4
    //command = "AT+UHHTP=0,1,\"api.thingspeak.com/channels/1555456/bulk_update.json\"";
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(command);
        //Serial1.println("AT+UHHTP=0,1,\"api.thingspeak.com/channels/1555456/bulk_update.json\""); //
        //SerialUSB.println("Setting server Name");//
        //SerialUSB.println(serverName);//
        SerialUSB.println("Setup API URL ");//

        delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf("OK");
            //response = response.substring(index,response.length());

            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                SerialUSB.println(response);
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }
}
void SaraR5::setHTTPserverPort(int profile, int OP_CODE, int serverPort)
{

    command = SARA_R5_HTTP_PROFILE ;
    command += "=" + String(profile) + "," + String(OP_CODE) +","+ String(serverPort)  ; // profile 0-4
    //command = "AT+UHHTP=0,1,\"api.thingspeak.com/channels/1555456/bulk_update.json\"";
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(command);
        //Serial1.println("AT+UHHTP=0,1,\"api.thingspeak.com/channels/1555456/bulk_update.json\""); //
        //SerialUSB.println("Setting server Name");//
        //SerialUSB.println(serverName);//
        SerialUSB.println("Setup Server port  ");//

        delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf("OK");
            //response = response.substring(index,response.length());

            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                SerialUSB.println(response);
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::listFiles()
{

    command = "AT+ULSTFILE" ;
    command +=  "="; // profile 0-4
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(command); //
        SerialUSB.println("List files");//
        delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf("OK");
            //response = response.substring(index,response.length());

            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                SerialUSB.println(response);
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::readFiles()
{

    command = "AT+URDFILE" ;
    command +=  "=\"post_response.txt\""; // profile 0-4
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(command); //
        SerialUSB.println("Read file");//
        delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf(":");
            //response = response.substring(index,response.length());

            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                SerialUSB.println(response);
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::tester()
{

    command = "AT+UMETRIC=3,2048" ; // Defines what cell information we want to read, and how to read it 
    //command = "AT+CSQ" ;

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(command); //
        SerialUSB.println("Test command active ");//
        delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf("OK");
            //response = response.substring(index,response.length());

            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                SerialUSB.println(response);
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::setHTTPsecure(int profile, boolean secure)
{

    command = SARA_R5_HTTP_PROFILE ;
    command += "=" + String(profile) + "," + SARA_R5_HTTP_OP_SECURE +","+ secure  ; // profile 0-4
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(command); //
        SerialUSB.println("Setting server security");//
        delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf("OK");
            //response = response.substring(index,response.length());

            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                SerialUSB.println(response);
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }
}


void SaraR5::setupHTTP()
{
    // Reset HTTP profile 0
    resetHTTPprofile(HTTPprofile);

    // Set the server name
    setHTTPserver(HTTPprofile,SARA_R5_HTTP_OP_CODE_SERVER_NAME, serverName);

    // Set the server-port up 
    setHTTPserverPort(HTTPprofile,5,8080);

    // Use HTTP secure
    setHTTPsecure(HTTPprofile, false); // Setting this to true causes the POST / GET to fail. Not sure why...

    // Set the custom header 
    setHTTPserver(HTTPprofile,9,"0:Connection:keep-alive" );
    setHTTPserver(HTTPprofile,9,"1:Cache-Control: no-store" );


}

void SaraR5::sendHTTPPOSTdata(int profile, String path, String responseFilename,
                              String data, int httpContentType)
{
    //SerialUSB.println("Send HTTP1");
    command = SARA_R5_HTTP_COMMAND;
    command +=  "=" + String(profile) + "," + SARA_R5_HTTP_COMMAND_POST_DATA +","+ "\""+path + "\"" + "," +  "\"" + responseFilename + "\"" + "," + "\"" + data + "\"" + "," + String(httpContentType) ; // profile 0-4
    //SerialUSB.println(command); //
    int counter = 0;
    int responseCounter; 
    
    Serial1.flush();
        
    //SerialUSB.println("Send HTTP POST data");//
    
    Serial1.println(command); //
    SerialUSB.println("Sending message" + command); //
    
        
    while(counter < 15 )
    {
        

        responseCounter= 0; 
        while(!Serial1.available() && responseCounter<100)
        {
            delay(10);
            responseCounter = responseCounter + 1; 
        }
        SerialUSB.println("Response counter : " + String(responseCounter));
        
        if (Serial1.available()){
            
            
            response = Serial1.readString();    
            

            int index = response.indexOf("UUHTTPCR");
            //response = response.substring(index,response.length());
            //SerialUSB.println(response);

            if(index > -1) //IF "OK" is received   
            {
                //response = "OK "; //
                SerialUSB.println("Response UUHTTPCR :" + response);
                break;
            }
            else
            {
                SerialUSB.println("Not Ready : " + response);
            }
        }
        counter = counter + 1;
    }
    //SerialUSB.println("Got response "); //
    

}
String SaraR5::encodeMessage()
{
    String outputString; 
    outputString = ""; // String(droneID->timestamp); 
    String speedString = String(droneID->speed,1);

    //Flags 
    String statusBit = "1"; //bit (Status): 0=On ground, 1=Flying 
    String speedMultiplierBit = "0"; // bit (Speed multiplier) : 0 = 0.25, 1 = 0.75
    String headingAdderBit = "0"; //bit (Heading adder) : 0 = +0, 1=+180 degrees 
    String reservedBit = "0"; //bit free?  


    uint8_t encodedSpeed = 0;
    // Speed encoding 
    float speedFloat = speedString.toFloat();

    if(speedFloat<=225*0.25)
    {
        encodedSpeed = speedString.toInt()/0.25;
    }
    else if(speedFloat>225*0.25 && speedFloat < 254.25)
    {
        encodedSpeed = (speedString.toInt()-(225*0.25))/0.75;
        speedMultiplierBit = "1"; 
    }
    else
    {
        encodedSpeed = 254; 
        speedMultiplierBit = "1"; 
    }
    
    int32_t EncodedLatitude = String(String(droneID->latitude,7).toFloat()*10e6).toInt(); 
    
    int32_t EncodedLongitude = String(String(droneID->longitude,7).toFloat()*10e6).toInt(); 

    // Heading encoding 2 HEX 

    
    int valueHeading = String(droneID->heading).toInt();
    uint8_t encodedHeading = 0; 

    if(valueHeading<180)
    {
        encodedHeading = valueHeading;
        headingAdderBit = "0";
    }
    else
    {
        encodedHeading = valueHeading-180;
        headingAdderBit = "1";
    }
    String hexEncodedHeading = String(encodedHeading,HEX);
    while(hexEncodedHeading.length()<2){
        hexEncodedHeading = "0" + hexEncodedHeading;
    }

    // SerialUSB.println("Heading Q" + String(droneID->heading));
    // Altitude AMSL 
    float valueAMSL = String(droneID->altitude,2).toFloat(); 
    uint16_t encodedAMSL = 0; 
    encodedAMSL = round((valueAMSL+1000)/0.5);
    String hexEncodedAMSL = String(encodedAMSL,HEX);
    while(hexEncodedAMSL.length()<4)
    {
        hexEncodedAMSL = "0" + hexEncodedAMSL; 
    }

    // HDOP - NYYYY
    float valueHDOP = String(droneID->hDop,1).toFloat(); 
    uint8_t encodedHDOP = 0; 
    encodedHDOP = round((valueHDOP*10));
    String hexEncodedHDOP = String(encodedHDOP,HEX);
    while(hexEncodedHDOP.length()<2)
    {
        hexEncodedHDOP = "0" + hexEncodedHDOP; 
    }
    // VDOP - NYYYY
    float valueVDOP = String(droneID->vDop,1).toFloat(); 
    uint8_t encodedVDOP = 0; 
    encodedVDOP = round((valueVDOP*10));
    String hexEncodedVDOP = String(encodedVDOP,HEX);
    while(hexEncodedVDOP.length()<2)
    {
        hexEncodedVDOP = "0" + hexEncodedVDOP; 
    }

    // Timestamp 
    uint16_t valueTime = String(droneID->timestamp).toInt();
    String hexEncodedTime = String(valueTime,HEX);
    while(hexEncodedTime.length()<3)
    {
        hexEncodedTime = "0" + hexEncodedTime; 
    }

    // Flags encoding 
    String bitString = statusBit + speedMultiplierBit + headingAdderBit + reservedBit; 
  
    int EncodedFlags = strtol( bitString.c_str(), NULL, 2 );   
  


    // From Decimal to Hex Speed 
    String hexEncodedSpeed = String(encodedSpeed,HEX);
    if(hexEncodedSpeed.length()<2)
    {
        hexEncodedSpeed = "0"+hexEncodedSpeed;   
    }
  
    //From Decimal to Hex Lat - Long -
    String hexEncodedLat = String(EncodedLatitude, HEX);
    String hexEncodedLong = String(EncodedLongitude, HEX); 
     if(hexEncodedLat.length() == 7)
    {
        hexEncodedLat = "0" + hexEncodedLat;
    }
    if(hexEncodedLong.length() == 7)
    {
        hexEncodedLong = "0" + hexEncodedLong;
    }

    if(hexEncodedLat.length() != 8 or hexEncodedLong.length() != 8)
    {
        hexEncodedLat = "00000000";
        hexEncodedLong = "00000000";       
    }


    // Encoded 
    SerialUSB.println(serialNumber + " Flags: " + String(EncodedFlags, HEX) + " Speed :" + hexEncodedSpeed + " Heading : " + hexEncodedHeading + " LAT:"+ String(hexEncodedLat) + " Lon:" + String(hexEncodedLong) + " AMSL:" + String(hexEncodedAMSL) + " HDOP:" + String(hexEncodedHDOP) + " VDOP:" + String(hexEncodedVDOP) + " Time:" + String(hexEncodedTime));
    outputString =  serialNumber + String(EncodedFlags, HEX) + hexEncodedSpeed + hexEncodedHeading + String(hexEncodedLat) + String(hexEncodedLong) + String(hexEncodedAMSL) + String(hexEncodedHDOP) + String(hexEncodedVDOP) + String(hexEncodedTime);
    
    return outputString;
}


void SaraR5::sendHTTPusingPOST()
{
    int temperature =26;
    // Send data using HTTP POST
    //String httpRequestData = "api_key=" + apiKey1 + "&field1=" + String(temperature);
    
    // //                  Things speak server 
    // //Main Channel 
    // payload1 = "api_key=" + apiKey1 + "&field1=" + String(droneID->timestamp) + "&field2=" + String(droneID->latitude,5) + "&field3="+ String(droneID->longitude,5) + "&field4=" + String(droneID->speed,3) + "&field5=" + String(droneID->heading,3) + "&field6=" + String(droneID->altitude,1) +"&field7="+  String(droneID->hDop) + "&field8="+ String(droneID->vDop); //+ "&latitude=" + String(droneID->latitude,5);
    // //TestChannel 
    // payload2 = "api_key=" + apiKey2 + "&field1=" + String(droneID->timestamp) + "&field2="  + String(rsrq) + "&field3=" + String(rsrp) + "&field4=" +total_cell_reselections + "&field5=" +total_radioLinkLoss +"&field6="+connEstablishAttemptCount + "&field7=" + connEstablishSuccessCount + "&field8="+ connEstablishFailureCount; 
    // //TestChannel2
    // payload3 = "api_key=" + apiKey3 + "&field1=" + String(droneID->timestamp) + "&field2=" + reestablishmentAttemptCount + "&field3=" + reestablishmentSuccessCount + "&field4=" + reestablishmentFailureCount + "&field5=" +HO_AttemptCount+"&field6="+ HO_SuccessCount + "&field7=" + HO_FailureCount + "&field8="+ String(rssi); 
   
    // // WWW-web-application
    // sendHTTPPOSTdata(HTTPprofile, "/update", "post_response1.txt", payload1, SARA_R5_HTTP_CONTENT_APPLICATION_X_WWW);
    // sendHTTPPOSTdata(HTTPprofile, "/update", "post_response2.txt", payload2, SARA_R5_HTTP_CONTENT_APPLICATION_X_WWW);
    // sendHTTPPOSTdata(HTTPprofile, "/update", "post_response3.txt", payload3, SARA_R5_HTTP_CONTENT_APPLICATION_X_WWW);

    //payload1 = "FFFF06140F2104C04A063121BE03FD03FD03FD00000E1B" //46 bytes HEX ENCODED
    payload1 = encodeMessage();//"Heej fra DroneID5G"; //52 bytes deciman

    //                  For SDU node js server 
    // FOR SDU SERVER 
    sendHTTPPOSTdata(HTTPprofile, "/post-test", "post_response.txt", payload1, SARA_R5_HTTP_CONTENT_TEXT_PLAIN);


    //              READ response 
    //listFiles();

    //readFiles();

}

void SaraR5::HybridPositioningSetup()
{
    command = SARA_R5_GNSS_CONFIGURE_SENSOR;
    command += "=15" ; // + String(profile) + "," + SARA_R5_HTTP_OP_SECURE +","+ secure  ; // profile 0-4
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(command); //
        SerialUSB.println(command);//
        delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf("OK");
            //response = response.substring(index,response.length());

            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                SerialUSB.println(response);
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }
    command = SARA_R5_GNSS_CONFIGURE_LOCATION;
    command += "=0" ; // + String(profile) + "," + SARA_R5_HTTP_OP_SECURE +","+ secure  ; // profile 0-4
    counter = 0;
    while(counter < 4 )
    {
        Serial1.println(command); //
        SerialUSB.println(command);//
        delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf("OK");
            //response = response.substring(index,response.length());

            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                SerialUSB.println(response);
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::HybridPositioning()
{
     command = SARA_R5_GNSS_REQUEST_LOCATION; //SARA_R5_GNSS_REQUEST_LOCATION;
    command += "=2,3,0,120,15,1" ; // + String(profile) + "," + SARA_R5_HTTP_OP_SECURE +","+ secure  ; // profile 0-4
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(command); //
        SerialUSB.println(command);//
        delay(1000);
        if (Serial1.available()){
            response = Serial1.readString();
            //Serial1.println(response);
            int index = response.indexOf("OK");
            //response = response.substring(index,response.length());

            if(index > -1) //IF "OK" is received
            {
                //response = "OK "; //
                SerialUSB.println(response);
                break;
            }
            else
            {
                SerialUSB.println(response);
            }
        }
        counter = counter + 1;
    }
}


void SaraR5::init() { //Setup
    
   
    // Function used to setup and initialized the device
    pinMode(LED_BUILTIN, OUTPUT); // Setup LED
    

    beginSerial();
    
    //turnLED(HIGH);

    waitForBoardOK();
    checkSimCardId();

    //searchForNetworks(); // Er ikke på normalt 

      /// HTTP
  

   
    enableGPS(); // Er på Normal 
   
    waitForLTESignal(); //PÅ NORMALT 
    //enableCellInformation();

    //HTTP
    setupPSDprofile();  //PÅ Normalt 

    setupHTTP(); // PÅ NORMALT 

  //  HybridPositioningSetup();
  //  HybridPositioning();


    //tester();

    SerialUSB.println("Init done ");
}


void SaraR5::main() {

    //
    turnLED(HIGH);

    getGPSPosition(); //Gør sådan at den at den først får positionen hvis der er fix
    //HybridPositioning();
    //SerialUSB.println("GPS done ");
    ///SerialUSB.println(encodeMessage());
    
    turnLED(LOW);
    turnLED(HIGH);
    sendHTTPusingPOST();
    //delay(500);
    
    turnLED(LOW);


    if (GNSSStatus == 'A')
    {
        //logLTEStats();
        //logUMetric();
        //RSSI();
        
        turnLED(LOW);
        //delay(500);
       
    }
    else
    {
        
        //delay(100);
        turnLED(LOW);
        //delay(100);
    }
    
        
}
