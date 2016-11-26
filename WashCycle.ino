/**************************************************************
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************
 * This example shows how to use ordinary Arduino Serial
 * to connect your project to Blynk.
 * Feel free to apply it to any other example. It's simple!
 *
 * 1. Optional, but recommended.
 *    Connect additional USB-serial adapter to see the prints.
 *
 * 2. Edit auth token and upload this sketch.
 *
 * 3. Run the script (script located in "scripts" folder of library root,
 *    e.g. 'blynk-library/scripts') for redirecting traffic to server:
 *
 *      for Windows:
 *                     1. Open cmd.exe
 *                     2. write : (your way to blynk-ser.bat folder) example: "cd C:\blynk-library-0.3.1\blynk-library-0.3.1\scripts"
 *                     3. write : "blynk-ser.bat -c COM4" (where COM4 is port with your Arduino)
 *                     4. And press "Enter" , press "Enter" and press "Enter"
 *
 *      for Linux and OSX:
 *
 *            cd /Users/Andrew/Documents/Arduino/libraries/Blynk/scripts        
 *            ./blynk-ser.sh (may need to run with sudo)
 *
 *    You can specify port, baud rate, and server endpoint like this:
 *      ./blynk-ser.sh -c <serial port> -b <baud rate> -s <server address> -p <server port>
 *
 *    For instance :
 *      ./blynk-ser.sh -c /dev/ttyACM0 -b 9600 -s cloud.blynk.cc -p 8442
 *
 *    Run blynk-ser.sh -h for more information
 *
 *    Be sure to select the right serial port (there may be multiple).
 *
 *    Attention!
 *        Arduino IDE may complain with "programmer is not responding".
 *        You need to terminate script before uploading new sketch.
 *
 * 4. Start blynking! :)
 *
 **************************************************************/

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "297c73adc7964678892c25dadc5833ea";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "CarrollHome";
char pass[] = "Amazinglove1222";

SimpleTimer timer;
WidgetLED led0(0);
WidgetLED led1(1);
const int sensePin = 13;
const int vibeToThresh = 12000;
const int notifyToThresh = 12000;
int vibeState = 0;
int washDet = 0;
int vibeCnt = 0;
int toCnt = 0;
int washComplete = 0;
int ackComplete = 0;
int notifyTo = notifyToThresh;
int ackButton = 0;

void setup()
{
  Blynk.begin(auth, ssid, pass);
  pinMode(sensePin, INPUT);
  // Setup a function to be called every second
  timer.setInterval(50L, getSample);
  
}

void getSample()
{

  vibeState = digitalRead(sensePin);
  if (washComplete == 0) {
    if (vibeState == 1) {
      toCnt = 0;
      if (vibeCnt == 1) {
        washDet = 1;
        vibeCnt = vibeCnt + 1;
        led0.on();
      } else {
        vibeCnt = vibeCnt + 1;
        delay(5000);
      }
    } else {
      if (toCnt == vibeToThresh) {     // 10 minute timeout
        if (washDet == 1) {
          washComplete = 1;
          led1.on();
        }
        vibeCnt = 0;
        washDet = 0;
        toCnt = 0;
        led0.off();
      } else {
        toCnt = toCnt + 1;
      }
    }
    Blynk.virtualWrite(V3,vibeCnt);
  } else {
    if (ackButton == 1) {
      ackComplete = 1;
      washComplete = 0;
      led1.off();
      notifyTo = notifyToThresh;
    }
    if (notifyTo == notifyToThresh) {
      Blynk.notify("Wash Cycle Complete!");
      notifyTo = 0;
    } else {
      notifyTo = notifyTo + 1;
    }
  }
}

BLYNK_WRITE(V4)
{
  ackButton = param.asInt();
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates SimpleTimer
}

