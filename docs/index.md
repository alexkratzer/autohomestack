# another, self-built, home automation system

In the following I present my home automation solution. Originally I wanted to have a documentation for myself.

In the meantime, the system has proven itself and maybe you want to replicate parts of it.

## Introduction

My main focus was a robust, self-sufficient control system with which I could not depend on a proprietary system from a single manufacturer but could expand with hardware / software from different manufacturers.

Here my choice fell on Simatic CPUs. The sensors and actuators are wired to these and they process the control program.

Via the LAN interface, the CPUs communicate with the other home automation parts like:

* A NAS where the data logger server is running. Here various data are stored in a mySQL database for later evaluation.

* Several Raspberry Pi's are used for multimedia purpose and to run docker containers.

* As a central control system I use NodeRed which provides a webserver for operation (and a lot of help during development).

## Business view

![business_view.svg](drawio/business_view.svg)

This picture shows the allocation of the components in 3 levels.

### Levels

1. The end-user can control and observe the system with web services / web based applications.

1. The process level devices are mainly housed in control cabinets and a server cabinet.

1. The field level components are the sensors and actuators of the system with which the end user mainly interacts.

### Actuators

Actuators include lights, blinds, sockets, heaters and other powered devices.

### Sensors

Actively used sensors are mainly switches and buttons as in a traditional house installation.

Additional sensors measure among other things: movement, temperature, humidity, brightness, noise, sunlight per direction, wind speed, rain, indoor air quality and any amount of data from the heating control system

## Disclaimer

!!! hint

    Of course I am not liable for any damage caused by replicas ;-)
