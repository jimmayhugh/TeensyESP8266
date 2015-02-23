/*
  ESP8266.h - Library for controlling the ESP8266 Wifi Module
  Created by Angel Hernandez, February 7, 2015.
  Released into the public domain.
*/

#ifndef ESP8266_h
#define ESP8266_h

#include <Arduino.h>

//Error Table
#define NO_ERROR	                          0
#define ERROR_UNABLE_TO_LINK				        1
#define ERROR_UNABLE_TO_UNLINK           	  2
#define ERROR_SEND_LONG_MESSAGE          	  3  
#define ERROR_MODULE_DOESNT_RESPOND			    4
#define ERROR_MODULE_DOESNT_RESPOND_TO_AT 	5
#define ERROR_RESPONSE_NOT_FOUND         	  6
#define ERROR_NO_RESPONSE					          7
#define ERROR_UNABLE_TO_CONNECT          	  8
#define ERROR_REBOOTING						          9
#define ERROR_WIFI_MODE						         10
#define ERROR_CONNECTION_MODE				       11
#define ERROR_NOT_AT_OK						         12

//AT Commands
#define AT_CMD_AT				        "AT"
#define AT_CMD_RST				      "AT+RST"
#define AT_CMD_CIPSTART       	"AT+CIPSTART=\"TCP\",\""
#define AT_CMD_JOIN_AP			    "AT+CWJAP=\""
#define AT_CMD_SERVER_MODE 		  "AT+CIPSERVER="
#define AT_CMD_CONNECTION_MODE	"AT+CIPMUX="
#define AT_CMD_WIFI_MODE		    "AT+CWMODE="
#define AT_CMD_GET_IP			      "AT+CIFSR"
#define AT_CMD_CIPMODE_ON		    "AT+CIPMODE=1"
#define AT_CMD_CIPMODE_OFF		  "AT+CIPMODE=0"
#define AT_CMD_CLOSE_CONNECTION	"AT+CIPCLOSE"
#define AT_CMD_SEND				      "AT+CIPSEND="

//AT Responses
#define AT_RESP				    "RCV: "
#define AT_RESP_READY		  "ready"
#define AT_RESP_IPD			  "+IPD"
#define AT_RESP_LINK		  "Link"
#define AT_RESP_UNLINK		"Unlink"
#define AT_RESP_OK     		"OK"
#define AT_RESP_SEND_OK		"SEND OK"
#define AT_RESP_NO_CHANGE	"no change"
#define AT_EMPTY_STRING		""
#define AT_TRUE				    "1"
#define AT_FALSE			    "0"

// Teensy3.x Serial Ports
#define  SP1  0x01
#define  SP2  0x02
#define  SP2A 0x2A
#define  SP3  0x03



class ESP8266
{
  public:
  // Constructor
  ESP8266(uint8_t _port, uint8_t _rstPin, uint32_t _initBaud, uint8_t debug);
      
	//Public Functions
  uint8_t init(String, String);
  uint8_t reboot();
  uint8_t reset();
  uint8_t checkWifi();
	uint8_t wifiMode(int);
	uint8_t setServer(String);
	uint8_t closeServer();
	uint8_t connectionMode(String);
  uint8_t connect(String _SSID, String _pass);
  String getIP();
  void setTxMode(boolean);
  uint8_t expectResponse(char*);
	String getResponse(char*);
	boolean contains(String _s, String _search);
  uint8_t openTCP(String, String, boolean _waitResponse = true);
  uint8_t closeTCP();
  uint8_t sendLongMessage(char*, boolean _waitResponse = true);
  char* wifiRead();
	String readAll();
	String readCmd();
  void wifiEcho();
  void sendDebug(String);
  void sendCommand(char *);
  void sendCommand(char *, uint32_t);
  void getResponse (uint32_t);
  void portInit(void);
	
	//Private variables
	String lastResponse;
	String IP;
	String serverPort;
	uint32_t responseTimeOut = 3000;
	String wifiLongMessage;
	Stream * myESP8266;
	Stream * myDEBUG;

  private:
	//Private Functions
	
	//Private Variables
	uint8_t  debug;
	uint8_t  port;
  uint8_t  rstPin;
	uint32_t baudRate; 
};


#endif
