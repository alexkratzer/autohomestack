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