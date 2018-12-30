/*********
  DaFlinos Project
  Controllare da remoto l'accensione e lo spegnimento dell'aria condizionata per il confort abitativo del mio hermano 
*********/

// Including the ESP8266 WiFi library
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Replace with your network details
const char* ssid = "************";
const char* password = "****************";

// Variable to store the HTTP request
String header;

//Variable to store the room, based on IP
String roomName;

//Variable to store the IP
String strIP;

// Auxiliar variables to store the current output state
String HvacState = "off";
uint8_t HvacMode = 1;
uint8_t HvacTemp = 25;

#define ONE_WIRE_BUS D1      // Data wire GPIO pin to use. GPIO5 (D1)
const uint16_t kIrLed = D2;  // ESP8266 GPIO pin to use. GPIO4 (D2).
IRSamsungAc ac(kIrLed);    // Set the GPIO to be used to sending the message

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);
char temperatureCString[7];

// Web Server on port 80
WiFiServer server(80);

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  ac.begin();
  Serial.begin(115200);
  delay(10);

  DS18B20.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
  
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(5000);
  
  // Printing the ESP IP address
  strIP=WiFi.localIP().toString().c_str();
  Serial.println(strIP);
  if (strIP=="192.168.1.3") {
    roomName = "Sala";
    } else {roomName="Camera";}
  roomName = roomName + " - " + strIP;
}

void getTemperature() {
  float tempC;
  float tempF;
  do {
    DS18B20.requestTemperatures(); 
    tempC = DS18B20.getTempCByIndex(0);
    dtostrf(tempC, 2, 2, temperatureCString);
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));
}

// runs over and over again
void loop() {
  // Listenning for new clients
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the AC on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("AC is on");
              HvacState = "on";

                Serial.println("Sending...");

                // Set up what we want to send. See ir_Samsung.cpp for all the options.
                ac.on();
                ac.setFan(kSamsungAcFanAuto);
                ac.setMode(kSamsungAcCool);
                ac.setTemp(HvacTemp);
                ac.setSwing(false);
                
                // Display what we are going to send.
                Serial.println(ac.toString());
                // Now send the IR signal.
                #if SEND_SAMSUNG
                  ac.send();
                  Serial.println("INVIO");
                #endif  // SEND_SAMSUNG
                //delay(1000);
              
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("AC is off");
              HvacState = "off";
              Serial.println("Sending...");

                // Set up what we want to send. See ir_Samsung.cpp for all the options.
                ac.off();
                ac.setFan(kSamsungAcFanAuto);
                ac.setMode(kSamsungAcCool);
                ac.setTemp(HvacTemp);
                ac.setSwing(false);
                
                // Display what we are going to send.
                Serial.println(ac.toString());
                // Now send the IR signal.
                #if SEND_SAMSUNG
                  ac.send();
                  Serial.println("INVIO");
                #endif  // SEND_SAMSUNG
                //fdelay(1000);
              
              
            } else if (header.indexOf("GET /4/caldo") >= 0) {
              if (HvacState == "off") {
                Serial.println("Ac Mode CALDO");
                HvacMode = 4;
              }
            } else if (header.indexOf("GET /4/freddo") >= 0) {
              if (HvacState == "off") {
                Serial.println("Ac Mode FREDDO");
                HvacMode = 1;
              }
            } else if (header.indexOf("GET /3/minus") >= 0) {
              Serial.println("Temp -");
              if (HvacTemp > 16) {
                  HvacTemp = HvacTemp - 1;
              }
              if (HvacState == "on") {
              Serial.println("Sending...");
              ac.on();
              ac.setFan(kSamsungAcFanAuto);
              ac.setMode(kSamsungAcCool);
              ac.setTemp(HvacTemp);
              ac.setSwing(false);
                
              // Display what we are going to send.
              Serial.println(ac.toString());
              // Now send the IR signal.
              #if SEND_SAMSUNG
                 ac.send();
                 Serial.println("INVIO");
              #endif  // SEND_SAMSUNG
              //delay(500);
              }
            } else if (header.indexOf("GET /3/plus") >= 0) {
              Serial.println("Temp +");
              if (HvacTemp < 30) {
                  HvacTemp = HvacTemp + 1;
              }
                            if (HvacState == "on") {
              Serial.println("Sending...");
              ac.on();
              ac.setFan(kSamsungAcFanAuto);
              ac.setMode(kSamsungAcCool);
              ac.setTemp(HvacTemp);
              ac.setSwing(false);
                
              // Display what we are going to send.
              Serial.println(ac.toString());
              // Now send the IR signal.
              #if SEND_SAMSUNG
                 ac.send();
                 Serial.println("INVIO");
              #endif  // SEND_SAMSUNG
              //delay(500);
              }
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}");
            client.println(".button3 {background-color: #0000FF;}");
            client.println(".button4 {background-color: #CC0000;}");
            client.println(".button5 {background-color: #0066CC;}</style></head>");

                        
            // Web Page Heading
            client.println("<body><h1>Da Flino's - " + roomName + "</h1>");

            getTemperature();
            client.println("<p>Temperatura attuale </p>");
            client.println("<p><a href=\"/6/on\"><button class=\"button button5\">");
            client.println(temperatureCString);
            client.println("</button></a></p>");

            client.println("<p>Modalita' AC </p>");
                                                            
            // Display current state, and ON/OFF button  
            if (HvacState=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button\">ON</button></a></p>");
            } 
               
            // Display current mode, and COOL/HEAT button  
            // If the AcMode is Cool, it displays the Cool button      
            Serial.println(HvacMode); 
            if (HvacMode==1) {
              client.println("<p><a href=\"/4/caldo\"><button class=\"button button3\">FREDDO</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/freddo\"><button class=\"button button4\">CALDO</button></a></p>");
            }

            client.println("<p><a href=\"/3/minus\"><button class=\"button button5\">-</button></a></p>");
            client.println("<p><a href=\"/3/on\"><button class=\"button button5\">");
            client.println(HvacTemp);
            client.println("</button></a></p>");
            client.println("<p><a href=\"/3/plus\"><button class=\"button button5\">+</button></a></p>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}   
