/*
  SaraR5.cpp - Library for sending HTTP ATcommands to Naviair via u-blox SARA-R5 chip.
*/

#include "SaraR5.h"

//SoftwareSerial debug(9,8); // RX , TX
//SoftwareSerial debug(9,8); // RX , TX

/// SET SERIAL_BUFFER_SIZE 320 in RingBuffer.h

int commaList[UmetricCommas];

SaraR5::SaraR5(DroneID5GLib* _droneID){
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
    socketID = -1; 
    socketID1 = -1; 
    
    serialNumber = "ab1234";

    ATcommand = "{\"write_api_key\": \"RJ9P1U1PKXIYX8MS\",\"updates\": [{\"delta_t\": 0,\"field1\": 1.0,\"field2\": \"2.0";
    ATresponse = "{\"write_api_key\": \"RJ9P1U1PKXIYX8MS\",\"updates\": [{\"delta_t\": 0,\"field1\": 1.0,\"field2\": \"2.0";

   // tmElements_t my_time;  // time elements structure

    //              -- Thingsspeak API --
    //serverName = "api.thingspeak.com";
    

    //      NODE JS server 
    serverName = "genius-lte-m.sdu.dk";
    serverPort = "164.92.145.69";//"130.225.157.81";

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

void SaraR5::beginSerial(){
    Serial.begin(57600);

    Serial1.begin(115200);
    //while (!Serial1) {  ;  }

    Serial1.setTimeout(10);

    SerialUSB.begin(115200);
    //while (!SerialUSB) {  ;  }

    SerialUSB.println("New upload!");
}

void SaraR5::waitForLTESignal() {
    SerialUSB.println("Waiting for operator connection...");
    int counter = 0;
    String ATresponse;
    while(counter < 300) {
        ATresponse = getOperatorInfo();
        counter++;
        SerialUSB.println(ATresponse);
        //Serial.println("Start of ATresponse:");
        //Serial.println(ATresponse);
        //Serial.println("End of ATresponse:");
        if(ATresponse.indexOf("TDC") > 0){ // Check if message contains "TDC"
            //Serial.println(ATresponse);
            SerialUSB.println("Connection to TDC network established!");
            break;
        }
        //Serial.println(counter);
        RSSI();
    }
}

void SaraR5::waitForBoardOK() {
    int counter = 0;
    String ATresponse;
    while(counter < 20) {
        ATresponse = checkStatus();
        counter++;
        //Serial.println(ATresponse);
        if(ATresponse.indexOf("OK") > 0){ // Check if message contains "OK"
            SerialUSB.println("OK recieved! Board is powered and online.");
            break;
        }
        //Serial.println(counter);
    }
}

String SaraR5::checkStatus() {

    //serialWriteToSara.println("AT\r");
    Serial1.println("AT\r");
    String ATresponse = "no ATresponse";
    delay(2500);
    /*
    while (serialWriteToSara.available())
        Serial.write(serialWriteToSara.read());
    while (Serial.available())
        serialWriteToSara.write(Serial.read());
        */

    while (Serial1.available()){
        ATresponse = Serial1.readString();
    }
    return ATresponse;
}

void SaraR5::enableGPS() {
    String ATresponse = "no ATresponse";

    // Read GPS status
    Serial1.println("AT+UGPS?\r");
    delay(10);
    while (Serial1.available()){
        ATresponse = Serial1.readString();
    }
    SerialUSB.println(ATresponse);

    if(ATresponse.indexOf("1") > 0) // Check if message contains "OK"
        SerialUSB.println("GNSS module already on.");
    else{// Enable the GPS module
            SerialUSB.println("Turning on GNSS module...\r");
            Serial1.println("AT+UGPS=1,15,127\r");
            delay(2000);
            while (Serial1.available()){
                ATresponse = Serial1.readString();
            }
            if(ATresponse.indexOf("OK") > 0) // Check if message contains "OK"
                SerialUSB.println("OK recieved! GNSS module is on and using all GNSS systems.");
            else
                SerialUSB.println(ATresponse);
    }

    //Serial.println(ATresponse);

    // Enable storing of the last value of NMEA $RMC messages
    Serial1.println("AT+UGRMC=1\r");
    ATresponse = "no ATresponse";

    delay(10);
    while (Serial1.available()){
        ATresponse = Serial1.readString();
    }
    if(ATresponse.indexOf("OK") > 0) // Check if message contains "OK"
        SerialUSB.println("OK recieved! Storing of NMEA $RMC messages is enable.");

    // Enable storing of the last value of NMEA $GGA messages
    Serial1.println("AT+UGGGA=1\r");
    ATresponse = "no ATresponse";

    delay(10);
    while (Serial1.available()){
        ATresponse = Serial1.readString();
    }
    if(ATresponse.indexOf("OK") > 0) // Check if message contains "OK"
        SerialUSB.println("OK recieved! Storing of NMEA $GGA messages is enable.");
    //Serial.println(ATresponse);

    // Enable storing of the last value of NMEA $GSA messages
    Serial1.println("AT+UGGSA=1\r");
    ATresponse = "no ATresponse";

    delay(10);
    while (Serial1.available()){
        ATresponse = Serial1.readString();
    }
    if(ATresponse.indexOf("OK") > 0) // Check if message contains "OK"
        SerialUSB.println("OK recieved! Storing of NMEA $GSA messages is enable.");
    //Serial.println(ATresponse);
}


void SaraR5::enableCellInformation() {
    String ATresponse = "no ATresponse";

    Serial1.println("AT+UMETRIC=4,2048"); // enable read without tag and getting LTE UE STATS -> 12. bit to 1 = 2048
    delay(10);
    while (Serial1.available()){
        ATresponse = Serial1.readString();
    }
    //SerialUSB.println(ATresponse);

}

String SaraR5::getOperatorInfo() {
    Serial1.println("AT+COPS?\r");
    String ATresponse = "no ATresponse";
    delay(1000);
    while (Serial1.available()){
        ATresponse = Serial1.readString();
    }
    return ATresponse;
}

void SaraR5::checkSimCardId() {
    String ATresponse = "no ATresponse";


    SerialUSB.println("Sim Card Circuit Card ID:");


    Serial1.println("AT+CCID");

    ATresponse = "no ATresponse";
    delay(50);
    while (Serial1.available()){
        ATresponse = Serial1.readString();
    }
    SerialUSB.println(ATresponse);

}

void SaraR5::searchForNetworks() {
    String ATresponse = "no ATresponse";

    Serial1.println("AT+COPN");

    ATresponse = "no ATresponse";
    delay(100);
    while (Serial1.available()){
        ATresponse = Serial1.readString();
    }
    SerialUSB.println(ATresponse);

}



void SaraR5::getGPSPosition() {
    // https://www.sparkfun.com/datasheets/GPS/NMEA%20Reference%20Manual1.pdf
    
    int counterGPS = 0; 
    String RMCmessage = "";
    String GGAmessage = "";
    String GSAmessage = "";


    //SerialUSB.println("Print 1 ");
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

            //SerialUSB.println("index RMC " + String(RMCmessage.indexOf("AT+UGRMC?")));
            parseGRMCMessage(RMCmessage);
            //SerialUSB.println("Print 3.3.1 ");

            if (GNSSStatus == 'A' && GGAmessage.indexOf("OK") > 0 && 1 > GGAmessage.indexOf("AT+UGGGA?") > -1)
                //SerialUSB.println("Len GGA " + String(GGAmessage.length()));
                parseGGGAMessage(GGAmessage);
              //  SerialUSB.println("Print 3.3.3 ");
            if (GNSSStatus == 'A' && GSAmessage.indexOf("OK") > 0 && 1 > GSAmessage.indexOf("AT+UGGSA?") > -1)
                //SerialUSB.println("Len GSA " + String(GSAmessage.length()));
                parseGGSAMessage(GSAmessage);
                //SerialUSB.println("Print 3.3.4 ");

    } 
    else 
    {
        turnLED(LOW); /// Glemmer at slukke hvis timestamp ikke bliver opdateret
        SerialUSB.print("No GPS ");
    }

    if (GNSSStatus != 'A')
    {
        
        SerialUSB.print(GNSSStatus);
        
    }
    //while (!Serial1.available()); /// Wait on data
    //SerialUSB.println("Print 4 ");
}

