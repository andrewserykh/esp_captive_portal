/*
 * V1.1
 * 
 * EEPROM bytes:
 * -------------
 * 0 - ANG0
 * 1 - ANG1
 * 2 - ANG2
 * 
 */

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "htmlContents.h"

#define PIN_DI1   1
#define PIN_DI2   2
#define PIN_DI3   3
#define PIN_TX    10
#define PIN_RX    11

const byte DNS_PORT = 53;
IPAddress apIP(172, 0, 0, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

bool DI1 = false;
bool DI2 = false;
bool DI3 = false;

//настройки
int DI_MODE = 0;
bool DI_INVERT = false;

int ANGLE0 = 0;
int ANGLE1 = 1000;
int ANGLE2 = 2000;

//временные переменные
int SetAngleIndex = 0;

void ajaxInputs(){
  String di1on = "";
  if (DI1) di1on = "on";
  String di2on = "";
  if (DI2) di2on = "on";
  String di3on = "";
  if (DI3) di3on = "on";
  
  String contents= ""
  "<div class='t0'>ВХОДЫ</div>"
  "<div style='display: grid;grid-template-columns:repeat(3,1fr);grid-template-rows:repeat(1,2em);'>"
  "<a href='#' class='s "+di1on+"'>DI 1</a>"
  "<a href='#' class='s "+di2on+"'>DI 2</a>"
  "<a href='#' class='s "+di3on+"'>DI 3</a>"
  "</div>";
  webServer.send(200,"text/html",contents);
}

void pageSetup(){
  String sdi1="";
  String sdi2="";
  String idi1="";
  String idi2="";
  if (DI_MODE==0) sdi1="on";
  if (DI_MODE==1) sdi2="on";
  if (!DI_INVERT) idi1="on";
  if (DI_INVERT) idi2="on";
  
  String contentSetup = ""
    "<div class='t0'>КОНФИГУРАТОР ВХОДОВ</div>"
    "<div style='display: grid;grid-template-columns:repeat(2,1fr);grid-template-rows:repeat(1,2em);'>"
      "<a href='/setup/di1' class='s "+sdi1+"'>DI1, DI2</a>"
      "<a href='/setup/di2' class='s "+sdi2+"'>DI1, DI2, DI3</a>"
    "</div>"
    "<div class='t0'>ИНВЕРСИЯ ВХОДОВ</div>"
    "<div style='display: grid;grid-template-columns:repeat(2,1fr);grid-template-rows:repeat(1,2em);'>"
      "<a href='/setup/noninvert' class='s "+idi1+"'>НЕТ</a>"
      "<a href='/setup/invert' class='s "+idi2+"'>ИНВЕРСИЯ</a>"
    "</div>";
  webServer.send(200, "text/html", contentHead + contentSetup + contentFooter);
}

void pageSetupDi1(){
  DI_MODE = 0;
  pageSetup();
}
void pageSetupDi2(){
  DI_MODE = 1;
  pageSetup();
}
void pageSetupNoninvert(){
  DI_INVERT = false;
  pageSetup();
}
void pageSetupInvert(){
  DI_INVERT = true;
  pageSetup();
}

void pageSetAngle(){
  String strAngle = "";
  if (SetAngleIndex == 0) strAngle = String(ANGLE0);
  if (SetAngleIndex == 1) strAngle = String(ANGLE1);
  if (SetAngleIndex == 2) strAngle = String(ANGLE2);
  String contentAngSet = ""
  "<div>"+strAngle+"</div>"
  "<div style='display: grid;grid-template-columns:repeat(4,1fr);grid-template-rows:repeat(1,2em);'>"
    "<a href='/motor/set100m' class='s'>-100</a>"
    "<a href='/motor/set10m' class='s'>-10</a>"
    "<a href='/motor/set10p' class='s'>+10</a>"
    "<a href='/motor/set100p' class='s'>+100</a>"
  "</div>";
  webServer.send(200, "text/html", contentHead + contentAngSet + contentFooter);
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(3);
  ANGLE0 = EEPROM.read(0);
  ANGLE1 = EEPROM.read(1);
  ANGLE2 = EEPROM.read(2);
  
  delay(10);
  Serial.println("Started");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("INFO");

  dnsServer.start(DNS_PORT, "*", apIP);
  webServer.on("/setup", pageSetup);
  webServer.on("/setup/di1", pageSetupDi1);
  webServer.on("/setup/di2", pageSetupDi2);
  webServer.on("/setup/noninvert", pageSetupNoninvert);
  webServer.on("/setup/invert", pageSetupInvert);

  webServer.on("/motor",[]() {
    webServer.send(200, "text/html", contentHead + contentMotor + contentFooter);
  });
  
  webServer.on("/motor/ang0", [](){
    SetAngleIndex = 0;
    pageSetAngle();
  });
    webServer.on("/motor/ang1", [](){
    SetAngleIndex = 1;
    pageSetAngle();
  });
  webServer.on("/motor/ang2", [](){
    SetAngleIndex = 2;
    pageSetAngle();
  });

  webServer.on("/motor/set100m", [](){
    if (SetAngleIndex==0){
      ANGLE0 = ANGLE0 - 100;      
      if (ANGLE0<0) ANGLE0 = 0;
      EEPROM.write(0, ANGLE0);
    } else if (SetAngleIndex==1) {
      ANGLE1 = ANGLE1 - 100;
      if (ANGLE1<0) ANGLE1 = 0;
      EEPROM.write(1, ANGLE1);      
    } else if (SetAngleIndex==2) {
      ANGLE2 = ANGLE2 - 100;
      if (ANGLE2<0) ANGLE2 = 0;
      EEPROM.write(2, ANGLE2);
    }
    pageSetAngle();
  });
  webServer.on("/motor/set10m", [](){
    if (SetAngleIndex==0){
      ANGLE0 = ANGLE0 - 10;      
      if (ANGLE0<0) ANGLE0 = 0;
      EEPROM.write(0, ANGLE0);
    } else if (SetAngleIndex==1) {
      ANGLE1 = ANGLE1 - 10;
      if (ANGLE1<0) ANGLE1 = 0;
      EEPROM.write(1, ANGLE1);
    } else if (SetAngleIndex==2) {
      ANGLE2 = ANGLE2 - 10;
      if (ANGLE2<0) ANGLE2 = 0;
      EEPROM.write(2, ANGLE2);
    }
    pageSetAngle();
  });
  webServer.on("/motor/set100p", [](){
    if (SetAngleIndex==0){
      ANGLE0 = ANGLE0 + 100;
      EEPROM.write(0, ANGLE0);      
    } else if (SetAngleIndex==1){
      ANGLE1 = ANGLE1 + 100;
      EEPROM.write(1, ANGLE1);
    } else if (SetAngleIndex==2){
      ANGLE2 = ANGLE2 + 100;
      EEPROM.write(2, ANGLE2);
    }
    pageSetAngle();
  });
  webServer.on("/motor/set10p", [](){
    if (SetAngleIndex==0){
      ANGLE0 = ANGLE0 + 10;
      EEPROM.write(0, ANGLE0);      
      if (ANGLE0<0) ANGLE0 = 0;
    } else if (SetAngleIndex==1){
      ANGLE1 = ANGLE1 + 10;
      EEPROM.write(1, ANGLE1);
    } else if (SetAngleIndex==2){
      ANGLE2 = ANGLE2 + 10;
      EEPROM.write(2, ANGLE2);
    }
    pageSetAngle();
  });
  
  webServer.on("/ajaxinputs", ajaxInputs);

  webServer.onNotFound([]() {
    webServer.send(200, "text/html", contentHead + contentAjaxStart + contentAjaxData + contentAjaxEnd + contentMain + contentFooter);
  });

  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
