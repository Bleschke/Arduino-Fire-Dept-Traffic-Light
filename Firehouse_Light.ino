/*
 New email alarm/notification for google gmail account.
 Receives emails from Codemessaging.net
 
 Created December 8, 2011
 
 Based on the WebClient example by David A. Mellis,
 modified by Tom Igoe, based on work by Adrian McEwen.
 -----------------------------------------------------
 
 Modified by : Brian Leschke 13617
 
 Date: June 21, 2016
 Version 6.0
 
 Created to connect to server on Raspberry Pi or EasyPHP.
 This program will light up a traffic light at Co. 13 Fallston
 FVAC using corresponding colors for "leaving the station"
 actions.
 
 "Red" means WAIT (light turns on when call is received)
 "Yellow" means GET SET (turns on at 3 minutes) (red turns off)
 "Green" means GO (turns on at 5 minutes) (red/yellow off)
 "Green" stays on for 5 minutes

 This is designed to receive multiple fire/ems calls at any
 given time. (Up to 3 calls only)
 
 *** Information about HIGH and LOW values. ***
    Pins not connected to the relay use HIGH for ON and LOW for OFF.
    Pins connected to the relay use LOW for ON and HIGH for OFF.
    
 *** UPDATE HISTORY ***
 
 12/8/2011 - Initial release
 2012-2015 - Unspecified Updates
 6/19/2016 - "cleaned up", organized, and simplified code.
 6/19/2016 - ADDED UDP SERVER: This unit will send UDP packets to the receiver box located at my home.
             This is to make sure that the Traffic Light is working correctly and the network is active. 
 
 */
 
// ---------------- LIBRARIES ---------------- 
 
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
//#include <VirtualWire.h>
#include <stdlib.h>

// ---------------- PIN AND TIME CONFIGURATION ---------------- 

int powerLED      = 2;
int ledClient     = 3;
int ledConnect    = 4;
int ledError      = 5;
int TrafficRED    = 6;
int TrafficYELLOW = 7;
int TrafficGREEN  = 8;

char Str[11];
int prevNum  = 0;
int num      = 0;
long onUntil = 0;
long pollingInterval = 1000;   // polling time, in milliseconds
long onTimeGreen     = 299000; // time, in milliseconds, for Green light to be on after new alert.
long onTimeRed       = 179000; //  time, in milliseconds, for Red light to be on after new alert.
long onTimeYellow    = 119000; // time, in milliseconds, for Yellow light to be on after new alert.


// ---------------- NETWORK CONFIGURATION ---------------- 

byte mac[]                  = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //Arduino MAC address
const char SERVER_NAME[]    = "WEBSERVER ADDRESS"; // Address of the webserver
int SERVER_PORT             = WEBSERVER PORT; // webserver port
const char UDP_SERV_DEST[]  = "ADDRESS TO SEND UDP TO"; // Address to send UDP packets to
int UDP_PORT                = UDP PORT; // port to send UDP packets over
char UDP_MESSAGE[]          = "UDP MESSAGE"; // UDP packet message


// ---------------- INITIALIZATION ----------------

EthernetClient client;
EthernetUDP Udp;

void setup()
{
  Serial.begin(9600);
  Serial.println("setup");
  pinMode(powerLED,OUTPUT);
  pinMode(ledClient,OUTPUT);
  pinMode(ledConnect,OUTPUT);
  pinMode(ledError,OUTPUT);
  pinMode(TrafficRED,OUTPUT);
  pinMode(TrafficYELLOW,OUTPUT);
  pinMode(TrafficGREEN,OUTPUT);
  
  digitalWrite(powerLED,HIGH);  //Power LED ON
  
// ---------------- All Lights Off ----------------  
  
  digitalWrite(TrafficRED,HIGH);     // Red lamp off
  digitalWrite(TrafficYELLOW,HIGH);  // Yellow lamp off
  digitalWrite(TrafficGREEN,HIGH);   // Green lamp off

// ---------------- Test Lights ----------------

  digitalWrite(TrafficRED,LOW);     // Red lamp test: ON
  delay(500);
  digitalWrite(TrafficRED,HIGH);    // Red lamp test: OFF
  delay(500);  
  digitalWrite(TrafficYELLOW,LOW);  // Yellow lamp test: ON
  delay(500);
  digitalWrite(TrafficYELLOW,HIGH); // Yellow lamp test: OFF
  delay(500);
  digitalWrite(TrafficGREEN,LOW);   // Green lamp test: ON
  delay(500);
  digitalWrite(TrafficGREEN,HIGH);  // Green lamp test :OFF
  delay(500);
  digitalWrite(TrafficRED,LOW);     // Error lamp combo test: ON
  digitalWrite(TrafficYELLOW,LOW);  // Error lamp combo test: ON
  delay(500);
  digitalWrite(TrafficRED,HIGH);    // Error lamp combo test: OFF
  digitalWrite(TrafficYELLOW,HIGH); // Error lamp combo test: OFF
  delay(500);
  digitalWrite(TrafficRED,LOW);     // Full lamp test: ON
  digitalWrite(TrafficYELLOW,LOW);  // Full lamp test: ON
  digitalWrite(TrafficGREEN,LOW);   // Full lamp test: ON
  delay(3000);
  digitalWrite(TrafficRED,HIGH);    // Full lamp test OFF
  digitalWrite(TrafficYELLOW,HIGH); // Full lamp test OFF
  digitalWrite(TrafficGREEN,HIGH);  // Full lamp test OFF

  
// ---------------- Start the Ethernet connection ----------------

  restart:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
     for(int x = 0; x < 2; x++){
       digitalWrite(ledError,HIGH);
       digitalWrite(TrafficRED,LOW);    // red lamp on
       digitalWrite(TrafficYELLOW,LOW); // yellow lamp on
       delay(500);
       digitalWrite(ledError,LOW);
       digitalWrite(TrafficRED,HIGH);    // red lamp off
       digitalWrite(TrafficYELLOW,HIGH); // yellow lamp off
       delay(500);
     }
    delay(3000);  //delay before resetting Traffic Light
     goto restart;
  }
  // give the Ethernet shield time to initialize:
  delay(2000);
}

