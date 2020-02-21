# IoT multisensor

## ESP8266

To enable the esp8266 libs at Arduino IDE

* Go to File > Preferences -> Additional Board Manager URLs: http://arduino.esp8266.com/stable/package_esp8266com_index.json
* Tools > Board -> Boards Manager -> Search for ESP8266 and install

Dependency libs:

* git clone https://github.com/knolleary/pubsubclient.git

* git clone https://github.com/adafruit/DHT-sensor-library.git

* copy to ~/Arduino/libraries

Features:

* read temperatur / humility and publish cyclic

* read light lux and publish cyclic

* detect motion and publish at detection (on rising edge)

MQTT Interface

* subscription topics (start with prefix(iot/v01) + esp_name + topic_type)

* e.g. iot/v01/nodeMCUIBS/config/get_status

topic|meaning
-|-
/config/echo|just send an reply
/config/get_status[value: ""; status; prozess; heap]|
/config/set_name [new name]|change name of esp (with prefix for topics)
/config/send_intervall_sensor [new time in ms]|e.g. "10000" for 10s
/config/send_intervall_status|watchdog with status information
/config/reset [dht, setup, vars]|

GPIO usage

pin|DHT22|AM312|KY-018
-|-|-|-
1|VCC|VCC|GND
2|DATA=D7|OUT=D2/D5|VCC
3|nc|GND|DATA=A0
4|GND||

## 3D case

This is the 3D model of the case

![multisensor_case_3d_model](multisensor_case_3d_model.png)

## Downloads

You can download the STL file here: [new_multisensor_V0.1](new_multisensor_V0.1.stl)

The arduino sketch is stored here: [ah_iot_multisensor_v06](ah_iot_multisensor_v06.ino)