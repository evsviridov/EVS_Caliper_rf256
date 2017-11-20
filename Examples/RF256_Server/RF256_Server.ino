#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "MERCURY";
const char* password = "87654321";

ESP8266WebServer server(80);

const int led = 14;
String serStr;

void handleRoot() {
  char tempStr[401];
  digitalWrite(led, 1);
  snprintf ( tempStr, 400,

             "<html>\
  <head>\
    <meta http-equiv='refresh' content='1'/>\
    <title>RF256 Caliper</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>%s</h1>\
    <p>ESP8266 Uptime=%d (sec)</p>\
   </body>\
</html>",

             serStr.c_str(), (millis() / 1000));

  server.send(200, "text/html", tempStr);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  //  Serial.print("SSID:");
  //  Serial.println(ssid);
  //  Serial.print("IP:");
  //  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    //    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.flush();
}

void loop(void) {
  if (Serial.available())
  {
    serStr = Serial.readStringUntil('\n');
    if (!strncmp(serStr.c_str(), "IP?", 3))
    {
      Serial.print("IP:");
      Serial.println(WiFi.localIP());
    }
    else if (!strncmp(serStr.c_str(), "SSID?", 5))
    {
      Serial.print("SSID:");
      Serial.println(ssid);
    }
  }
  server.handleClient();

}