void loop()
{
  
  // ---------------- NETWORK/SYSTEM STABILITY: FIXES SYSTEM FREEZING ----------------
  
  Serial.println("Resetting Ethernet");
  client.stop();
  delay(1000);
  Ethernet.begin(mac);
  delay(2000);

  // ---------------- LIFE STATUS: Send life status (UDP Packets) to home ----------------
  
    // Send life status back to server
  for(int x = 0; x < 4; x++){ 
    digitalWrite(ledError, HIGH);
    Serial.println("Sending UDP Packet");
    Udp.begin(UDP_PORT); //open UDP Port
    Udp.beginPacket(UDP_SERV_DEST, UDP_PORT);
    Udp.write(UDP_MESSAGE);
    Udp.endPacket(); 
    Udp.stop();
    Serial.println("Packet Sent");
    digitalWrite(ledError, LOW); //blink light every time a packet is sent
    delay(1000);
   }   

  // ---------------- FIRE/EMS: CONNECT TO WEBSERVER ----------------
     
  Serial.println("connecting...");
  // if you get a connection, report back via serial:
  digitalWrite(ledConnect,HIGH);
  digitalWrite(ledClient,LOW);
  digitalWrite(ledError,LOW);
  if (client.connect(SERVER_NAME, SERVER_PORT)) {
    Serial.println("connected");
    digitalWrite(ledConnect,HIGH);
    // Make a HTTP request:
    //client.println("GET /my%20portable%20files/GetGmail.php");   //EasyPHP pc server pathway
    client.println("GET /GetGmail.php");  // Apache server pathway.
    client.println();
    int timer = millis();
    delay(2000);
  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");  //cannot connect to server
     digitalWrite(ledError, HIGH);
     for(int x = 0; x < 4; x++){
     digitalWrite(ledConnect,HIGH);
     delay(300);
     digitalWrite(ledConnect,LOW);
     delay(300);
     }
  }

  // if there's data ready to be read:
  if (client.available()) {  
     int i = 0;
     digitalWrite(ledError,LOW);
     digitalWrite(ledClient,HIGH);     
     //put the data in the array:
     do {
       Str[i] = client.read();
       i++;
       delay(1);
     } while (client.available());
     
     // Pop on the null terminator:
     Str[i] = '\0';
     //convert server's repsonse to a int so we can evaluate it
     num = atoi(Str); 
     
     Serial.print("Server's response: ");
     Serial.println(num);
     Serial.print("Previous response: ");
     Serial.println(prevNum);
     if (prevNum < 0)
     { //the first time around, set the previous count to the current count
      prevNum = num; 
      Serial.println("First email count stored.");
     }
     if (prevNum > num)
     { // handle if count goes down for some reason
      prevNum = num; 
     }
  }
  else
    {
     Serial.println("No response from server."); //cannot connect to server.
     digitalWrite(ledError,HIGH);
     digitalWrite(ledConnect,LOW);
     digitalWrite(ledClient,LOW);
    }
    Serial.println("Disconnecting."); //disconnecting from server to reconnect
    client.stop();
    
    // ---------------- FIRE\EMS: ALERT FOR FIRE\EMS CALL ----------------   
    
    if(num > prevNum) {
      Serial.println("FIRE ALERT!");  //alert for new email
      Serial.println("RED LIGHT ON");
      for(int x = 0; x < 2; x++)  // Red light blinks 2 times then stays solid.
      {
     digitalWrite(TrafficRED,LOW);
     delay(500);
     digitalWrite(TrafficRED,HIGH);
     delay(500);
     digitalWrite(TrafficRED,LOW);
      }
      delay(onTimeRed);  // 3 minutes delay
      digitalWrite(TrafficRED,HIGH);
      Serial.println("RED LIGHT OFF");
      Serial.println("YELLOW LIGHT ON");     
      for(int x = 0; x < 2; x++)  // Yellow light blinks 2 times then stays solid.
      {
     digitalWrite(TrafficYELLOW,LOW);
     delay(500);
     digitalWrite(TrafficYELLOW,HIGH);
     delay(500);
     digitalWrite(TrafficYELLOW,LOW);
      }
      delay(onTimeYellow);  // 2 minutes delay
      digitalWrite(TrafficYELLOW,HIGH);
      Serial.println("YELLOW LIGHT OFF");
      Serial.println("GREEN LIGHT ON");
      for(int x = 0; x < 2; x++)   // Green light blinks 2 times then stays solid.
      {
     digitalWrite(TrafficGREEN,LOW);
     delay(500);
     digitalWrite(TrafficGREEN,HIGH);
     delay(500);
     digitalWrite(TrafficGREEN,LOW);
      }

      
prevNum = num;
      onUntil = millis() + onTimeGreen;
    }
    else if(millis() > onUntil)  //if email value is lower/equal to previous, no alert.
    {
     Serial.println("GREEN LIGHT OFF");
     digitalWrite(TrafficGREEN,HIGH);
    }
  delay(pollingInterval);  //wait time until restart.
}