void SaraR5::parseGRMCMessage(String &str) {

    //debug.println(str);
    //SerialUSB.println("Start parseGRMCMessage");
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

    if(c == 'A' || c == 'v'){

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
    }

    
    
    //SerialUSB.println("Slut parseGRMCMessage");
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
    //SerialUSB.println("Start parseGGGAMessage");

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
    //SerialUSB.println("Slut parseGGGAMessage");

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
    // ATresponse = "no ATresponse";
    // delay(1000);

   
    // while (!Serial1.available()); /// Wait on data

    // if (Serial1.available()){
    //     ATresponse = Serial1.readString();
    // }

    // SerialUSB.println(ATresponse);





    ATcommand = "AT+UMETRIC?" ;
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

        Serial1.println(ATcommand);
        
        SerialUSB.println("U metric  ");//

        //delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            
            if(index > -1) //IF "OK" is received
            {
                ATresponse = ATresponse.substring(ATresponse.indexOf("+UMETRIC:")+1,ATresponse.length()); 
                
                //ATresponse = "OK "; //
                SerialUSB.println("OK len : " + String(ATresponse.length()));
                //SerialUSB.println(ATresponse);     

                ATresponse = ATresponse.substring(ATresponse.indexOf("total_cell_reselections:")+1,ATresponse.length()); 
                //SerialUSB.println(ATresponse);

                total_cell_reselections = ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(","));
                SerialUSB.println("total_cell_reselections:"+ total_cell_reselections);
                //SerialUSB.println(ATresponse);
                
                ATresponse = ATresponse.substring(ATresponse.indexOf("total_radioLinkLoss:")+1,ATresponse.length());  
                total_radioLinkLoss =ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(","));
                SerialUSB.println("total_radioLinkLoss:" + total_radioLinkLoss);
                //SerialUSB.println(ATresponse);

                ATresponse = ATresponse.substring(ATresponse.indexOf("connEstablishAttemptCount")+1,ATresponse.length());  
                connEstablishAttemptCount = ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(","));
                SerialUSB.println("connEstablishAttemptCount:" + connEstablishAttemptCount);
                //SerialUSB.println(ATresponse);
                
                ATresponse = ATresponse.substring(ATresponse.indexOf("connEstablishSuccessCount")+1,ATresponse.length()); //14,0,0,0,0,0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 -- 11
                connEstablishSuccessCount = ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(","));
                SerialUSB.println("connEstablishSuccessCount:"+ connEstablishSuccessCount);
                //SerialUSB.println(ATresponse);

                ATresponse = ATresponse.substring(ATresponse.indexOf("connEstablishFailureCount")+1,ATresponse.length()); //0,0,0,0,0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --12
                connEstablishFailureCount = ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(","));
                SerialUSB.println("connEstablishFailureCount:"+ connEstablishFailureCount);
                //SerialUSB.println(ATresponse);

                ATresponse = ATresponse.substring(ATresponse.indexOf("reestablishmentAttemptCount")+1,ATresponse.length()); //0,0,0,0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --13
                reestablishmentAttemptCount = ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(","));
                SerialUSB.println("reestablishmentAttemptCount:"+ reestablishmentAttemptCount);
                //SerialUSB.println(ATresponse);

                ATresponse = ATresponse.substring(ATresponse.indexOf("reestablishmentSuccessCount")+1,ATresponse.length()); //0,0,0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --14
                reestablishmentSuccessCount = ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(","));
                SerialUSB.println("reestablishmentSuccessCount:"+ reestablishmentSuccessCount);
                //SerialUSB.println(ATresponse);

                ATresponse = ATresponse.substring(ATresponse.indexOf("reestablishmentFailureCount")+1,ATresponse.length()); //0,0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --15
                reestablishmentFailureCount = ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(","));
                SerialUSB.println("reestablishmentFailureCount:"+ reestablishmentFailureCount);
                //SerialUSB.println(ATresponse);

                ATresponse = ATresponse.substring(ATresponse.indexOf("HO_AttemptCount")+1,ATresponse.length()); //0,0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --16
                HO_AttemptCount = ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(","));
                SerialUSB.println("HO_AttemptCount:"+ HO_AttemptCount);
                //SerialUSB.println(ATresponse);

                ATresponse = ATresponse.substring(ATresponse.indexOf("HO_SuccessCount")+1,ATresponse.length()); //0,0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --17
                HO_SuccessCount = ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(","));
                SerialUSB.println("HO_SuccessCount:"+ HO_SuccessCount);
                //SerialUSB.println(ATresponse);
                
                ATresponse = ATresponse.substring(ATresponse.indexOf("HO_FailureCount")+1,ATresponse.length()); //0,0,0,0,13,11,12,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 --18
                HO_FailureCount = ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(","));
                SerialUSB.println("HO_FailureCount:"+ HO_FailureCount);
                //SerialUSB.println(ATresponse);

                break;
            }
            else
            {

                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::logLTEStats() {
    
    
    int temp;
    String str;

    Serial1.println("AT+CESQ\r");
    ATresponse = "no ATresponse";
    delay(50);

    int ATresponseCounter= 0; 

    while(!Serial1.available() && ATresponseCounter<100)
    {
        delay(10);
        ATresponseCounter = ATresponseCounter + 1; 
    }

    if (Serial1.available()){
        ATresponse = Serial1.readString();
    }

    //SerialUSB.println(ATresponse);

    if(ATresponse.indexOf("O") > 0) { // Check if message contains "OK"
        // Find commas starting from behind.
        int index = 0;
        char c;
        for (int i = ATresponse.length(); 0 < i; i--) {
            c = ATresponse[i];
            if (checkIfComma(c)){
                commaList[index] = i;
                //SerialUSB.println(i);
                index++;
                
            }
        }

        str = ATresponse.substring(commaList[1]+1,commaList[0]); // RSRQ
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

        str = ATresponse.substring(commaList[0]+1,ATresponse.length()); // RSRP
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

        SerialUSB.println(ATresponse);
        
        

        // https://www.twilio.com/docs/iot/supersim/how-determine-good-cellular-signal-strength
    }
}
void SaraR5::RSSI()
{
    int temp;
    String str;
    int ATresponseCounter;


    ATcommand = "AT+CSQ" ;

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        //SerialUSB.println("Test ATcommand active ");//
        ATresponseCounter= 0; 
        while(!Serial1.available() && ATresponseCounter<100)
        {
            delay(10);
            ATresponseCounter = ATresponseCounter + 1; 
        }

        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                str = ATresponse.substring(ATresponse.indexOf(":")+1,ATresponse.indexOf(",")); // RSSI
                temp = str.toInt();


              //  SerialUSB.println(ATresponse);
                    
               // SerialUSB.println("str" + str);
                    
                // if (temp <= 32)
                //     rssi = temp*2  - 113;
                // else if(temp == 99)
                //     rssi = 255;
                rssi = temp;

               // SerialUSB.print(rssi); SerialUSB.println(" - RSSI");
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }

}
/// HTTP
void SaraR5::performPDPaction(int profile, int action)
{

    ATcommand = SARA_R5_MESSAGE_PDP_ACTION;
    ATcommand += "=" + String(profile) + "," + String(action); // 0 is deactivate

    int counter = 0;
    // VI skal deaktivere profil nul
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //Read the Packet Data protocol data
        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index1 = ATresponse.indexOf("UUPSDA: ");

            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());
            if(index1 > -1)
            {
                ATresponse = ATresponse.substring(index1 + 7, ATresponse.length());
                SerialUSB.println("Profile is activated with IP_address" + ATresponse);
                break;
            }
            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
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

    ATcommand = SARA_R5_HTTP_PROFILE ;
    ATcommand += "=" + String(profile); // profile 0-4
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println("The profile is reset");
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}
void SaraR5::setHTTPserver(int profile, int OP_CODE, String serverName)
{

    ATcommand = SARA_R5_HTTP_PROFILE ;
    ATcommand += "=" + String(profile) + "," + String(OP_CODE) +","+ "\""+ serverName + "\""; // profile 0-4
    //ATcommand = "AT+UHHTP=0,1,\"api.thingspeak.com/channels/1555456/bulk_update.json\"";
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand);
        //Serial1.println("AT+UHHTP=0,1,\"api.thingspeak.com/channels/1555456/bulk_update.json\""); //
        //SerialUSB.println("Setting server Name");//
        //SerialUSB.println(serverName);//
        SerialUSB.println("Setup API URL ");//

        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}
