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
#define wlan_pwd "xxx"
#define mqtt_server_ip "192.168.1.211"
#define esp_name_default "basement_light"


/*****************************************************************************************
 Hardware Settings
*****************************************************************************************/
//internal led


//NodeMcu (esp8266-12)
const int PIRPIN = D6;

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

const String pir1_name = "pir"; 
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

bool oldPIR1State;
int motion_counter = 0;
int ch1_change_counter = 0;
int ch2_change_counter = 0;
int ch3_change_counter = 0;
int ch4_change_counter = 0;

/*****************************************************************************************
 init / setup
*****************************************************************************************/

String topic_pub_pir;
String topic_pub_response;
String topic_sub;
String topic_sub_set_ch;
String topic_pup_status_log;

void setDefaultTopics(){
  topic_sub =             default_prefix + esp_name + "/cmd/#";
  topic_sub_set_ch =      default_prefix + esp_name + "/set_ch/#";
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
  pinMode(LED_BUILTIN, OUTPUT);
  
  pinMode(RELAIS1, OUTPUT);
  pinMode(RELAIS2, OUTPUT);
  pinMode(RELAIS3, OUTPUT);
  pinMode(RELAIS4, OUTPUT);
  
  pinMode(PIRPIN, INPUT);
  
  oldPIR1State = !digitalRead(PIRPIN);

  digitalWrite(LED_BUILTIN, HIGH); 
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
      client.subscribe(topic_sub.c_str());
      client.subscribe(topic_sub_set_ch.c_str());
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
}

/*****************************************************************************************
 CODE publish
*****************************************************************************************/
void sendMotionDetected(String pir)
{  
  String mqttMessage = "motion " + String(pir) + " ("+pir1_name+"=" + String(motion_counter) + ")";
  String topic = topic_pub_pir + pir;
  client.publish(topic.c_str(), mqttMessage.c_str());
}

void sendResponse(String rmsg)
{  
  client.publish(topic_pub_response.c_str(), rmsg.c_str());
}

void sendStatus(String cmd){
    uint32_t free_heap = ESP.getFreeHeap();
    
    snprintf (mqtt_msg, 510, "motion %d, reconnect ok=%d, err=%d, free_heap=%u, ch1=%d, ch2=%d,  ch3=%d,  ch4=%d",
    motion_counter, reconnect_success, reconnect_error, free_heap, ch1_change_counter, ch2_change_counter, ch3_change_counter, ch4_change_counter);
    client.publish(topic_pup_status_log.c_str(), mqtt_msg);    
}

/*****************************************************************************************
 CODE callback
*****************************************************************************************/ 
int ERR_MSG_count = 0;
void set_ch(String t, String p){
  
  int last_separator = t.lastIndexOf('/');
  String ch_nr = t.substring(last_separator+1);
  if (ch_nr == "set_ch"){ 
    if(p=="all on"){
      set_ch_helper(RELAIS1, 1, "on");
      set_ch_helper(RELAIS2, 2, "on");
      set_ch_helper(RELAIS3, 3, "on");
      set_ch_helper(RELAIS4, 4, "on");
      }else if(p=="all off"){
        set_ch_helper(RELAIS1, 1, "off");
        set_ch_helper(RELAIS2, 2, "off");
        set_ch_helper(RELAIS3, 3, "off");
        set_ch_helper(RELAIS4, 4, "off");
        }
        else{sendResponse("set_ch ERROR payload not all on, all off" + p);}
  }
  else if(ch_nr == "1"){ set_ch_helper(RELAIS1, 1, p);}
  else if(ch_nr == "2"){ set_ch_helper(RELAIS2, 2, p);}
  else if(ch_nr == "3"){ set_ch_helper(RELAIS3, 3, p);}
  else if(ch_nr == "4"){ set_ch_helper(RELAIS4, 4, p);}
  else{sendResponse("set_ch ERROR: " + ch_nr);}
}

void set_ch_helper(int relais, int ch_nr, String p){
  if (p == "on") {digitalWrite(relais, LOW); sendResponse( String(ch_nr) + " set_channel: on"); }
  else if (p == "off") {digitalWrite(relais, HIGH); sendResponse(String(ch_nr) + " set_channel: off");}
  else sendResponse("set_out ERROR: " + p);  
  }
  
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0'; // Make payload a string by NULL terminating it.
  String t = String(topic);

  String topic_prefix = default_prefix + esp_name + "/cmd/";
  String topic_set_ch = default_prefix + esp_name + "/set_ch";

  if(t.startsWith(topic_set_ch)){
    set_ch(t, (char*)payload);
  }
  else if(t == (topic_prefix + "echo")){
    sendResponse("hello from= " + String(esp_name) + " echo: " + (char*)payload);     
  }
  else if(t == topic_prefix +"set_name"){
    String new_name = (char*)payload;
    sendResponse("change esp_name from [" + String(esp_name) + "] to= [" + new_name + "]");
    client.unsubscribe(topic_sub.c_str());
    client.unsubscribe(topic_sub_set_ch.c_str());
    esp_name = new_name;
    setDefaultTopics();
    client.subscribe(topic_sub.c_str());
    client.subscribe(topic_sub_set_ch.c_str());
    
  }else if(t == topic_prefix + "send_intervall_status"){
    int tmp = atoi((char *)payload);
    sendResponse("change send_intervall_status from " + String(send_intervall_status) + " to " + String(tmp) );
    send_intervall_status = tmp;
    
  }else if(t == topic_prefix + "reset"){
    String reset_modul = (char*)payload;
    if(reset_modul=="setup"){
      sendResponse("reset esp (setup())");
      setup();
    }else if(reset_modul=="vars"){
      sendResponse("reset counter variables");
      reconnect_success = 0;
      reconnect_error = 0;
      motion_counter = 0;
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

bool ch1;
bool ch2;
bool ch3;
bool ch4;
void check_out_changes(){
  if(digitalRead(RELAIS1) != ch1){
    ch1 = digitalRead(RELAIS1);
    ch1_change_counter++;
    if(ch1)
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
  }

/*****************************************************************************************
 main loop
*****************************************************************************************/
long lastCycle = 0;



bool switch_flag = false;
void loop() {
  ArduinoOTA.handle();

  if (!client.connected()) { reconnect(); }
  client.loop(); //to enable MQTT callback usage
  
  //################### 1 second loop ################
  long now = millis();
  if (now - lastCycle > 1000) {
    lastCycle = now; 
    if(switch_flag){
      switch_flag = false;
      digitalWrite(LED_BUILTIN, LOW);
      //sendResponse("watchdog " + String(switch_flag));
      //sendStatus("dbg");
    }else{
      switch_flag = true;
      digitalWrite(LED_BUILTIN, HIGH);
      } 
  } 
  //################### 1 second loop ################
  
  //for IBS -> send every output change
  check_out_changes();
    
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
  
  /****************************************************/
  // environment sensors

  /****************************************************/
  // send status
  if (now - lastStatusMsgTime > send_intervall_status) {
    lastStatusMsgTime = now;
    sendStatus("my status");
  }
  
}
