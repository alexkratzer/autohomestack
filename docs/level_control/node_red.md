# Node red

As control system and for the connection of other devices I currently use [node-RED](https://nodered.org).

***Node-RED is a programming tool for wiring together hardware devices, APIs and online services***

It has already implemented interfaces (which are called 'nodes') to a lot of devices I use.

![NodeRedInterfaces](https://www.plantuml.com/plantuml/png/0/TP4nJyCm48Lt_uetepO3I1NAW0fYGMgH80C3ON3ZIsFXdeDzQGk_7ZlGKePErhtl-RspF8-YWmmRfzJhnHkcqniyUaFFPDHWUfoClsUM4XbfYTuri2mKMjsGsdo8_8iutabQXZ-E4gqbP-1eImw6jhQXM3F57jUNizcavfsMfZD-ZKXXf1CKHQ6iFn-VsCXBUEae_6qLKREpxuH0KQ1xcy9kNofKLwaenWxYuImF-yCcISvbf-5cfSmgHIy_gwS5D9kK-S2j6_RtT-heB1751u9s3dtmNnHpTtQdXxB0QD4r7qYUiBFI8GoEGBosJ3MhER59__4D "NodeRedInterfaces")

## Requirements regarding plugins

1. node-red-dashboard
1. node-red-contrib-fritz
1. node-red-contrib-s7
1. node-red-node-mysql
1. node-red-contrib-os
1. node-red-contrib-mqtt-broker
1. node-red-node-email
1. (outdated) node-red-contrib-netatmo
1. node-red-contrib-netatmo-dashboard
1. node-red-contrib-viera

## Dashboards

It is easy to make quick dash boards to monitor the sensor and health state of the devices.

![node_red_status_example](node_red_status_example.png)

Or to configure debug / control clients for different devices

![node_red_cmd](node_red_cmd.png)

## Rule engine

The rule engine is a function node that is triggered by the receipt of any message.

![node red rule engine](https://www.plantuml.com/plantuml/svg/0/FOvB3i8m34JtFeNL5I3r2gWR5fm1TM6bKpLIubIEMwvF21BRcJUVgRgO7pdHvNRbSRopuiqueKxqN7a89UzYO2zx30Fx1gKlzihYOKET6s8G1qiIoqCtAtHf2JO4n1FMnJP0I1M_k1Rk3JE7eXxr03rqYUiVfQcLj6CV "node red rule engine")

In this example rule a sensor event from the light barrier triggers the rule engine.

If the weather station detects that it is dark two lights are switched on.

```javascript
var plc_cmd = [];
if(t.startsWith('v01/plc/car/sensor/event')){
    var lux_weather = global.get('PD.plcog.weather.light_lux')

    //not a bug but a feature
    //weather station sends never 0 value
    //but 1 if no lux is detected
    if(lux_weather!=1){
        info = 'aktor/light set to value on'
        plc_cmd.push({ payload: "aktor/light/led_stripes/set_switch_value#on", topic: "car" });
        plc_cmd.push({ payload: "aktor/light/drive/set_switch_value#on", topic: "car" });  
    }else{
        info = 'no action since it is not dark'
    }

//for logging or later use a new alarm is generated
//and stored at the database
msg.alarm = {
    topic : t,
    status : 'created',
    time_created : getFormattedDate(),
    info : info
}

msg.topic = "INSERT INTO node_log(topic, message) VALUES ( \"rule_engine/"+t+"\", \""+info+"\" )"

//the alarm and commands for the PLC s are returned for further processing
return [msg, plc_cmd];
```

## Downloads

[node_red_flows repo](https://github.com/alexkratzer/autohomestack/tree/master/node_red_flows)