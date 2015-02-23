/******************************************************************
* Description: ESP8266 library for the Teensy3.x device family
* version: 0.1
* modified: 02/22/2015
* modified by: Jim Mayhugh
*******************************************************************
* Description: ESP8266 Teensy3.x Library
* This library helps users working with the WiFi module ESP8266.
* It manages the AT command messages to communicate and configure 
* the module.
* It also manages the communication process by extracting the messages 
* sent through the serial port
*******************************************************************
* Original Arduino Author = Angel Hernandez
* Contributors = Angel Hernandez
* License = GPL
* Contact = angel@tupperbot.com
* 			@mifulapirus
*******************************************************************/

#include <ESP8266.h>

/*******************************************************************
* CONSTRUCTOR
* Sets some variables for the serial port, reset pin, baud rate and
* debug port
********************************************************************/

ESP8266::ESP8266(uint8_t _port, uint8_t _rstPin, uint32_t _initBaud, uint8_t _debug)
{
  debug     = _debug;
  port      = _port;
	rstPin    = _rstPin;
	baudRate  = _initBaud;
	IP="0.0.0.0";
	pinMode(rstPin, OUTPUT);
}


/*******************************************************************
* ESP8266 portInit
* configures the serial and debug port
*******************************************************************/
void ESP8266::portInit(void)
{
  switch(debug)
  {
    case SP1:
    {
      Serial1.begin(baudRate);
      myDEBUG = &Serial1;
      break;
    }
    
    case SP2:
    {
      Serial2.begin(baudRate);
      myDEBUG = &Serial1;
      break;
    }
    
    case SP2A:
    {
      // reassign pins 26 and 31 to use the ALT3 configuration
      // which makes them  Serial port 2 Rx(26) and Tx(31)
      CORE_PIN26_CONFIG = PORT_PCR_MUX(3);
      CORE_PIN31_CONFIG = PORT_PCR_MUX(3);
      Serial2.begin(baudRate);
      myDEBUG = &Serial2;
      break;
    }
    
    case SP3:
    {
      Serial3.begin(baudRate);
      myDEBUG = &Serial3;
      break;
    }
    
    default: // any other value defaults to the Arduino IDE Serial monitor
    {
      Serial.begin(baudRate);
      myDEBUG = &Serial;
      break;
    }    
  }

  myDEBUG->print(F("myDEBUG initialized at "));
  myDEBUG->print(baudRate);
  myDEBUG->println(F(" baud"));

  switch(port)
  {
    case SP1:
    {
      Serial1.begin(baudRate);
      myESP8266 = &Serial1;
      myDEBUG->print(F("Serial1 initialized at "));
      myDEBUG->print(baudRate);
      myDEBUG->println(F(" baud"));
      break;
    }
    
    case SP2:
    {
      Serial2.begin(baudRate);
      myESP8266 = &Serial1;
      myDEBUG->print(F("Serial2 initialized at "));
      myDEBUG->print(baudRate);
      myDEBUG->println(F(" baud"));
      break;
    }
    
    case SP2A:
    {
      // reassign pins 26 and 31 to use the ALT3 configuration
      // which makes them  Serial port 2 Rx(26) and Tx(31)
      CORE_PIN26_CONFIG = PORT_PCR_MUX(3);
      CORE_PIN31_CONFIG = PORT_PCR_MUX(3);
      Serial2.begin(baudRate);
      myESP8266 = &Serial2;
      myDEBUG->print(F("Serial2A initialized at "));
      myDEBUG->print(baudRate);
      myDEBUG->println(F(" baud"));
      break;
    }
    
    case SP3:
    {
      Serial3.begin(baudRate);
      myESP8266 = &Serial3;
      myDEBUG->print(F("Serial3 initialized at "));
      myDEBUG->print(baudRate);
      myDEBUG->println(F(" baud"));
      break;
    }
    
    default:
    {
      myDEBUG->println(F("No Serial Port Initialized"));
      break;
    }    
  }
}

/*******************************************************************
* INITIALIZATION
* This is the initialization routine for the module. In case of 
* error, it will exit with the proper error code (see table on 
* the ESP8266.h file)
********************************************************************/
uint8_t ESP8266::init(String _SSID, String _pass) {
  if (reboot() != NO_ERROR) {return ERROR_REBOOTING;}	
  if (wifiMode(1) != NO_ERROR)	{return ERROR_WIFI_MODE;} 
  if (connect(_SSID, _pass) != NO_ERROR) {return ERROR_UNABLE_TO_CONNECT;}
  getIP();
  return NO_ERROR;
}

