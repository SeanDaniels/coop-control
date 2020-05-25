#include <Arduino.h>
#include <FastLED.h>
#include <ESP8266WIFI.h>
#include "lights.h"

String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

String html_1 = R"=====(
<!DOCTYPE html>
<html>
 <head>
 <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
 <meta charset='utf-8'>
 <style>
 html {font-family: monospace;}
body {width: 50%; max-width: 800px; min-width: 480px; margin: 0 auto;  background-position: center;  background-color: #282c34;}
  #main {display: table; margin: auto;  padding: 0 10px 0 10px; } 
  h2 {color: #da8548; text-align:center; } 
  .button { 
  background-color: peru;
  border: none;
  border-radius: 12px;
  color: whitesmoke;
  padding: 15px 32px;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  transition-duration: 0.4s;
  box-shadow: 0 8px 16px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);
  }

 </style>
 
 <title>LED Control</title>
</head>
<body>
 <div id='main'>
  <h2>LED Control</h2>
)=====";

String html_2 = "  <form id='F1' action='LIGHTSON'><input class='button' type='submit' value='Turn Lights On' ></form><br>\r\n";
String html_3 = R"=====(
  <h2>Gate Control</h2>
  )=====";
String html_4 = "  <form id='F1' action='OPENGATE'><input class='button' type='submit' value='Open Gate' ></form><br>\r\n";
String html_5 = R"=====(
    </div>
 </body>
</html>
)=====";


#define NUM_LEDS 14
#define DATA_PIN 13
#define relayIn1 10
#define relayIn2 15
#define DELAY_TIME_ON 25
#define DELAY_TIME_OFF 15
//#define ALL_ON
//#define CRAWL
#define WIFI_LIGHT_CONTROL
#define TRY_BUTTON
#define CONSTANT_STRINGS

//LED strip strucTure
CRGB leds[NUM_LEDS];

//wifi variables and structs
const char* ssid = "ChatHeaux";
const char* pw = "shabbaranks";
WiFiServer server(80);

String request = "";

//
unsigned firstRun = 1;
unsigned buttonPressed = 0;

//init wifi
void init_wifi(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid,pw);
  while(WiFi.status()!= WL_CONNECTED){
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
}

//init server
void init_server(){
  server.begin();
  Serial.println("Web server started");
  Serial.print("Connect to server @ http://");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

void create_page(WiFiClient client)
{
#ifdef CONSTANT_STRINGS
    client.print(header);
    client.print(html_1);
    client.print(html_2);
    client.print(html_3);
    client.print(html_4);
    client.print(html_5);
#else
    client.println("HTTP/1.1 200 OK"); // HTML Header
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>"); // Start of HTML
    client.println("<style>");
    client.println("body {background-color: #ACAEAD;}"); // Set Background Color
    client.println("</style>");
    if (firstRun == 0)
    {
        client.print("Please Click a Button ");
    }
    else
    {
        client.print("The lights are ");
    }

    if (buttonPressed == LOW)
    {
        client.print("Off");
    }
    if (buttonPressed == HIGH)
    {
        client.print("On");
    }
    client.println("<br><br>");
    client.println("<a href=\"/ON=1\"\"><button>On </button></a>");
    client.println("<a href=\"/OFF=1\"\"><button>Off </button></a><br />");
    client.println("</html>");
#endif
    delay(10);
}

//main setup
void setup(){
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is typical
  init_wifi();
  init_server();
  pinMode(relayIn1, OUTPUT);
  pinMode(relayIn2, OUTPUT);
}

//individual light of color 'thisColor' moves down LED strip 

//main loop
void loop()
{
#ifdef WIFI_LIGHT_CONTROL
//connect to server
WiFiClient client = server.available();
if(!client){
  return;
}
//get info from server
String request = client.readStringUntil('\r');
//if not first run
Serial.print("Request: ");
Serial.println(request);
if(!firstRun){
    if (request.indexOf("LIGHTSON") > 0)
    {
        all_on(CRGB::White);
        buttonPressed = HIGH;
        html_2 = "  <form id='F1' action='LIGHTSOFF'><input class='button' type='submit' value='Turn Lights Off' ></form><br>\r\n";
    }
    else if (request.indexOf("LIGHTSOFF") > 0)
    {
        all_off();
        buttonPressed = LOW;
        html_2 = "  <form id='F1' action='LIGHTSON'><input class='button' type='submit' value='Turn Lights On' ></form><br>\r\n";
    }
    else if (request.indexOf("OPENGATE") > 0){
      digitalWrite(relayIn1, HIGH);
      delay(5000);
      digitalWrite(relayIn1, LOW);
      html_4 = "  <form id='F1' action='CLOSEGATE'><input class='button' type='submit' value='Close Gate' ></form><br>\r\n";
    }
    else if (request.indexOf("CLOSEGATE") > 0){
      digitalWrite(relayIn2, HIGH);
      delay(5000);
      digitalWrite(relayIn2, LOW);
      html_4 = "  <form id='F1' action='OPENGATE'><input class='button' type='submit' value='Open Gate' ></form><br>\r\n";
    }
}
create_page(client);
firstRun = 0;
#endif

//testing all on functionality
#ifdef ALL_ON
  all_on(CRGB::White);
  delay(2500);
  all_off();
  delay(500);
  all_on(CRGB::Red);
  delay(2500);
  all_off();
  delay(500);
  all_on(CRGB::Green);
  delay(2500);
  all_off();
  delay(500);
  all_on(CRGB::Blue);
  delay(2500);
  all_off();
  delay(500);
#endif

//testing crawl functionality
#ifdef CRAWL
  light_forward(CRGB::Yellow);
  light_backward(CRGB::Yellow);
  light_forward(CRGB::Red);
  light_backward(CRGB::Red);
  light_forward(CRGB::Green);
  light_backward(CRGB::Green);
  light_forward(CRGB::Blue);
  light_backward(CRGB::Blue);
#endif
}

void all_on(long thisColor){
    for (unsigned i = 0; i<NUM_LEDS; ++i)
    {
      leds[i] = thisColor;
      FastLED.show();
    }
}

//turn all LED strip lights off
void all_off(){
    for (unsigned i = 0; i<NUM_LEDS; ++i)
    {
      leds[i] = CRGB::Black;
      FastLED.show();
    }

}