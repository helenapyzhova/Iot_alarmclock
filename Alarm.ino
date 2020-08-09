
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
const char* ssid = "asus121";
const char* password = "qwerty123456";
WiFiUDP ntpUDP; //ntp
// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);
#define MAX_ALARMS 10
String alarms[MAX_ALARMS];
int alarmCount = 0;
String nowtime;
int val;
NTPClient timeClient(ntpUDP);

bool budilnik(){
 // int per;
   nowtime = timeClient.getFormattedTime();
   for (int i = 0; i < alarmCount; i++)
    {
        //Serial.print(alarms[i]+ " ");
        if (nowtime == alarms[i])
        return true;
    }
  return false;
}

void saveAlarm(String alarm)
{
    if (alarmCount == 10)
      return; 
    alarms[alarmCount] = alarm;  
    alarmCount++;
}

void deleteAlarm(String alarm)
{
  int pos = -1;
  for (int i = 0; i < alarmCount; i++)
  {
      if (alarms[i] == alarm)
        pos = i;
  }

  if (pos != -1)
  {
    alarms[pos] = "";
    for (int i = pos; i < MAX_ALARMS-1; i++)
    {
      alarms[i] = alarms[i+1];
    }
    alarmCount--;
  }
    
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2 
  pinMode(12, OUTPUT);
  pinMode(4, OUTPUT);

 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  //ntp
 timeClient.begin();
  timeClient.setTimeOffset(10800);
  //ntp
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  ////ntp
   timeClient.update();
   Serial.println(timeClient.getFormattedTime());

  delay(1000);
  
 if (budilnik() == true)
  { 
    tone (4, 500, 2000); //включаем на 500 Гц
  
//  delay(2000); //ждем 100 Мс
  
  //tone(4, 1000); //включаем на 1000 Гц
  
  //delay(100); //ждем 100 Мс
    digitalWrite(12, val);
    }
   

  //ntp
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  req = req.substring(0, req.indexOf(" HTTP/1.1"));
  client.flush();

 // timerecuest
   int Pos;
   
  if (req.indexOf("/setalarm/") != -1) {
    Pos = req.lastIndexOf("/");
    String alarmtime = req.substring(Pos+1);
    Serial.println(alarmtime);
    saveAlarm(alarmtime);
    
  } else if(req.indexOf("/deletealarm/") != -1) {
    Pos = req.lastIndexOf("/");
    String alarmtime = req.substring(Pos+1);
    deleteAlarm(alarmtime);
  } else if (req.indexOf("/gpio/1") != -1) {
    val = 1;
  } else {
    Serial.println("invalid request");
    client.stop();
    return;
  }
  
// Set Alarmtime according to the request

  // Set GPIO2 according to the request
 
//  digitalWrite(12, HIGH);

  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val) ? "high" : "low";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}
