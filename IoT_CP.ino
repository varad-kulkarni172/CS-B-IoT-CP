#include <ESP8266WiFi.h>

#include <WiFiClient.h>

#include <WiFiClientSecure.h>

#include <ESP8266WebServer.h>

#include <ESP8266mDNS.h>


MDNSResponder mdns;


boolean trigger_sms = false;

boolean power_module = false;



const char* ssid = "Varad's G82";  //Enter you Wifi SSID here

const char* password = "123456789"; //Enter your password here


 


ESP8266WebServer server(80);


String mainPage = ""; //The default page

String feedback = ""; //Gives staus of the switch

String currentPage = ""; //Combines main and feedback page


int GPIO_0 = 0; //Pin defanition

int GPIO_2 = 2; //Pin defanition


void setup(void){

  //HTML code for webpage//

  mainPage += "<h1 align=\"center\">IOT based Smart Home Security System</h1><h2 align=\"center\">-By Batch 3 Group 3</h2><h1 align=\"center\"><p>Alarm Status: <a href=\"switch2On\"><button>ARM</button></a>&nbsp;<a href=\"switch2Off\"><button>DISARM</button>  </a></p>";

  feedback = "<h3 align=\"center\">Click on ARM to enable the security system</h3>";

  //End of HTML code//

  currentPage = mainPage+feedback;

  

  // preparing GPIOs

  pinMode(GPIO_0, INPUT); //feeded by PIR sensor

  pinMode(GPIO_2, OUTPUT); //used to turn on PIR and Voice module

  digitalWrite(GPIO_2, LOW);

  

  delay(1000);

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.println("");


  // Wait for connection

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");

  }

  // config static IP



  Serial.println("");

  Serial.print("Connected to ");

  Serial.println(ssid);

  Serial.print("IP address: ");

  Serial.println(WiFi.localIP()); //Serial monitor will give the IP addrss of your ESP module

  

  if (mdns.begin("esp8266", WiFi.localIP())) {

    Serial.println("MDNS responder started");

  }

  

  server.on("/", [](){

    currentPage = mainPage+feedback;

    server.send(200, "text/html", currentPage);

    currentPage = "";

  });



  

  server.on("/switch2On", [](){

    feedback = "<h3 align=\"center\">Alarm is up and running </h3>"; //HTML code modification

    currentPage=mainPage+feedback;

    server.send(200, "text/html", currentPage);

    currentPage="";

    digitalWrite(GPIO_2, HIGH); //Turn on PIR and Voice module

    power_module=true;

    delay(1000);

  });

  server.on("/switch2Off", [](){

    feedback = "<h3 align=\"center\">Alarm is down</h3>"; //HTML code modification 

    currentPage=mainPage+feedback;

    server.send(200, "text/html", currentPage);

    currentPage="";

    digitalWrite(GPIO_2, LOW); //Turn off PIT and Voice Module

    power_module=false;

    delay(1000); 

  });



  server.begin();

  Serial.println("IOT security system is up and running");

}


//Function to send an E-mail//

void send_Email()

{

const char* host = "maker.ifttt.com"; //The host of API URL will be the same for all

const int httpsPort = 443;


   WiFiClientSecure client;

  Serial.print("connecting to ");

  Serial.println(host);

  if (!client.connect(host, httpsPort)) {

    Serial.println("connection failed");

    return;

  }


 


  String url = "/trigger/123/with/key/dhRJYGjud1CQgATguauLKV-XpccqZVyU6ffwewbCx8E"; //Must change it to your API URL

  Serial.print("requesting URL: ");

  Serial.println(url);


  client.print(String("GET ") + url + " HTTP/1.1\r\n" +

               "Host: " + host + "\r\n" +

               "User-Agent: BuildFailureDetectorESP8266\r\n" +

               "Connection: close\r\n\r\n");


  Serial.println("request sent");

  while (client.connected()) {

    String line = client.readStringUntil('\n');

    if (line == "\r") {

      Serial.println("headers received");

      break;

    }

  }

  String line = client.readStringUntil('\n');


  Serial.println("reply was:");

  Serial.println("==========");

  Serial.println(line);

  Serial.println("==========");

  Serial.println("closing connection");

}

//Function to send an E-mail


 

void loop(void){

  server.handleClient();


  if (digitalRead(GPIO_0==HIGH) && power_module==true)

  trigger_sms=true;

  else

  trigger_sms=false;

  

  if (trigger_sms==true)

  {

    send_Email(); //If the PIR module is powered and if a person is detected send a E-mail

    trigger_sms=false;

    delay(2000);

  }

  

}

