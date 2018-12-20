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
const char* ssid = "Vodafone-skynet";
const char* password = "Rickyale.73";

#define ONE_WIRE_BUS 5      // Data wire GPIO pin to use. GPIO5 (D1)
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. GPIO4 (D2).
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
  Serial.println(WiFi.localIP());
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
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n' && blank_line) {
            getTemperature();
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            // your actual web page that displays temperature
            client.println("<!DOCTYPE HTML>");
            client.println("<HTML>");
            client.println("<HEAD><TITLE>Da Flinos - Camera da letto</TITLE></HEAD><body><H1>Da Flinos - Camera da letto</H1>Temperatura attuale: ");
            client.println(temperatureCString);
            client.println("gradi");  
            client.println("<br>");  
            client.println("<a href=\"/?ON\"\">ON</a><br />");
            client.println("</body></html>");  
            break;
            if (readString.indexOf("?ON") >0){
                Serial.println("Sending...");

                // Set up what we want to send. See ir_Samsung.cpp for all the options.
                ac.on();
                ac.setFan(kSamsungAcFanAuto);
                ac.setMode(kSamsungAcCool);
                ac.setTemp(25);
                ac.setSwing(false);
                
                // Display what we are going to send.
                Serial.println(ac.toString());
                // Now send the IR signal.
                #if SEND_SAMSUNG
                  ac.send();
                #endif  // SEND_SAMSUNG
                delay(5000);
           }
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}   
