//#include <rp2040_pio.h>

/* 
 * Single Strand Christmas Lights
 * Author: David Orser
 * Date: Dec. 10, 2023
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */
 //Migrated to Rpico 12/19/2024
 //Added Wifi/OTA/UDP features 11/16/2025

//  10@31x3 Lights : 5.07 - 5.01  
//  80@31x3 Lights : 5.03 - 4.90  
// 160@31x3 Lights : 5.00 - 4.68
// 240@31x3 Lights : 4.96 - 4.35
// 290@31x3 Lights : 4.94 - 4.08
// 300@31x3 Lights : 4.93 - 4.02
// 300@31x3 Lights : 4.90 - 3.98

#include <WiFi.h>
#include <SimpleMDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Adafruit_NeoPixel.h>
#include "math.h"
//#include "Sunrise.h"

#define LED_PIN    22
#define LED_COUNT 297
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_BGR + NEO_KHZ800);

//Sunrise winterSunrise(44.9778, 93.2650, -6);  // Minneapolis, MN, USA - Latitude/Longitude and UTC offset
//Sunrise summerSunrise(44.9778, 93.2650, -5);  // Minneapolis, MN, USA - Latitude/Longitude and UTC offset

const bool djoDebug=false;
void sparkle(int i);
int timeHour = 0;
double aboveZero(double val);

// Wi-Fi and OTA credentials live in secrets.h (git-ignored).
// Copy secrets.h.example to secrets.h and fill in your values.
#if __has_include("secrets.h")
  #include "secrets.h"
#else
  #error "Missing secrets.h - copy secrets.h.example to secrets.h and edit it."
#endif

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

WiFiUDP Udp;
const unsigned int LOCAL_PORT = 5005;
char rxBuf[1024] = "redgrn";

void setupWifi();
void udpHandler();

void setup() { 
  pinMode(D2,INPUT_PULLDOWN);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin();

  delay(1000);
  setupWifi();
  if (Udp.begin(LOCAL_PORT)) {
    Serial.print("UDP listening on port "); Serial.println(LOCAL_PORT);
  } else {
    Serial.println("UDP begin failed!");
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  ArduinoOTA.handle();  // must be called each loop and at least once every ~20ms 
  udpHandler();

  double time = millis()/1000.0;
  double freq = 2.0*M_PI*0.8*0.5;
  double freq2 = 2.0*M_PI*0.66*0.5;
  double sinAmp = 80.0;
  
  timeHour = 18;  // Can use a internet time library here

  if(timeHour >= 17 || digitalRead(D2)) {  // Turn off the lights at midnight, on at 5pm
    strip.setBrightness(255);
  } else {
    strip.setBrightness(0);
    // strip.setBrightness(255);  //disable here for day time testing.
  }

  // if(djoDebug) {Serial.printf("Clock Time: %d",Time.hour());}
  if(djoDebug) {Serial.printf("SinTime: %f",time);}
  //if(djoDebug) {Serial.printf("SinArg: %f",sinArg);}
  //if(djoDebug) {Serial.printf("Value: %f",curPix);}

  for(int i = 0;i<(LED_COUNT-1);i=i+2) {
    double curPixR1 = aboveZero(sinAmp*sin( freq *time + 2.0*M_PI*i/60.0 + 0*M_PI/3)-sinAmp/4); // NEO_GRB Green, NEO_RGB Blue
    double curPixG1 = 0; //aboveZero(sinAmp*sin(-freq *time + 2.0*M_PI*i/60.0 + 2*M_PI/3)-sinAmp/4); // NEO_GRB Blue, NEO_RGB Green
    double curPixB1 = 0; //aboveZero(sinAmp*sin( freq2*time + 2.0*M_PI*i/60.0 + 4*M_PI/3)-sinAmp/4); // NEO_GRB Red, NEO_RGB Red
    double curPixR2 = 0; //aboveZero(sinAmp*sin( freq *time + 2.0*M_PI*i/60.0 + 0*M_PI/3)-sinAmp/4);
    double curPixG2 = aboveZero(sinAmp*sin(-freq2 *time/2.0 + 2.0*M_PI*i/60.0 + 2*M_PI/3)-sinAmp/3);
    double curPixB2 = 0; //aboveZero(sinAmp*sin( freq2*time/2.0 + 2.0*M_PI*i/60.0 + 4*M_PI/3+M_PI)-sinAmp/3);
    strip.setPixelColor(i,curPixR1,curPixG1,curPixB1);
    strip.setPixelColor(i+1,curPixR2,curPixG2,curPixB2);

    // if (curPixR>0) {
    //   strip.setPixelColor(i,curPixR,0,0);
    // } else {
    //   strip.setPixelColor(i,-curPixR,0,0);
    // }
    // if (curPixG>0) {
    //   strip.setPixelColor(i,0,curPixG,0);
    // } else {
    //   strip.setPixelColor(i,0,-curPixG,0);
    // }
    sparkle(i);

  }
  strip.show();

  if(djoDebug) {Serial.printf("\n");}

}

void sparkle(int i) {
  static long unsigned int timeToFlash = 0; 
  int locToFlash = rand() % 300;

  if( (millis() > timeToFlash) && (locToFlash == i)) {
    timeToFlash = timeToFlash + rand() % 450 + 500;
    strip.setPixelColor(i,255,255,255);
  }

}

double aboveZero(double val) {
  if(val > 0) {
    return val;
  } else {
    return 0;
  }
}

void udpHandler() {
  int packetSize = Udp.parsePacket();
  if (packetSize <= 0) return;

  int len = Udp.read(rxBuf, sizeof(rxBuf) - 1);
  if (len < 0) return;
  rxBuf[len] = '\0';

  IPAddress rip = Udp.remoteIP();
  uint16_t rport = Udp.remotePort();

  Serial.print("RX "); Serial.print(packetSize);
  Serial.print("B from "); Serial.print(rip);
  Serial.print(":"); Serial.println(rport);
  Serial.print("Data: "); Serial.println(rxBuf);

  // Confirmation reply (echo with a small prefix)
  Udp.beginPacket(rip, rport);
  Udp.print("OK: ");
  Udp.write((const uint8_t*)rxBuf, len);
  Udp.endPacket();
}

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    rp2040.restart();
  }

  // Port defaults to 2040
  // ArduinoOTA.setPort(2040);

  // Hostname defaults to pico-[ChipID]
  ArduinoOTA.setHostname(OTA_HOSTNAME);

  // No authentication by default
  ArduinoOTA.setPassword(OTA_PASSWORD);

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
