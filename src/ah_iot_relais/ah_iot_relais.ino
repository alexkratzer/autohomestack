/******************************************************************************************
## home automation iot light control by alexander kratzer
## for docu go to: https://autohomestack.de/level_field/iot_multisensor/
##
## V0.1 200904        copy from multisensor
## 
## to debug call: screen /dev/ttyUSB0 115200



******************************************************************************************/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//OTA update
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h> 

/*****************************************************************************************
 local settings (edit to fit at your own setup)
*****************************************************************************************/
#define wlan_ssid "sandman_V2"
#define wlan_pwd ""
#define mqtt_server_ip "192.168.1.211"
#define esp_name_default "basement_light"


/*****************************************************************************************
 Hardware Settings
*****************************************************************************************/
//NodeMcu (esp8266-12)

//light control
const int RELAIS1 = D2;
const int RELAIS2 = D3;
const int RELAIS3 = D8;
const int RELAIS4 = D7;

/*****************************************************************************************
 connection settings
*****************************************************************************************/
const char* ssid = wlan_ssid; 
const char* password = wlan_pwd;
const char* mqtt_server = mqtt_server_ip;
String esp_name = esp_name_default;


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
int ch1_change_counter = 0;
int ch2_change_counter = 0;
int ch3_change_counter = 0;
int ch4_change_counter = 0;

/*****************************************************************************************
 init / setup
*****************************************************************************************/

String topic_pub_response;
String topic_sub_set_ch;
String topic_pup_status_log;

void setDefaultTopics(){
  topic_sub_set_ch =      default_prefix + esp_name + "/set_ch/#";
  topic_pub_response =    default_prefix + esp_name + "/msg/";
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
  pinMode(RELAIS1, OUTPUT);
  pinMode(RELAIS2, OUTPUT);
  pinMode(RELAIS3, OUTPUT);
  pinMode(RELAIS4, OUTPUT);

  digitalWrite(RELAIS1, HIGH);
  digitalWrite(RELAIS2, HIGH);
  digitalWrite(RELAIS3, HIGH);
  digitalWrite(RELAIS4, HIGH);
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      ++reconnect_success;
      sendResponse("mqtt reconnect (success=" + String(reconnect_success) + " error=" + String(reconnect_error) + ")" );
      client.subscribe(topic_sub_set_ch.c_str());
    } else {
      ++reconnect_error;
      delay(5000);
    }
  }
}

void setup() {
  setup_sensor();
  setup_wifi();
  setup_mqtt();
  setup_ota();
}

/*****************************************************************************************
 CODE publish
*****************************************************************************************/
void sendResponse(String rmsg)
{  
  client.publish(topic_pub_response.c_str(), rmsg.c_str());
}

void sendStatus(String cmd){
    uint32_t free_heap = ESP.getFreeHeap();
    
    snprintf (mqtt_msg, 510, "reconnect ok=%d, err=%d, free_heap=%u, ch1_change_counter=%d, ch2_change_counter=%d,  ch3_change_counter=%d,  ch4_change_counter=%d",
    reconnect_success, reconnect_error, free_heap, ch1_change_counter, ch2_change_counter, ch3_change_counter, ch4_change_counter);
    //snprintf (mqtt_msg, 510, "reconnect ok=%d, err=%d, free_heap=%u",
    //reconnect_success, reconnect_error, free_heap);
    client.publish(topic_pup_status_log.c_str(), mqtt_msg);    
}

/*****************************************************************************************
 CODE callback
*****************************************************************************************/ 
void set_ch(String t, String p){
  if (p == "on") {digitalWrite(RELAIS1, LOW); sendResponse("set_channel 1: on"); }
  else if (p == "off") {digitalWrite(RELAIS1, HIGH); sendResponse("set_channel1: off");}
  else sendResponse("set_out ERROR: " + p);
}

  
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0'; // Make payload a string by NULL terminating it.
  String t = String(topic);

  String topic_set_ch = default_prefix + esp_name + "/set_ch/";

  if(t == topic_set_ch + "1"){
    set_ch(t, (char*)payload);
  }
}


/*****************************************************************************************
 main loop
*****************************************************************************************/
long lastCycle = 0;

bool ch1;
bool ch2;
bool ch3;
bool ch4;

void loop() {
  ArduinoOTA.handle();

  long now = millis();
  if (now - lastCycle > 1000) {
    lastCycle = now; 
    
  }

  if (!client.connected()) {
    // Serial.println("client reconnect");
    reconnect();
  }
  
  client.loop(); //to enable callback usage

  //for IBS -> send every output change
  if(digitalRead(RELAIS1) != ch1){
    ch1 = digitalRead(RELAIS1);
    ch1_change_counter++;
    sendResponse("RELAIS1 changed: " + String(ch1));
    }
  if(digitalRead(RELAIS2) != ch2){
    ch2 = digitalRead(RELAIS2);
    ch2_change_counter++;
    sendResponse("RELAIS2 changed: " + String(ch2));
    }
  if(digitalRead(RELAIS3) != ch3){
    ch3 = digitalRead(RELAIS3);
    ch3_change_counter++;
    sendResponse("RELAIS3 changed: " + String(ch3));
    }
  if(digitalRead(RELAIS4) != ch4){
    ch4 = digitalRead(RELAIS4);
    ch4_change_counter++;
    sendResponse("RELAIS4 changed: " + String(ch4));
    }
    
  /****************************************************/

  
  /****************************************************/
  // environment sensors

  /****************************************************/
  // send status
  if (now - lastStatusMsgTime > send_intervall_status) {
    lastStatusMsgTime = now;
    sendStatus("dbg");
  }
  
}