void SaraR5::setHTTPserverPort(int profile, int OP_CODE, int serverPort)
{

    ATcommand = SARA_R5_HTTP_PROFILE ;
    ATcommand += "=" + String(profile) + "," + String(OP_CODE) +","+ String(serverPort)  ; // profile 0-4
    //ATcommand = "AT+UHHTP=0,1,\"api.thingspeak.com/channels/1555456/bulk_update.json\"";
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand);
        //Serial1.println("AT+UHHTP=0,1,\"api.thingspeak.com/channels/1555456/bulk_update.json\""); //
        //SerialUSB.println("Setting server Name");//
        //SerialUSB.println(serverName);//
        SerialUSB.println("Setup Server port  ");//

        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::listFiles()
{

    ATcommand = "AT+ULSTFILE" ;
    ATcommand +=  "="; // profile 0-4
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("List files");//
        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::readFiles()
{

    ATcommand = "AT+URDFILE" ;
    ATcommand +=  "=\"post_ATresponse.txt\""; // profile 0-4
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("Read file");//
        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf(":");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::tester()
{

    ATcommand = "AT+UMETRIC=3,2048" ; // Defines what cell information we want to read, and how to read it 
    //ATcommand = "AT+CSQ" ;

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("Test ATcommand active ");//
        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::setHTTPsecure(int profile, boolean secure)
{

    ATcommand = SARA_R5_HTTP_PROFILE ;
    ATcommand += "=" + String(profile) + "," + SARA_R5_HTTP_OP_CODE_SECURE +","+ secure  ; // profile 0-4
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("Setting server security");//
        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
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

void SaraR5::sendHTTPPOSTdata(int profile, String path, String ATresponseFilename,
                              String data, int httpContentType)
{
    //SerialUSB.println("Send HTTP1");
    ATcommand = SARA_R5_HTTP_COMMAND;
    ATcommand +=  "=" + String(profile) + "," + SARA_R5_HTTP_COMMAND_POST_DATA +","+ "\""+path + "\"" + "," +  "\"" + ATresponseFilename + "\"" + "," + "\"" + data + "\"" + "," + String(httpContentType) ; // profile 0-4
    //SerialUSB.println(ATcommand); //
    int counter = 0;
    int ATresponseCounter; 
    
    Serial1.flush();
        
    //SerialUSB.println("Send HTTP POST data");//
    
    Serial1.println(ATcommand); //
    SerialUSB.println("Sending message" + ATcommand); //
    
        
    while(counter < 15 )
    {
        

        ATresponseCounter= 0; 
        while(!Serial1.available() && ATresponseCounter<100)
        {
            delay(10);
            ATresponseCounter = ATresponseCounter + 1; 
        }
        SerialUSB.println("ATresponse counter : " + String(ATresponseCounter));
        
        if (Serial1.available()){
            
            
            ATresponse = Serial1.readString();    
            

            int index = ATresponse.indexOf("UUHTTPCR");
            //ATresponse = ATresponse.substring(index,ATresponse.length());
            //SerialUSB.println(ATresponse);

            if(index > -1) //IF "OK" is received   
            {
                //ATresponse = "OK "; //
                SerialUSB.println("ATresponse UUHTTPCR :" + ATresponse);
                break;
            }
            else
            {
                SerialUSB.println("Not Ready : " + ATresponse);
            }
        }
        counter = counter + 1;
    }
    //SerialUSB.println("Got ATresponse "); //
    

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
    if(hexEncodedHeading.length()<2){
        hexEncodedHeading = "0" + hexEncodedHeading;
    }

    // SerialUSB.println("Heading Q" + String(droneID->heading));
    
    // Altitude AMSL_GNSS
    float valueAMSL_GNSS = String(droneID->altitude,2).toFloat(); 
    uint16_t encodedAMSL_GNSS = 0; 
    encodedAMSL_GNSS = round((valueAMSL_GNSS+1000)/0.5);
    String hexEncodedAMSL_GNSS = String(encodedAMSL_GNSS,HEX);
    while(hexEncodedAMSL_GNSS.length()<4)
    {
        hexEncodedAMSL_GNSS = "0" + hexEncodedAMSL_GNSS; 
    }
    // Altitude AGL BARO 
    float valueAGL_Baro = droneID->AGL_Baro;
    uint16_t encodedAGL_Baro = 0; 
    encodedAGL_Baro = round((valueAGL_Baro+1000)/0.5);
    String hexEncodedAGL_Baro = String(encodedAGL_Baro,HEX);
    while(hexEncodedAGL_Baro.length()<4)
    {
        hexEncodedAGL_Baro = "0" + hexEncodedAGL_Baro; 
    }

       // Altitude AMSL BARO 
    float valueAMSL_Baro = droneID->AMSL_Baro;
    uint16_t encodedAMSL_Baro = 0; 
    encodedAMSL_Baro = round((valueAMSL_Baro+1000)/0.5);
    String hexEncodedAMSL_Baro = String(encodedAMSL_Baro,HEX);
    while(hexEncodedAMSL_Baro.length()<4)
    {
        hexEncodedAMSL_Baro = "0" + hexEncodedAMSL_Baro; 
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
    // RSSI encoding 
    String hexRSSI = String(rssi, HEX);
    while(hexRSSI.length()<2)
    {
        hexRSSI = "0" + hexRSSI; 
    }




    // Encoded 
    SerialUSB.println(serialNumber + " Flags: " + String(EncodedFlags) + " Speed :" + String(speedFloat) + " Heading : " + String(valueHeading) + " LAT:"+ String(droneID->latitude,7) + " Lon:" + String(droneID->longitude,7) + " AMSL_GNSS: " + String(valueAMSL_GNSS) + " AGL_Baro: "+ String(valueAGL_Baro) + " AMSL_Baro: " + String(valueAMSL_Baro) +  " HDOP:" + String(valueHDOP) + " VDOP:" + String(valueVDOP) + " Time:" + String(valueTime) + " RSSI: " + String(rssi));
    SerialUSB.println(serialNumber + " Flags: " + String(EncodedFlags, HEX) + " Speed :" + hexEncodedSpeed + " Heading : " + hexEncodedHeading + " LAT:"+ String(hexEncodedLat) + " Lon:" + String(hexEncodedLong) + " AMSL_GNSS: " + String(hexEncodedAMSL_GNSS) + " AGL_Baro: " + String(hexEncodedAGL_Baro) +"AMSL_Baro: " + String(hexEncodedAMSL_Baro) + " HDOP:" + String(hexEncodedHDOP) + " VDOP:" + String(hexEncodedVDOP) + " Time:" + String(hexEncodedTime) + " RSSI: " + String(hexRSSI));
    outputString =  serialNumber + String(EncodedFlags, HEX) + hexEncodedSpeed + hexEncodedHeading + hexEncodedLat + hexEncodedLong + hexEncodedAGL_Baro + hexEncodedHDOP + hexEncodedVDOP + hexEncodedTime + hexRSSI + hexEncodedAMSL_GNSS; 
    
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
    // sendHTTPPOSTdata(HTTPprofile, "/update", "post_ATresponse1.txt", payload1, SARA_R5_HTTP_CONTENT_APPLICATION_X_WWW);
    // sendHTTPPOSTdata(HTTPprofile, "/update", "post_ATresponse2.txt", payload2, SARA_R5_HTTP_CONTENT_APPLICATION_X_WWW);
    // sendHTTPPOSTdata(HTTPprofile, "/update", "post_ATresponse3.txt", payload3, SARA_R5_HTTP_CONTENT_APPLICATION_X_WWW);

    //payload1 = "FFFF06140F2104C04A063121BE03FD03FD03FD00000E1B" //46 bytes HEX ENCODED
    payload1 = encodeMessage();//"Heej fra DroneID5G"; //52 bytes deciman

    //                  For SDU node js server 
    // FOR SDU SERVER 
    sendHTTPPOSTdata(HTTPprofile, "/post-test", "post_ATresponse.txt", payload1, SARA_R5_HTTP_CONTENT_TEXT_PLAIN);


    //              READ ATresponse 
    //listFiles();

    //readFiles();

}


int SaraR5::socketOpen(int protocol_OP_code, int local_port)
{
    int socketIDoutput = -1; 
    ATcommand = SARA_R5_CREATE_SOCKET;
    ATcommand += "=" + String(protocol_OP_code) + "," + String(local_port);

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("Setting up Socket for UDP/TCP");//
        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                
                socketIDoutput = ATresponse.substring(ATresponse.indexOf("+USOCR:")+7,ATresponse.indexOf("+USOCR:")+9).toInt();
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
    return socketIDoutput;
}
void SaraR5::SendtoCommandUDP(int socketID, String remote_IP_adress, int remote_port, int bytesWritten, String dataString)
{
    //ATcommand = "AT+USOST=0,\"130.225.157.81\",8080,3,\"Ab1234\"";
    ATcommand = SARA_R5_WRITE_UDP_SOCKET; //"AT+USOST=0,\"130.225.157.81\",8080,38,\"AB1234800002104e3c60632408507c9070b681\""; //SARA_R5_WRITE_UDP_SOCKET; //
    ATcommand += "=" + String(socketID) + ",\"" + remote_IP_adress + "\"," + String(remote_port) + "," + String(bytesWritten) + ",\"" + dataString + "\""  ;
    int ATresponseCounter = 0; 
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.print("SendTo UDP message");//
        
        ATresponseCounter= 0; 
        while(!Serial1.available() && ATresponseCounter<100)
        {
            delay(10);
            ATresponseCounter = ATresponseCounter + 1; 
        }

        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());
            
            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(" - got OK " + dataString);
                
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
                if(ATresponse.indexOf("ERROR")>-1)
                {
                    ReConnect();
                }
                
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::getSocketError()
{
    ATcommand = "AT+USOER";
    //ATcommand += "=" + String(socketID) + ",\"" + remote_IP_adress + "\"," + String(remote_port) + "," + String(bytesWritten) + ",\"" + dataString + "\""  ;

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("Socket Error");//
        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}
void SaraR5::HEXconfig(int bool1)
{
    ATcommand = "AT+UDCONF=1,";
    ATcommand += String(bool1); //"=" + String(socketID) + ",\"" + remote_IP_adress + "\"," + String(remote_port) + "," + String(bytesWritten) + ",\"" + dataString + "\""  ;

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("Hex config");//
        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}
void SaraR5::ReceiveFromCommandUDP(int socketID_)
{
    ATcommand = SARA_R5_READ_UDP_SOCKET;
    ATcommand += "=" + String(socketID_) + ",0"; //"=" + String(socketID) + ",\"" + remote_IP_adress + "\"," + String(remote_port) + "," + String(bytesWritten) + ",\"" + dataString + "\""  ;

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("ReveiveFromCommandUDP");//
        delay(2000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::SetListeningSocket(int socketID_)
{
    ATcommand = "AT+USOLI";
    ATcommand += "=" + String(socketID_) + ",54321"; //"=" + String(socketID) + ",\"" + remote_IP_adress + "\"," + String(remote_port) + "," + String(bytesWritten) + ",\"" + dataString + "\""  ;

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("SetListeningSocket");//
        delay(2000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}
void SaraR5::connectSocket(int socketID_, String IPadress, int remotePort)
{
    ATcommand = "AT+USOCO";
    ATcommand += "=" + String(socketID_) + ",\"" + IPadress + "\"," + String(remotePort); //"=" + String(socketID) + ",\"" + remote_IP_adress + "\"," + String(remote_port) + "," + String(bytesWritten) + ",\"" + dataString + "\""  ;

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("connectSocket");//
        delay(2000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::writeSocketData(int socketID_, int dataLength, String data1)
{
    ATcommand = "AT+USOWR";
    ATcommand += "=" + String(socketID_) + "," + String(dataLength) + ",\"" + data1 + "\""; //"=" + String(socketID) + ",\"" + remote_IP_adress + "\"," + String(remote_port) + "," + String(bytesWritten) + ",\"" + dataString + "\""  ;
    int ATresponseCounter; 
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("writeSocketData");//
        //delay(2000);
        ATresponseCounter= 0; 
        while(!Serial1.available() && ATresponseCounter<100)
        {
            delay(10);
            ATresponseCounter = ATresponseCounter + 1; 
        }


        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}

String SaraR5::readSocketData(int socketID_, int dataLength)
{
    ATcommand = "AT+USORD";
    ATcommand += "=" + String(socketID_) + "," + String(dataLength); //"=" + String(socketID) + ",\"" + remote_IP_adress + "\"," + String(remote_port) + "," + String(bytesWritten) + ",\"" + dataString + "\""  ;
    int ATresponseCounter; 
    int counter = 0;
    String OutputString = ""; 
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.print("readSocketData"); //

        ATresponseCounter= 0; 
        while(!Serial1.available() && ATresponseCounter<100)
        {
            delay(10);
            ATresponseCounter = ATresponseCounter + 1; 
        }

        //delay(100);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());
           
            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                //OutputString = ATresponse.substring(ATresponse.indexOf(",\"")+2,ATresponse.length()); // ATresponse.indexOf("\"\n")-3);
                //OutputString = OutputString.substring(0, OutputString.indexOf("\"")-1);
                
                SerialUSB.println(" - got OK");
                break;
            }
            else
            {
                SerialUSB.println("NO OK ?  " + ATresponse);
            }
        }
        counter = counter + 1;


    }
    OutputString = ATresponse.substring(ATresponse.indexOf(",\"")+2,ATresponse.length()); // ATresponse.indexOf("\"\n")-3);
    OutputString = OutputString.substring(0, OutputString.indexOf("\""));
    
    //SerialUSB.println("Test+ " + OutputString);
    if(OutputString.length()<10)
    {
        SocketControl(socketID1,10); 
    }


    return OutputString;
    
}
void SaraR5::getIPaddress()
{
    ATcommand = "AT+CGDCONT?";
   // ATcommand += "=" + String(socketID_) + "," + String(dataLength); //"=" + String(socketID) + ",\"" + remote_IP_adress + "\"," + String(remote_port) + "," + String(bytesWritten) + ",\"" + dataString + "\""  ;

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("getIPaddress");//
        delay(500);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}


void SaraR5::SocketControl(int socketID_,int param_id)
{
    ATcommand = "AT+USOCTL";
    ATcommand += "=" + String(socketID_) + "," + String(param_id); //"=" + String(socketID) + ",\"" + remote_IP_adress + "\"," + String(remote_port) + "," + String(bytesWritten) + ",\"" + dataString + "\""  ;
    int ATresponseCounter; 
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.print("SocketControl"); //

        ATresponseCounter= 0; 
        while(!Serial1.available() && ATresponseCounter<100)
        {
            delay(10);
            ATresponseCounter = ATresponseCounter + 1; 
        }

        //delay(100);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());
           
            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                //OutputString = ATresponse.substring(ATresponse.indexOf(",\"")+2,ATresponse.length()); // ATresponse.indexOf("\"\n")-3);
                //OutputString = OutputString.substring(0, OutputString.indexOf("\"")-1);
                if(ATresponse.indexOf("4")>-1)
                {
                    SerialUSB.println(" - TCP connection is in ESTABILISHED status");
                }
                else 
                {
                    SerialUSB.println("- TCP is NOT  in connection ESTABILISHED status");
                    SerialUSB.println(ATresponse);
                    connectSocket(socketID1, serverPort, 7070); 
                }
               
                    
                break;
            }
            else
            {
                SerialUSB.println("NO OK ?  " + ATresponse);
            }
        }
        counter = counter + 1;


    }
    
    //SerialUSB.println("Test+ " + OutputString);
    
    
}



void SaraR5::SetupULOC()
{
    ATcommand = "AT+ULOCGNSS=15";
    Serial1.println(ATcommand);
    // delay(500);
    // ATcommand = "AT+UTIME=1,1";
    // Serial1.println(ATcommand);
    
    
    int ATresponseCounter; 
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.print("SetupULOC"); //

        ATresponseCounter= 0; 
        while(!Serial1.available() && ATresponseCounter<100)
        {
            delay(10);
            ATresponseCounter = ATresponseCounter + 1; 
        }

        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());
           
            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                //OutputString = ATresponse.substring(ATresponse.indexOf(",\"")+2,ATresponse.length()); // ATresponse.indexOf("\"\n")-3);
                //OutputString = OutputString.substring(0, OutputString.indexOf("\"")-1);
               
                SerialUSB.println(ATresponse);
                    
                break;
            }
            else
            {
                SerialUSB.println("NO OK ?  " + ATresponse);
            }
        }
        counter = counter + 1;


    }
    
    //SerialUSB.println("Test+ " + OutputString);
}

