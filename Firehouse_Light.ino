/*
 New email alarm/notification for google gmail account.
 Receives emails from Codemessaging.net
 
 Created December 8, 2011
 
 Based on the WebClient example by David A. Mellis,
 modified by Tom Igoe, based on work by Adrian McEwen.
 -----------------------------------------------------
 
 Modified by : Brian Leschke
 
 Date: March 17, 2016
 Version 5.4
 
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
 
 Information about HIGH and LOW values.
    Pins not connected to the relay use HIGH for ON and LOW for OFF.
    Pins connected to the relay use LOW for ON and HIGH for OFF.
 
 */
 
#include <SPI.h>
#include <Ethernet.h>
//#include <VirtualWire.h>
#include <stdlib.h>

int powerLED = 2;
int ledClient = 3;
int ledConnect = 4;
int ledError = 5;
int TrafficRED = 6;
int TrafficYELLOW = 7;
int TrafficGREEN = 8;

char Str[11];
int prevNum = 0;
int num = 0;
long onUntil = 0;
long pollingInterval = 1000; // in milliseconds
long onTimeGreen = 299000; // time, in milliseconds, for Green light to be on after new alert.
long onTimeRed = 179000;  //  time, in milliseconds, for Red light to be on after new alert.
long onTimeYellow = 119000;  // time, in milliseconds, for Yellow light to be on after new alert.


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Older Ethernet cards require DE:AD:BE:EF:FE:ED as MAC.
char serverName[] = "SERVER ADDRESS";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):

EthernetClient client;

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

  digitalWrite(TrafficRED,LOW);    // Red lamp test: ON
  delay(500);
  digitalWrite(TrafficRED,HIGH);   // Red lamp test: OFF
  delay(500);  
  digitalWrite(TrafficYELLOW,LOW); // Yellow lamp test: ON
  delay(500);
  digitalWrite(TrafficYELLOW,HIGH); // Yellow lamp test: OFF
  delay(500);
  digitalWrite(TrafficGREEN,LOW);  // Green lamp test: ON
  delay(500);
  digitalWrite(TrafficGREEN,HIGH); // Green lamp test :OFF
  delay(500);
  digitalWrite(TrafficRED,LOW);    // Error lamp combo test: ON
  digitalWrite(TrafficYELLOW,LOW); // Error lamp combo test: ON
  delay(500);
  digitalWrite(TrafficRED,HIGH);    // Error lamp combo test: OFF
  digitalWrite(TrafficYELLOW,HIGH); // Error lamp combo test: OFF
  delay(500);
  digitalWrite(TrafficRED,LOW);    // Full lamp test: ON
  digitalWrite(TrafficYELLOW,LOW); // Full lamp test: ON
  digitalWrite(TrafficGREEN,LOW);  // Full lamp test: ON
  delay(3000);
  digitalWrite(TrafficRED,HIGH);     // Full lamp test OFF
  digitalWrite(TrafficYELLOW,HIGH);  // Full lamp test OFF
  digitalWrite(TrafficGREEN,HIGH);   // Full lamp test OFF

  restart:
  
  // start the Ethernet connection:
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
    // no point in carrying on, so do nothing forevermore:
    //while(true);
    delay(3000);  //delay before resetting Traffic Light
     goto restart;
  }
  // give the Ethernet shield time to initialize:
  delay(2000);


}
void loop()
{
    Serial.println("Resetting Ethernet"); // Must be here to prevent system from periodically freezing on ethernet.
    client.stop();
    delay(1000);
    Ethernet.begin(mac);
    delay(2000);
    
    Serial.println("connecting...");
  // if you get a connection, report back via serial:
  digitalWrite(ledConnect,HIGH);
  digitalWrite(ledClient,LOW);
  digitalWrite(ledError,LOW);
  if (client.connect(serverName, PORT HERE)) {
    Serial.println("connected");
    digitalWrite(ledConnect,HIGH);
    // Make a HTTP request:
    //client.println("GET /my%20portable%20files/LonelyGmail.php");   //EasyPHP pc server pathway
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

