/******************************************************************************************
## home automation iot multisensor by alexander kratzer
## V0.1 190410 (mqtt V01) initial creation
## 
## to debug call: screen /dev/ttyUSB0 115200

Dependency ESP8266:
Go to File > Preferences -> Additional Board Manager URLs: http://arduino.esp8266.com/stable/package_esp8266com_index.json 
Tools > Board -> Boards Manager -> Search for ESP8266 and install

Dependency libs:
git clone https://github.com/knolleary/pubsubclient.git
git clone https://github.com/adafruit/DHT-sensor-library.git
copy to ~/Arduino/libraries

Info: sketch is optimized for NodeMCU (ESP8266-12E)
for use with ESP8266-01 remove all OTA things (does not work as there is not enough space available).
update defines at Hardware Settings

Features
- read temperatur / humility and publish cyclic
- read light lux and publish cyclic
- detect motion and publish at detection (on rising edge)

MQTT Interface
subscription topics (start with prefix(iot/v01) + esp_name + topic_type)
e.g. iot/v01/nodeMCUIBS/config/get_status
  /config/echo -> just send an reply
  /config/get_status [value: ""; status; prozess; heap]
  /config/set_name [new name]-> change name of esp (with prefix for topics)
  /config/send_intervall_sensor [new time in ms] -> e.g. "10000" for 10s
  /config/send_intervall_status
  /config/reset [dht, setup, vars]


    DHT22         AM312         KY-018
1   VCC           VCC           GND
2   DATA=D7       OUT=D2/D5     VCC
3   nc            GND           DATA=A0
4   GND

https://www.instructables.com/id/NodeMCU-ESP8266-Details-and-Pinout/
https://esp8266-shop.com/esp8266-guide/esp8266-nodemcu-pinout/
digitalWrite did NOT work with GPIOs 6, 7, 8, 11, and ADC (A0)
digitalRead did NOT work with GPIOs 1, 3, 6, 7, 8, 11, and the ADC (A0)


******************************************************************************************/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

//OTA update
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h> 

/*****************************************************************************************
 local settings (edit to fit at your own setup)
*****************************************************************************************/
#define wlan_ssid "xxx"
#define wlan_pwd "xxx"
#define mqtt_server_ip "192.168.xxx.xxx"
#define esp_name_default "default"


/*****************************************************************************************
 Hardware Settings
*****************************************************************************************/
//NodeMcu (esp8266-12)
#define DHTPIN D7
#define PIRPIN D2
#define PIRPIN2 D5
#define LDRPIN A0
#define DHTTYPE DHT22

//esp8266-01
//#define DHTPIN 2 
//#define PIRPIN 0


// just for IBS to read free heap
uint32_t free_startup_heap = 0;

/*****************************************************************************************
 connection settings
*****************************************************************************************/
const char* ssid = wlan_ssid; 
const char* password = wlan_pwd;
const char* mqtt_server = mqtt_server_ip;
String esp_name = esp_name_default;
const String pir1_name = "corridor"; 
const String pir2_name = "stairs";

/*****************************************************************************************
 mqtt staff
*****************************************************************************************/
WiFiClient espClient;
PubSubClient client(espClient);

String default_prefix = "v01/esp/";

char mqtt_msg[512];
long lastStatusMsgTime = 0;

int reconnect_success = 0;
int reconnect_error = 0;

int send_intervall_status_DEFAULT = 60000; //1000=1s, 60000=1m, 600000 = 10m
int send_intervall_status = send_intervall_status_DEFAULT;

/*****************************************************************************************
 sensor staff
*****************************************************************************************/
DHT dht(DHTPIN, DHTTYPE);
int send_intervall_sensor_DEFAULT = 10000;  //10000 -> 10s

int send_intervall_sensor = send_intervall_sensor_DEFAULT;
int error_count_dht = 0; //just for IBS
unsigned long pup_cyclic_sensor_count = 0;
int dht_error_at_nr = 0; //just for IBS
long lastCyclicCensorTime = 0;

bool oldPIR1State;
bool oldPIR2State;
int motion_counter = 0;
int motion_counter2 = 0;

/*****************************************************************************************
 init / setup
*****************************************************************************************/
long DBG_start_now = 0;

String topic_pub_pd;
String topic_pub_pir;
String topic_pub_response;
String topic_sub;
String topic_pup_status_log;

void setDefaultTopics(){
  topic_sub =             default_prefix + esp_name + "/cmd/#";
  topic_pub_pd =          default_prefix + esp_name + "/pd/";
  topic_pub_pir =         default_prefix + esp_name + "/i/";
  topic_pub_response =    default_prefix + esp_name + "/response/";
  topic_pup_status_log =  default_prefix + esp_name + "/status/log/";
}

void setup_wifi() {
  delay(10);

  WiFi.mode(WIFI_STA); //OTA
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  randomSeed(micros());
}