void SaraR5::GetULOCPOS()
{
    ATcommand = "AT+ULOC=2,3,0,1,99999,1";
    int ATresponseCounter; 
    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.print("GetULOCPOS"); //

        ATresponseCounter= 0; 
        while(!Serial1.available() && ATresponseCounter<100)
        {
            delay(10);
            ATresponseCounter = ATresponseCounter + 1; 
        }

        delay(1000);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());
           
            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                //OutputString = ATresponse.substring(ATresponse.indexOf(",\"")+2,ATresponse.length()); // ATresponse.indexOf("\"\n")-3);
                //OutputString = OutputString.substring(0, OutputString.indexOf("\"")-1);
               
                SerialUSB.println(ATresponse);
                    
                break;
            }
            else
            {
                SerialUSB.println("NO OK ?  " + ATresponse);
            }
        }
        counter = counter + 1;


    }
    
    //SerialUSB.println("Test+ " + OutputString);
}



void SaraR5::SilentReset() {
    ATcommand = "AT+CFUN=16";
   // ATcommand += "=" + String(socketID_) + "," + String(dataLength); //"=" + String(socketID) + ",\"" + remote_IP_adress + "\"," + String(remote_port) + "," + String(bytesWritten) + ",\"" + dataString + "\""  ;

    int counter = 0;
    while(counter < 4 )
    {
        Serial1.println(ATcommand); //
        SerialUSB.println("SiletnReset");//
        delay(500);
        if (Serial1.available()){
            ATresponse = Serial1.readString();
            //Serial1.println(ATresponse);
            int index = ATresponse.indexOf("OK");
            //ATresponse = ATresponse.substring(index,ATresponse.length());

            if(index > -1) //IF "OK" is received
            {
                //ATresponse = "OK "; //
                SerialUSB.println(ATresponse);
                break;
            }
            else
            {
                SerialUSB.println(ATresponse);
            }
        }
        counter = counter + 1;
    }
}

