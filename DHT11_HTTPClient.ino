#include <Regexp.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include "time.h"



#define DHTPIN 16
#define DHTSTATUS 4
#define DHTTYPE DHT11   // DHT 11 



DHT dht(DHTPIN, DHTTYPE);

const char* ssid     = "Klajdi";
const char* password = "klajdi369";

const char* host = "as.klajdi.net";

int g_sTime1;
float g_h, g_t, g_f;
float failedTemps[6144];
unsigned int failedTempsTimes[6144];
int failedTempsIndex = 0;
const char* ntpServer = "pool.ntp.org";

void setup() {
  pinMode(DHTSTATUS, OUTPUT); //Pin for DHT11 status
  Serial.begin(115200);
  dht.begin();
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); //works!

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  configTime(0, 0, ntpServer);

  g_sTime1 = millis();
  Serial.println(g_sTime1);
  Serial.println("-----------------------------------");
}

void loop() {
  //NOTE: DHT11 takes ~2000ms to load
  float g_h = dht.readHumidity();
  float g_t = dht.readTemperature();
  float g_f = dht.readTemperature(true);
  if (isnan(g_h) || isnan(g_t) || isnan(g_f)) {
    Serial.println("Failed to read from DHT sensor!");
    digitalWrite(DHTSTATUS, HIGH);
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    delay(500);
    return;
  }


  
//  Serial.println(sTime1);
  
  if (millis() - g_sTime1 > 15000) {
    g_sTime1 = millis();
    Serial.println(LED_BUILTIN);
    sendGet(g_h, g_t, g_f, 0);
    digitalWrite(DHTSTATUS, LOW);
  }
  
  request();
  
  delay(1000);
}


void request() {

    HTTPClient http2;
    http2.begin("http://as.klajdi.net:80/espfilereq.php?request=1");
    http2.addHeader("Content-Type", "text/plain");  //Specify content-type header
   
    int httpCode = http2.GET();   //Send the request
    
    String line = http2.getString();
    
    char HTTPResponse[512];
    line.toCharArray(HTTPResponse, 512);
    Serial.println(HTTPResponse);

    MatchState ms;
    ms.Target(HTTPResponse);  // what to search
    char result = ms.Match("<span id='d1'>(.*)</span>"); 

    char buf[512];  // large enough to hold expected string
    Serial.println("_____________________________");
    Serial.print("Matched on: ");
    ms.GetCapture(buf, 0);

    int qPin = atoi(buf);
    Serial.println(qPin);
    int outPins[] = {0, 4, 5};
    for (int i=0; i<3; i++) {
      digitalWrite(outPins[i], LOW);
    }
    digitalWrite(qPin, 1);

}

void sendGet(float data1, float data2, float data3, unsigned long epochTime) {
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) { //works!
    failedTemps[failedTempsIndex] = data2;
    failedTempsTimes[failedTempsIndex] = getTime();
    failedTempsIndex++;
    Serial.println("connection failed");
    Serial.println(failedTempsIndex);
    delay(60000);
    return;
  } else if(failedTempsIndex > 0) {
    int tempIndex = failedTempsIndex;
    failedTempsIndex = 0;
    for(int i = 0; i < tempIndex; i++) {
      sendGet(0, failedTemps[i], 0, failedTempsTimes[i]);
    }
  }

  // We now create a URI for the request
  String url = "/esp.php";
  url += "?data1=";
  url += data1;
  url += "&data2=";
  url += data2;
  url += "&data3="; 
  url += data3;
  if(epochTime != 0) {
    url += "&time=";
    url += epochTime;
  }

  
  Serial.print("Sending data to URL: ");
  Serial.println(url);

//   This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close+-\r\n\r\n");
  Serial.println("closing connection");
  Serial.println("Requesting data");
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return(0);
  }
  time(&now);
  return now;
}

///  WiFi.begin("Wokwi-GUEST", "", 6);