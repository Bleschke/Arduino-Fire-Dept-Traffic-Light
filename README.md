# Arduino-Fire-Dept-Traffic-Light

Brian Leschke 2016
 
 - Update 12/8/2011 - Initial release
 - Update 2012-2015 - Unspecified Updates
 - Update 6/19/2016 - "cleaned up", organized, and simplified code.
 - Update 6/19/2016 - ADDED UDP SERVER: This unit will send UDP packets to the receiver box located at my home.
             This is to make sure that the Traffic Light is working correctly and the network is active. 

## **Overview**

This program will light up a traffic light using corresponding colors of RED, YELLOW, and GREEN for "leaving the station"  actions as well as send UDP packets to the "Arduino-Fire-Dept-Traffic-Light-Homeversion" program. 

Project Breakdown:
* Traffic Light
    * Displays Alert Countdown status
        * RED - "Get Ready": 0-3 minutes (Flashes 3 times, then solid)
        * YELLOW - "Get Set": 3-5 Minutes (Flashes 3 times, then solid)
        * GREEN - "Go": 5-10 Minutes (Flashes 3 times, then solid)
* Reset Button
    * Reset Arduino when pressed

### **Prerequisities**

You will need:

1. Arduino Uno
2. Sainsmart 4 module high voltage relay
3. 2 RED, 1 YELLOW, 1 GREEN LED
3. 1 monentary push button
4. Arduino programming software
5. Webserver hosting script.

### **Usage**

In order to use successfully implement this project and its code, you will need to install the Arduino Programming Software.
    
## **Uploading**

The code can be uploaded to the ESP8266 by a serial (USB) connection. 