void SaraR5::ReConnect() {
    
    SerialUSB.println("Reconnecting... ");
    SilentReset();

    waitForLTESignal(); //PÅ NORMALT 
    //enableCellInformation();

    //HTTP
    setupPSDprofile();  //PÅ Normalt 

    socketID = socketOpen(SARA_R5_UDP, 54321);    // UDP 
    socketID1 = socketOpen(SARA_R5_TCP, 55555);   // PÅ NORMALT VED TCP FORBINDELSE 


    SerialUSB.println("SocketID UDP: "+ String(socketID));

    SerialUSB.println("SocketID TCP: "+ String(socketID1));

    connectSocket(socketID1, serverPort, 7070); // På normalt når der en TCP ofrbindelse oprettet
  
}

void SaraR5::init() { //Setup
    
   
    // Function used to setup and initialized the device
    pinMode(LED_BUILTIN, OUTPUT); // Setup LED
    

    beginSerial();
    
    turnLED(HIGH);

    waitForBoardOK(); // På normalt 
    checkSimCardId(); // På normalt 

    droneID->uploadGeoFence();

    //searchForNetworks(); // Er ikke på normalt 

      /// HTTP



   // enableGPS(); // Er på Normal 

    waitForLTESignal(); //PÅ NORMALT 
    //enableCellInformation();

    //HTTP
    setupPSDprofile();  //PÅ Normalt 

    //setupHTTP(); // PÅ NORMALT VED HTTP 

  //  HybridPositioningSetup();
  //  HybridPositioning();
    

    //tester();

    //erialUSB.println("socket");
    HEXconfig(1); // 1= HEX MODE // PÅ Normalt

    socketID = socketOpen(SARA_R5_UDP, 54321);
    socketID1 = socketOpen(SARA_R5_TCP, 55555);   // PÅ NORMALT VED TCP FORBINDELSE 


    SerialUSB.println("SocketID: "+ String(socketID));

    connectSocket(socketID1, serverPort, 7070); // På normalt når der en TCP ofrbindelse oprettet
    //connectSocket(socketID, serverPort, 8080);

    //SetupULOC();
    while(1)
    {
       // SetupULOC();
        GetULOCPOS();
    }
    
    
    getGPSPosition(); 

    SerialUSB.println("Finding GNSS fix");
    while (GNSSStatus != 'A')
    {
        getGPSPosition(); //Gør sådan at den at den først får positionen hvis der er fix
        delay(200);
    }
    
    payload1 = encodeMessage();

   // writeSocketData(socketID1, 38, payload1);

    //SendtoCommandUDP(socketID, serverPort, 8080,19, payload1);

    //SetListeningSocket(socketID);
    droneID->BarometerSetup();
    SerialUSB.println("Init done ");
}