void setup_mqtt(){
  setDefaultTopics();
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqtt_callback);
}

void setup_ota(){  
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
   ArduinoOTA.setHostname(esp_name.c_str());

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    // Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    // Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    // Serial.printf("Progress= %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    // Serial.printf("Error[%u]= ", error);
    if (error == OTA_AUTH_ERROR) {
      // Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      // Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      // Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      // Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      // Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  // Serial.println("Ready");
  // Serial.print("IP address= ");
  // Serial.println(WiFi.localIP());
}

void setup_sensor(){
  pinMode(PIRPIN, INPUT);
  pinMode(PIRPIN2, INPUT);
  pinMode(LDRPIN, INPUT);
  
  oldPIR1State = !digitalRead(PIRPIN);
  oldPIR2State = !digitalRead(PIRPIN2);
  dht.begin();
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      ++reconnect_success;
      sendResponse("mqtt reconnect (success=" + String(reconnect_success) + " error=" + String(reconnect_error) + ")" );
      client.subscribe(topic_sub.c_str());
    } else {
      ++reconnect_error;
      delay(5000);
    }
  }
}

void setup() {
  setup_sensor();
  // Serial.begin(115200);
  setup_wifi();
  setup_mqtt();
  setup_ota();
  DBG_start_now = millis();
}

/*****************************************************************************************
 CODE publish
*****************************************************************************************/
void sendMotionDetected(String pir)
{  
  String mqttMessage = "motion " + String(pir) + " ("+pir1_name+"=" + String(motion_counter) + " "+pir2_name+"=" + String(motion_counter2) + ")";  
  client.publish(topic_pub_pir.c_str(), mqttMessage.c_str());
}

void sendResponse(String rmsg)
{  
  client.publish(topic_pub_response.c_str(), rmsg.c_str());
}

void sendStatus(String cmd){
  /*
    uint32_t free_heap = ESP.getFreeHeap();
    snprintf (mqtt_msg, 510, "cyclic_pup=%u, DHTerrAT=%d, pir_pup=%d, client.state=%d, free_heap=%u, reconnect ok=%d, err=%d, motion detected %s=%d, %s=%d", 
    pup_cyclic_sensor_count, dht_error_at_nr, (motion_counter + motion_counter2), client.state(), free_heap, reconnect_success, reconnect_error, pir1_name.c_str(), motion_counter, pir2_name.c_str(), motion_counter2);
    //snprintf (mqtt_msg, 510, "cyclic_pup=%u, reconnect ok=%d, err=%d", pup_cyclic_sensor_count, reconnect_success, reconnect_error);
    client.publish(topic_pup_status_log.c_str(), mqtt_msg);

    
*/

    //snprintf (mqtt_msg, 510, "cyclic_pup=%u, DHTerrAT=%d, pir_pup=%d, reconnect ok=%d, err=%d, motion detected %s=%d, %s=%d", 
    //pup_cyclic_sensor_count, dht_error_at_nr, (motion_counter + motion_counter2), reconnect_success, reconnect_error, pir1_name.c_str(), motion_counter, pir2_name.c_str(), motion_counter2);
    uint32_t free_heap = ESP.getFreeHeap();
    
    snprintf (mqtt_msg, 510, "cyclic_pup=%u, motion %d, reconnect ok=%d, err=%d, free_startup_heap=%u, free_heap=%u, ",
    pup_cyclic_sensor_count, (motion_counter + motion_counter2), reconnect_success, reconnect_error, free_startup_heap, free_heap);
    client.publish(topic_pup_status_log.c_str(), mqtt_msg);    
  /*
    uint32_t free_SketchSpace = ESP.getFreeSketchSpace();
    uint32_t SketchSize = ESP.getSketchSize();
    snprintf (mqtt_msg, 510, "heap at start=%u cur=%u sketch free=u% used=%u", free_startup_heap, free_heap, free_SketchSpace, SketchSize);
    */
}



