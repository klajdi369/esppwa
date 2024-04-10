#include <Regexp.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include "time.h"
#include <EEPROM.h>



#define DHTPIN 16
#define DHTSTATUS 4
#define DHTTYPE DHT22   // DHT 11 

#define EEPROM_SIZE (900*2)+1

DHT dht(DHTPIN, DHTTYPE);

const char* ssid     = "Klajdi";
const char* password = "klajdi369";

const char* host = "as.klajdi.net";

int g_sTime1;
float g_h, g_t, g_f;
RTC_DATA_ATTR float failedTemps[900];
RTC_DATA_ATTR unsigned int failedTempsTimes[900];
RTC_DATA_ATTR int failedTempsIndex = 0;
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

  int wifiStartTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStartTime <= 3000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  configTime(0, 0, ntpServer);

  Serial.println("-----------------------------------");

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);

  // Read failedTempsIndex from EEPROM
  EEPROM.get(0, failedTempsIndex);
  // Read failedTemps and failedTempsTimes from EEPROM
  EEPROM.get(sizeof(failedTempsIndex), failedTemps);
  EEPROM.get(sizeof(failedTempsIndex) + sizeof(failedTemps), failedTempsTimes);
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

  sendGet(g_h, g_t, g_f, 0);
  digitalWrite(DHTSTATUS, LOW);
  
  ///request();
  
  Serial.println(getTime());
  
  // Disconnect from WiFi and turn off the module
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  esp_deep_sleep(300 * 1000000); /// seconds * useconds
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

  // Use HTTPClient class to create HTTP connections
  HTTPClient http;
  String url = "http://";
  url += host;
  url += "/esp.php";
  url += "?data1=";
  url += data1;
  url += "&data2=";
  url += data2;
  url += "&data3=";
  url += data3;
  if (epochTime != 0) {
    url += "&time=";
    url += epochTime;
  }

  http.begin(url);

  int httpCode = http.GET();
  if (httpCode <= 0) {
    failedTemps[failedTempsIndex] = data2;
    failedTempsTimes[failedTempsIndex] = getTime();
    failedTempsIndex++;
    // Write updated values to EEPROM
    EEPROM.put(0, failedTempsIndex);
    EEPROM.put(sizeof(failedTempsIndex), failedTemps);
    EEPROM.put(sizeof(failedTempsIndex) + sizeof(failedTemps), failedTempsTimes);
    EEPROM.commit();
    Serial.println("connection failed");
    Serial.println(failedTempsIndex);
  } else if (failedTempsIndex > 0) {
    int tempIndex = failedTempsIndex;
    failedTempsIndex = 0;
    for (int i = 0; i < tempIndex; i++) {
      sendGet(0, failedTemps[i], 0, failedTempsTimes[i]);
      Serial.println(failedTempsIndex);
    }
    // Write updated failedTempsIndex to EEPROM
    EEPROM.put(0, failedTempsIndex);
    EEPROM.commit();
    Serial.println("Committed");
  }

  Serial.println(url);
  http.end();
  Serial.println("closing connection");
  Serial.println("Requesting data");
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    // NTP time is available
    time(&now);
    // Store the current timestamp in EEPROM
    EEPROM.put(sizeof(failedTempsIndex) + sizeof(failedTemps) + sizeof(failedTempsTimes), now);
    EEPROM.commit();
    return now;
  } else {
    // NTP time is not available
    // Retrieve the last stored timestamp from EEPROM
    unsigned long lastStoredTime;
    EEPROM.get(sizeof(failedTempsIndex) + sizeof(failedTemps) + sizeof(failedTempsTimes), lastStoredTime);
    if (lastStoredTime == 0) {
      // No previous timestamp stored, return 0
      return 0;
    }
    // Calculate the elapsed time since the last stored timestamp
    unsigned long elapsedTime = millis() / 1000; // Convert milliseconds to seconds
    // Return the last stored timestamp plus the elapsed time
    return lastStoredTime + elapsedTime;
  }
}

///  WiFi.begin("Wokwi-GUEST", "", 6);