void SaraR5::main() {

    //
    turnLED(HIGH);
    //SerialUSB.println("Hej");

    droneID->BarometerRead();

    getGPSPosition(); //Gør sådan at den at den først får positionen hvis der er fix
    //HybridPositioning();

    //SerialUSB.println("GPS done ");
    //SerialUSB.println(encodeMessage());
    payload1 = encodeMessage(); // PÅ NORMALT
   // writeSocketData(socketID1, 38, payload1);

   // delay(33);
    SendtoCommandUDP(socketID, serverPort, 8080,22, payload1); //Normalt På sender på UDP port 
    //getSocketError();
    //readSocketData(socketID1,192); // 24*8 = 192 // IKKE PÅ NORMALT 

    droneID->DecodeVehicle(readSocketData(socketID1,192));
    //sendHTTPusingPOST();
    //   delay(300);
    

    //SetListeningSocket(socketID);
   //getIPaddress();
    delay(185);
    if (Serial1.available()){
        ATresponse = Serial1.readString();
        //Serial1.println(ATresponse);
        int index = ATresponse.indexOf("OK");
        //ATresponse = ATresponse.substring(index,ATresponse.length());

        if(index > -1) //IF "OK" is received
        {
            //ATresponse = "OK "; //
            SerialUSB.println(ATresponse);
            
        }
        else
        {
            SerialUSB.println(ATresponse);
        }
    }
    RSSI();

   // ReceiveFromCommandUDP(socketID);
    turnLED(LOW);
    SerialUSB.println(GNSSStatus);

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
    
    SerialUSB.println(" ");
    
    SerialUSB.println(" ");    
}