/*******************************************************************
* REBOOT
* Hardware reset of the module. You must have connected the Reset 
* pin to some pin on your micro-controller and provide it as an 
* argument on the constructor
********************************************************************/
byte ESP8266::reboot() {
  digitalWrite(rstPin, LOW);
  delay(50);
  digitalWrite(rstPin, HIGH);
  return expectResponse(AT_RESP_READY);
}

/*******************************************************************
* RESET
* Software reset of the module. this routine might not work if the 
* ESP8266 is not responding to any command, in which case, a reboot() 
* would be required
********************************************************************/
byte ESP8266::reset() {
  myESP8266->flush();
  myESP8266->println(AT_CMD_RST); // reset and test if module is ready
  return expectResponse(AT_RESP_READY);
}

/*******************************************************************
* CHECK WIFI
* Simply sends the "AT" command and expects the "OK" response in 
* in order to make sure the module is working fine.
********************************************************************/
uint8_t ESP8266::checkWifi() {
  myESP8266->println(AT_CMD_AT);
  if (expectResponse(AT_RESP_OK)) {return NO_ERROR;}
  else {return ERROR_MODULE_DOESNT_RESPOND_TO_AT;}
}

/*******************************************************************
* CONNECT TO WIFI NETWORK
* This function builds the connection string and waits for the 
* module to be connected.
* You must provide the correct WiFi SSID (Name) and Password
********************************************************************/
uint8_t ESP8266::connect(String _SSID, String _pass) {
  String _cmd = AT_CMD_JOIN_AP;
  _cmd += _SSID;
  _cmd += "\",\"";
  _cmd += _pass;
  _cmd += "\"";
  myESP8266->println(_cmd);
  return expectResponse(AT_RESP_OK);
}

/*******************************************************************
* SET THE SERVER MODE
* Configures the module in Server mode on the given port. From that 
* moment on, it will be possible to talk to the module on that port
* by opening a TCP connection on its IP (You can use Putty to do so)
********************************************************************/
uint8_t ESP8266::setServer(String _port) {
	if (connectionMode(AT_TRUE) != NO_ERROR) {return ERROR_CONNECTION_MODE;}
	String _cmd = AT_CMD_SERVER_MODE;
	serverPort = _port;
	_cmd += "1,";
	_cmd += _port;
	myESP8266->println(_cmd);
	return expectResponse(AT_RESP_OK);
}

/*******************************************************************
* CLOSE THE SERVER MODE
********************************************************************/
uint8_t ESP8266::closeServer() {
	if (connectionMode(AT_TRUE) != NO_ERROR) {return ERROR_CONNECTION_MODE;}
	String _cmd = AT_CMD_SERVER_MODE;
	_cmd += "0";
	myESP8266->println(_cmd);
	return expectResponse(AT_RESP_OK);
}

/*******************************************************************
* CONNECTION MODE
********************************************************************/
uint8_t ESP8266::connectionMode(String _mux) {
	String _cmd = AT_CMD_CONNECTION_MODE;
	_cmd += _mux;
	myESP8266->println(_cmd);
	return expectResponse(AT_RESP_OK);
}

/*******************************************************************
* WIFI MODE: AP or SAT
* The ESP8266 can be set as an Access Point (AP, _mode = 1) or as 
* a Station (STA, _mode=2) connected to the given WiFi (see connect())
* Most of the time you will use it just in STA mode so it connects to 
* your WiFi as any other device.
********************************************************************/
uint8_t ESP8266::wifiMode(int _mode) {
	myESP8266->print(AT_CMD_WIFI_MODE);
	myESP8266->println(_mode);
	return expectResponse(AT_RESP_NO_CHANGE);
}

/*******************************************************************
* GET IP
* Returns the given IP and sets the public IP variable.
********************************************************************/
String ESP8266::getIP() {
	//TODO: Check if the module is actually connected
	String _cmd = AT_CMD_GET_IP;
	String _msg="";
	myESP8266->println(_cmd); // Get IP
	_msg = readAll();
	IP = _msg.substring(11,_msg.length()-8);
	return IP;
}

/*******************************************************************
* SET TRANSMISSION MODE
* Sets the transmission mode to NORMAL or TRANSPARENT
//TODO: Explain the difference and talk about the +IPD
********************************************************************/
void ESP8266::setTxMode(boolean _value) {
  if (_value) {myESP8266->println(AT_CMD_CIPMODE_ON);}
  else {myESP8266->println(AT_CMD_CIPMODE_OFF);}
}

