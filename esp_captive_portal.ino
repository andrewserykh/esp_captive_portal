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

long ANGLE1 = 0;
long ANGLE2 = 45;
long ANGLE3 = 90;

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
  String contentAngSet = ""
  "<div>"+String(ANGLE1)+"</div>"
  "<div style='display: grid;grid-template-columns:repeat(4,1fr);grid-template-rows:repeat(1,2em);'>"
    "<a href='/motor/set10m' class='s'>-10</a>"
    "<a href='/motor/set1m' class='s'>-1</a>"
    "<a href='/motor/set1p' class='s'>+1</a>"
    "<a href='/motor/set10p' class='s'>+10</a>"
  "</div>";
  webServer.send(200, "text/html", contentHead + contentAngSet + contentFooter);

}

void setup() {
  Serial.begin(115200);
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
  webServer.on("/motor/ang1", pageSetAngle);
  

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
