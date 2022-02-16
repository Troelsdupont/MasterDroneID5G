
#ifndef SaraR5_h
#define SaraR5_h

#include "NaviairLib.h"
#include <TimeLib.h> /// Remember to install the library via the Arduino IDE "Manages libraries"

#define RMCCommas 13
#define GGACommas 15
#define GSACommas 3
#define UmetricCommas 40

#define SARA_R5_NUM_PDP_CONTEXT_IDENTIFIERS 11
#define SARA_R5_PSD_CONFIG_PARAM_MAP_TO_CID 100
#define SARA_R5_PSD_PROTOCOL_IPV4V6_V4_PREF 2
#define SARA_R5_PSD_CONFIG_PARAM_PROTOCOL 0
#define SARA_R5_PSD_ACTION_RESET 0
#define SARA_R5_PSD_ACTION_STORE 1
#define SARA_R5_PSD_ACTION_LOAD 2
#define SARA_R5_PSD_ACTION_ACTIVATE 3
#define SARA_R5_PSD_ACTION_DEACTIVATE 4
#define SARA_R5_HTTP_OP_CODE_SERVER_IP 0
#define SARA_R5_HTTP_OP_CODE_SERVER_NAME 1
#define SARA_R5_HTTP_OP_SECURE 6
#define SARA_R5_HTTP_CONTENT_APPLICATION_X_WWW 0
#define SARA_R5_HTTP_CONTENT_TEXT_PLAIN 1
#define SARA_R5_HTTP_CONTENT_APPLICATION_JSON 4
#define SARA_R5_HTTP_COMMAND_POST_DATA 5

#define SARA_R5_MESSAGE_PDP_DEF "AT+CGDCONT" // Packet switched Data Profile context definition
#define SARA_R5_MESSAGE_PDP_ACTION "AT+UPSDA" // Perform the action for the specified PSD profile
#define SARA_R5_MESSAGE_PDP_CONFIG "AT+UPSD" // Packet switched Data Profile configuration
#define SARA_R5_HTTP_PROFILE "AT+UHTTP" // Configure the HTTP profile. Up to 4 different profiles can be defined
#define SARA_R5_HTTP_COMMAND "AT+UHTTPC" // Trigger the specified HTTP command
class SaraR5
{
public:
    SaraR5(NaviairLib* _droneID);

    //Functions:
    void init();
    void main();

    void printDroneID(); // Ny

    // Variables:
    char GNSSStatus;
    int PSDprofile;
    int HTTPprofile;
    String command;
    String response;
    String serverName;
    String apiKey1;
    String apiKey2;
    String apiKey3;
    String payload1;
    String payload2;
    String payload3;

private:
    // Object:
    NaviairLib* droneID;

    // Variables:
    float rsrq;
    int rsrp;
    int rssi;
    String total_cell_reselections;
    String total_radioLinkLoss;
    String connEstablishAttemptCount;
    String connEstablishSuccessCount;
    String connEstablishFailureCount;
    String reestablishmentAttemptCount;
    String reestablishmentSuccessCount;
    String reestablishmentFailureCount;
    String HO_AttemptCount;
    String HO_SuccessCount;
    String HO_FailureCount;


    // Functions:
    void turnLED(char output);

    void beginSerial();
    void waitForLTESignal();
    void waitForBoardOK();
    void enableGPS();
    void checkSimCardId(); // Ny

    void searchForNetworks();
    void enableCellInformation(); // Ny
    void logLTEStats();
    void logUMetric();
    void RSSI();

    String getOperatorInfo();
    String checkStatus();
    void getGPSPosition();
    void parseGRMCMessage(String &str);
    void parseGGGAMessage(String &str); // Ny
    void parseGGSAMessage(String &str);

    void changeInGNSSFIX(char &status); // Ny

    void setupTimestamp(String &time, String &date);
    void setupLatitude(String &str, char &direction);
    void setupLongitude(String &str, char &direction);
    void setupSpeed(String &str); // Ny
    void setupHeading(String &str); // Ny
    void setupAltitude(String &str); // Ny

    bool checkIfComma(char &c);
    bool checkIfAsterisk(char &c); // Ny

    void tester();
    /// HTTP:
    void deactivatePSD(int cid);
    void setPDPconfiguration(int profile, int parameter, int value);
    void getAPN(int cid, String &apn, String &ip);
    void performPDPaction(int profile, int action);
    void setupPSDprofile();
    void configurePacketSwitchData();
    void resetHTTPprofile(int profile);
    void setHTTPserverName(int profile, String serverName);
    void setHTTPsecure(int profile, boolean secure);
    void setupHTTP();
    void sendHTTPPOSTdata(int profile, String path, String responseFilename, String data, int httpContentType);
    void sendHTTPusingPOST();
    void listFiles();
    void readFiles();

};

#endif