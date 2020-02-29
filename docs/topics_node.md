# topics

device-type|device-value
-|-
plc|eg, og, car,
esp|basement, carport, ..
netatmo|base, outside, rain
rpi3|self, media_eg, media_bath, media_hall

component-type|component-value|description
-|-|-
dl|weather, sensor, aktor|data logger events
status|error, self, ...
i|sensor_name, e.g. light_barrier, motion_sensor|input values
o|aktor_name, e.g. light_bath_ceiling|output values
get|global device values, e.g. get_all|get from device
set|global device cmd, e.g. set_verbose|set to device

## logging at DB

All topics containing /status/log will be stored at table **node_status_log**

* v01/netatmo/base/status/log
* v01/netatmo/outdoor/status/log
* v01/netatmo/rain/status/log
* v01/rpi/4/status/log
* v01/rpi/OG_floor/status/log
* v01/rpi/EG_living/status/log
* v01/rpi/OG_bath/status/log

## general structure of messages

|version    |device-type    |device-value   |component-type |component-value    |example msg
|-          |-              |-              |-              |-                  |-
|v01        |plc            |eg             |dl             |eta                |
|v01        |plc            |eg             |pd             |eta                |
|v01        |plc            |eg             |status         |error              |
|v01        |esp            |basement       |i              |pir1               |
|v01        |esp            |basement       |dl             |pir1               |

## plc -> node_red

plc function **NodeSend___send()** add prefix to topic **v01/plc/**
device-value is added from node_red while receiving (from IP address)

example

* `v01/plc/og/o/light/stairs_west`
* `v01/plc/eg/i/NodeDiFlag`

|component-type     |component-value    |message                |description
|-                  |-                  |-                      |-
|pd                 |eta                |values of ProzessData  |all pd components are filtert from node_log
|status             |error              |name + value           |description of error status => use FC NodeSend_status/error
|dl                 |eta                |values                 |maybe redundant to pd/eta ==> current deactivated at PLC
|o                  |heater/bath        |                       |
|i                  |[input name]       |{switches:value}       |plc detected input change (button, pir sensor, and so on...)
|i                  |cmd_local          |[cmd name]             |cmd from local input
|i                  |cmd_remote         |[cmd name]             |cmd from remote plc input

## node_red -> plc

## iot -> node_red

## node_red -> iot
