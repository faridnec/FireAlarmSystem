#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include "ThingSpeak.h" 
#include <BlynkSimpleEsp8266.h>

char auth[] = "HeLJ3Hlko5d76Xk1TzzYsstZ_PYsmU-i";
char ssid[] = "WiFi/Connectivity Name";
char pass[] = "WiFi/Connectivity Password";

/* ThingSpeak */ 
unsigned long channelID =1996784; //Channel ID from thingspeak
unsigned int field_no=1; 
const char* writeAPIKeyy = "Thingspeak write API Key for graph";   // Thingspeak write API Key for graph
//const char* readAPIKey = "Okuma anahtarını giriniz";   // Thingspeak read API Key 

void send_event(const char *event);
const char *host = "maker.ifttt.com";
const char *privateKey = "ifttt api key";//ifttt api key

//twitter thingspeak
const String writeAPIKey = "Thingspeak write API Key";   // Thingspeak write API Key
const char* thingSpeakHost = "api.thingspeak.com";     //ThingSpeak site host
String thingtweetAPIKey = "ThingTweet api key";    //ThingTweet api key
int randNumber;
WiFiClient client;
float temperature;   // Analog returns temperature
float olculendeger;     // measured analog value
String tsData;

BlynkTimer timer;
int pinValue = 0;

#define LED1 D1
#define LED2 D2
#define Buzzer D3
#define Sensor D0

void setup() {
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Sensor, INPUT);
  Blynk.begin(auth, ssid, pass);
   ThingSpeak.begin(client);         // ThingSpeak client class started
  timer.setInterval(1000L, notifiaction);
}

BLYNK_WRITE(V0) {
  pinValue = param.asInt();
}

void sendDataTS(int deger){
  ThingSpeak.writeField (channelID, field_no, deger, writeAPIKeyy);       // send temperature
  Serial.println("\n"); 
}

void sendtweet(){
  //thingspeak tw
  randNumber=random(5,1000);
  Serial.print("//");
  Serial.println(randNumber);
  if (client.connect (thingSpeakHost, 80)) {        //with url
  String postStr = "field1=";                  //data will be displayed on Field 1 graph
  postStr+= String (temperature);
  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
 client.print("Content-Length: ");
  client.print(postStr.length());
  client.print("\n\n");
  client.print(postStr);
  client.print("\n\n");
  Serial.println(temperature);
  Serial.println ("temperature has been sent to ThingSpeak");
}
  if (client.connect("api.thingspeak.com",80)) {
  Serial.println("TWİTTER CONNECTED");  //to upload to twitter
  tsData = "api_key="+thingtweetAPIKey+"&status="+"  #WARNING" + "  A fire was detected! Location : Alarm Location "+"   // "+randNumber ;
  client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(tsData.length());
  client.print("\n\n");
  client.print(tsData);
  Serial.println("TWEETED");
}
client.stop();
}

void send_event(const char *event)
{
  Serial.print("Connecting to "); 
  Serial.println(host);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += privateKey;
  Serial.print("Requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  while(client.connected())
  {
    if(client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    } else {
      delay(50);
    };
  }
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}

int deger;
void notifiaction() {//the circuit
  int sensor = digitalRead(Sensor);
  if (pinValue == 1) {
    Serial.println("System is ON");
    if (sensor == 1) {
      digitalWrite(LED2, HIGH);
      digitalWrite(LED1, LOW);
      digitalWrite(Buzzer, LOW);
      deger=0; 
      sendDataTS(deger);
    } else if (sensor == 0) {
      Blynk.notify("WARNING! A fire was detected");
      digitalWrite(LED2, LOW);
      digitalWrite(LED1, HIGH);
      digitalWrite(Buzzer, HIGH);
      sendtweet(); 
      send_event("FIRE DETECTION");  
      send_event("Alert");
      deger=100; 
      sendDataTS(deger);
    }
  } else if (pinValue == 0) {
    Serial.println("System is OFF");
  }
}

void loop() {
 Blynk.run();
 timer.run();
}