/*****************************************************************************************
 CODE callback
*****************************************************************************************/ 
int ERR_MSG_count = 0;

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0'; // Make payload a string by NULL terminating it.
  String t = String(topic);

  String topic_prefix = default_prefix + esp_name + "/cmd/";
  
  if(t == (topic_prefix + "echo")){
    sendResponse("hello from= " + String(esp_name) + " echo: " + (char*)payload);
       
  }else if(t == topic_prefix +"set_name"){
    String new_name = (char*)payload;
    sendResponse("change esp_name from [" + String(esp_name) + "] to= [" + new_name + "]");
    client.unsubscribe(topic_sub.c_str());
    esp_name = new_name;
    setDefaultTopics();
    client.subscribe(topic_sub.c_str());
    
  }else if(t == topic_prefix + "send_intervall_sensor"){
    int tmp_sensor_send_intervall = atoi((char *)payload);
    sendResponse("change send_intervall_sensor from " + String(send_intervall_sensor) + " to " + String(tmp_sensor_send_intervall) );
    send_intervall_sensor = tmp_sensor_send_intervall;
    
  }else if(t == topic_prefix + "send_intervall_status"){
    int tmp = atoi((char *)payload);
    sendResponse("change send_intervall_status from " + String(send_intervall_status) + " to " + String(tmp) );
    send_intervall_status = tmp;
    
  }else if(t == topic_prefix + "reset"){
    String reset_modul = (char*)payload;
    if(reset_modul=="dht"){
      sendResponse("reset dht (dht.begin())");
      dht.begin();
    }else if(reset_modul=="setup"){
      sendResponse("reset esp (setup())");
      setup();
    }else if(reset_modul=="vars"){
      sendResponse("reset counter variables");
      reconnect_success = 0;
      reconnect_error = 0;
      motion_counter = 0;
      motion_counter2 = 0;
      pup_cyclic_sensor_count = 0;
      send_intervall_sensor = send_intervall_sensor_DEFAULT;
      send_intervall_status = send_intervall_status_DEFAULT;
      ERR_MSG_count = 0;
      }else{
      if(ERR_MSG_count < 10){
        sendResponse("received unknown msg at topic " + topic_prefix +"reset= " + reset_modul); 
        ERR_MSG_count = ERR_MSG_count + 1;
        }
      }
      
    }else if (t == topic_prefix + "status"){
      sendResponse("try to send status"); 
      sendStatus("dbg");    
      // String cmd = (char*)payload;
    
    }else{ 
    if(ERR_MSG_count < 10){
      sendResponse("received unknown topic= " + t); 
      ERR_MSG_count = ERR_MSG_count + 1;
      }
    }
}


/*****************************************************************************************
 main loop
*****************************************************************************************/
long lastCycle = 0;
void loop() {
  ArduinoOTA.handle();
  long now = millis();
  if (now - lastCycle > 1000) {
    // Serial.print(pup_cyclic_sensor_count);
    // Serial.print(", ");
    lastCycle = now;
  }

  if (!client.connected()) {
    // Serial.println("client reconnect");
    reconnect();
  }
  
  client.loop(); //to enable callback usage

  //DBG -> only at first loop itteration
  if(free_startup_heap==0){
     free_startup_heap = ESP.getFreeHeap();
  }

  /****************************************************/
  // pir
  
  int inputState = digitalRead(PIRPIN);
  if (inputState != oldPIR1State)
  {
    oldPIR1State = inputState;
    if(inputState){
        ++motion_counter;
        sendMotionDetected(pir1_name);
    }
  }

  int inputState2 = digitalRead(PIRPIN2);
  if (inputState2 != oldPIR2State)
  {
    oldPIR2State = inputState2;
    if(inputState2){
        ++motion_counter2;
        sendMotionDetected(pir2_name);
    }
  }
  
  /****************************************************/
  // environment sensors
  
  if (now - lastCyclicCensorTime > send_intervall_sensor) {
    lastCyclicCensorTime = now;
    ++pup_cyclic_sensor_count;
  
    //dht staff
    float tempValue = dht.readTemperature();
    float humidityValue = dht.readHumidity();
    
    if (isnan(tempValue) || isnan(humidityValue)) {
      ++error_count_dht;
      if(dht_error_at_nr==0){
        dht_error_at_nr = pup_cyclic_sensor_count;
      }
    }

    /****************************************************/
    //only NodeMcu, not available to ESP8266-01
    int luxValue = analogRead(LDRPIN);

    //publish with mqtt
    
    snprintf (mqtt_msg, 510, "tmp=%4.1f, humid=%2.1f, lux=%d", tempValue, humidityValue, luxValue);
    client.publish(topic_pub_pd.c_str(), mqtt_msg);
  }

  /****************************************************/
  // send status
  if (now - lastStatusMsgTime > send_intervall_status) {
    lastStatusMsgTime = now;
    sendStatus("dbg");
    /*
    lastStatusMsgTime = now;

    uint32_t free_heap = ESP.getFreeHeap();
    snprintf (mqtt_msg, 510, "cyclic_pup=%u, DHTerrAT=%d, pir_pup=%d, client.state=%d, free_heap=%u, reconnect ok=%d, err=%d", pup_cyclic_sensor_count, dht_error_at_nr, (motion_counter + motion_counter2), client.state(), free_heap, reconnect_success, reconnect_error);
    // snprintf (mqtt_msg, 255, "cyclic_pup=%u, reconnect ok=%d, err=%d", pup_cyclic_sensor_count, reconnect_success, reconnect_error);
    client.publish(topic_pup_status_log.c_str(), mqtt_msg);
    */
  }
}