/*******************************************************************
* EXPECT A GIVEN RESPONSE
* Waits for the module to provide the given response. If there is no 
* answer or the answer does not contain the given String in the 
* specified responseTimeOut, the function will return with a 
* ERROR_NO_RESPONSE or ERROR_RESPONSE_NOT_FOUND
********************************************************************/
uint8_t ESP8266::expectResponse(char* _expected)
{
	String _received = AT_EMPTY_STRING;
	for (int i = 0; i < responseTimeOut; i++)
	{
		_received = readAll();
		if (_received != AT_EMPTY_STRING)
		{
			if (contains(_received, _expected))	return NO_ERROR;
		}
		delay(1);
	}
	if (_received == AT_EMPTY_STRING)
	{
	  return ERROR_NO_RESPONSE;
	}else{
	 return ERROR_RESPONSE_NOT_FOUND;
	}
}
		
/*******************************************************************
* READS ALL CHARACTERS FROM BUFFER
* Reads all characters sent BY the ESP8266 TO the micro-controller
* saves the response as the lastResponse, and returns with the String
********************************************************************/
String ESP8266::readAll() {
  char _inChar;
  //String _response = "";
  lastResponse = AT_EMPTY_STRING;
  while (myESP8266->available()) {
    _inChar = myESP8266->read();
    lastResponse += _inChar;
    myDEBUG->print(_inChar);
    delay(1);
  }
  //lastResponse = _response;
  return lastResponse;
}

/*******************************************************************
* READS A SPECIFIC COMMAND
* This function reacts to the provided AT commands, including the 
* +IPD extracting the characters from the message being received and 
* obtaining the current channel and length
//TODO: Do a better job at explaining this function. It's just too late.
//TODO: consider dividing the AT_RESP_IPD into an isolated function
********************************************************************/
String ESP8266::readCmd() {
	String _received = readAll();
	if (_received != AT_EMPTY_STRING) {
		if (contains(_received, AT_RESP_NO_CHANGE)) 	{return AT_RESP_NO_CHANGE;}
		else if (contains(_received, AT_RESP_LINK)) 	{return AT_RESP_LINK;}
		else if (contains(_received, AT_RESP_UNLINK)) 	{return AT_RESP_UNLINK;}
		else if (contains(_received, AT_RESP_IPD)) 	{
			//FIND Channel, length and Msg
			String _channel = _received.substring(_received.indexOf(',')+1);
			String _length = _channel;
			_length = _length.substring(_channel.indexOf(',')+1, _channel.indexOf(':'));
			_channel = _channel.substring(0,_channel.indexOf(','));
			String _msg = _received.substring(_received.indexOf(':')+1, _received.indexOf(':')+1 + _length.toInt());
			return _msg;
		}
		
		else {
			String _unknown = "Unk: " + _received;
			return _unknown;
		}
	}
	return _received;
}

/*******************************************************************
* CONTAINS A GIVEN STRING
* Checks if the "Original" string contains the "Search" string and 
* returns true or false.
********************************************************************/
boolean ESP8266::contains(String _original, String _search) {
    int _searchLength = _search.length();
	int _max = _original.length() - _searchLength;
    for (int i = 0; i <= _max; i++) {
        if (_original.substring(i, i + _searchLength) == _search) {return true;}
    }
    return false;
} 

/*******************************************************************
* OPEN TCP CONNECTION
* Opens a TCP connection to the given server on the _IP and _port 
* provided. 
//TODO: Implement the send function
********************************************************************/
uint8_t ESP8266::openTCP(String _IP, String _port, boolean _waitResponse) {
	String _cmd = AT_CMD_CIPSTART; 
	_cmd += _IP;
	_cmd += "\",";
	_cmd += _port;
	myESP8266->println(_cmd);
	if (_waitResponse) {
		delay(200);
		if (expectResponse(AT_RESP_LINK) != NO_ERROR) {return ERROR_UNABLE_TO_LINK;}
		return NO_ERROR;
	}
	else {return NO_ERROR;}
}


/*******************************************************************
* CLOSE TCP CONNECTION
* Closes a TCP connection.
********************************************************************/
uint8_t ESP8266::closeTCP() {
  myESP8266->println(AT_CMD_CLOSE_CONNECTION);
  if (expectResponse(AT_RESP_UNLINK) != NO_ERROR) {return ERROR_UNABLE_TO_UNLINK;}
  return NO_ERROR;
}

/*******************************************************************
* SEND LONG MESSAGE 
* Sends a long message provided on the WifiLongMessage variable
//TODO: This needs a lot of improvement, it is only useful to 
* abstract the user from the pointer
********************************************************************/
uint8_t ESP8266::sendLongMessage(char* _expected, boolean _waitResponse) {
	myESP8266->print(AT_CMD_SEND);
	myESP8266->println(wifiLongMessage.length());
	delay(200); 
	myESP8266->print(wifiLongMessage);
	if (_waitResponse) {
		if (expectResponse(_expected) == NO_ERROR) {return NO_ERROR;}
		else {return ERROR_SEND_LONG_MESSAGE;}
	}
	else {return NO_ERROR;}
}
