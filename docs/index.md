# Auto home stack

another self-built home automation system

## Introduction

My requirement was a robust, self-sufficient control system with which I could not depend on a proprietary system from a single manufacturer but could expand with hardware / software from different manufacturers.

Here my choice fell on Simatic CPUs. The sensors and actuators are wired to these and they process the control program.

Via the LAN interface, the CPUs communicate with a NAS running a data logger script. Here various data are stored in a mySQL database for later evaluation.

As a central control system I use NodeRed which provides a webserver for operation (and a lot of help during development).

## Requirements

requirement | feature
-|-
r|f

### Robustnes

R: When switching on (e.g. after a power failure) the system itself should start up and be active.

R: Services / applications on servers should be monitored automatically and restarted if necessary.

R: Backups of databases and other artifacts should be created automatically.

### Interoperability

R: The different components of the system shall provide apis to exchange information and communicate with internal and external applications and systems.

R: The system shall be open to different transport protocols and publishing / describing interfaces.

F: For integrating iot devices the system provides a MQTT broker. That allows easy w-lan connecting of esp based devices.

F: The PLC are communicating to sensors / actors hardwired, via modbus, analog voltage signals, ...

R: The data formats, transport protocols and interfaces shall be derived from central managed masterdata

## Components of the auto home stack

![auto home stack](https://www.plantuml.com/plantuml/png/0/RLB1Rjim3BtxAuZax00jq0m3WgBPrkoqKmux54QWM6fin1OrYfwR3VdtKPRjn4ju8z-Z-1wf5y-AkAch9F77qZf5geOSQuVMM8Q_2KXiqF9Nh91WZ7sbycC7hfcft3TiBgmB66hRye-vDCB3fzksI7bukaMigWNvHbXgs2hhuGTQx6XVPCQ1iB5wb3PVhZ-_RZOHHjA69gglD1DXEtKqVvHOBfDpad39bG7LC4A1CbvM97rtrhFban1bUOz9Ob4Rc3NU49IM3RshtCpY-jufc9XfuyZaYetkwo7UDB8r32u7Hgmoc7ydTUhWStANi4hJPsYqsxagZupMx26lIX4aw6Bn30Mp2qRo2XlTtt0K1MtRnhxrpsq6uRMn8eCKroZLM3mFlbJXPpSFQSLgL-7X89wLlqx_8zQ_c7ipme6-HLRrsr3MjLO-ukJANPX8HdU0fQG3X01fuqIMFE6BFIhIsV3aa0VLdVMXGrPreqfi1PwDMQ37ZVO5Iv1IUIWuZ98Dxpu-iW5OIMrGsgKQMokrqLv_bOuuQRxUr2gTOZ4vPOE_2MQy2pTjtE9gp9itrYFTxr173j0gO1T83YdnOgoMt_eF "auto home stack")

[source](/images/readme_plantuml.md)

### Control system

TODO: node red

### Processing level

The core components of my home automation solution consist of Siemens Simatic 1200 series CPUs. These are actually used in industrial environments (so they meet my requirements regarding robustness) and were priced within my budget.

The picture shows a sub-distributor during the construction phase. Right beside the multimeter you can see a 1214C CPU with 3x DI/DO expansion modules.

The CPUs are programmed with SCL (Structured Control Language). The syntax is similar to Pascal. In the following I describe the user program in excerpts.

![Unterverteiler_Bauphase](/images/Unterverteiler_Bauphase.JPG)

### field bus -> input/ output signals

TODO: esp

## Simatic PLC programm

### aktor interface

My focus was on a generic solution in order to be able to extend the control program easily or to port it to another environment without major adaptations.

For each actuator type (light, jalouse, socket, heating) there is a general function module that is instantiated for each device.

![TIA_multiinstanz](/images/TIA_multiinstanz.PNG)

Here is an example of the light building block. This is called several times in a superimposed "control" block.

***ash became light***

```javascript
IF #turn_on THEN
    #switch_signal := true;
END_IF;

#Tturn_off(IN:=#turn_off, PT:=t#5s, Q=>#tmp_turn_off_hold);

IF #param."auto-off_lux_enable" THEN
    #"Tauto-off-lux"(IN := "sense_lux_stairs_og" > #param."auto-off_lux",
                    PT := t#10s,
                    Q => #tmp_turn_off_lux
    );
END_IF;
```

Since the interior of the house took place over the winter, I realized the light building block first.

The module has one input to change the output signal and one input to switch it on and one input to switch it off.

The output switches directly the coupling relay which controls the physical light.

In the "inner life" of the module there is a delay to prevent the rear switching on/off (I implemented this function because of a stress test of my kids). 

In addition there is a timer which switches off the light after a configured time and the possibility to also switch off the light if a brightness value is exceeded.

***and getting dark again***

The blind building block was a little trickier. Because I can't evaluate the signal of the physical end stop in the control I measured the time of the up and down movement.

The target position and the target angle of the slats (each in 0 - 100%) are transferred to an extra "blind motor" module.

In this module, the time of the actual position and the actual angle is continuously normalized and the target values are approached in a state machine. 

In addition, I have locked a simultaneous up and down signal at the outputs at different positions.

```javascript
// normierung der position/zeit zwischen 0 - 100 
#cur_data.position := REAL_TO_INT(NORM_X(MIN := 0, VALUE := #cur_data.position_in_time, MAX := #DRIVING_TIME_POSITION) * #DIGIT_SCALA);
#cur_data.angle := REAL_TO_INT(NORM_X(MIN := 0, VALUE := #cur_data.angle_in_time, MAX := #DRIVING_TIME_ANGLE) * #DIGIT_SCALA);
END_IF;
```

The blind module has the following features:

* When the up/down button is pressed briefly, the blind moves to the respective end position (i.e. completely up or completely down).

* If the blind is moving and the up/down button is pressed again, the motor stops.

* If a button is pressed for a longer period of time (> 300ms have become established), the motor moves until the button is released again.

* Automatic startup when a parameterized wind speed is exceeded (measured by the weather station)

* Time-controlled approach of a position and an angle (I have planned 10 events so far but needed a maximum of 4)

* still on the TO-DO list: event-based approaching of an angle due to solar radiation and room temperature

***heating module***

The heating module is kept simpler again.

Due to the relatively sluggish wall heating, I have not yet implemented an exact regulation of the heating circuits.

In order to avoid a constant toggling the servomotors switch off only with 0,5 degrees over target.

In addition, an interval can be parameterized if the temperature falls below the target value and the time of day during which the module is to be active.

For IBS purposes (and e.g. vacation time), the PC-Tool can also be used to switch to manual mode (defined on or off).

***TO-DO Current***

I have not yet realized the socket module. Currently I only switch the sockets in the children's room.

Since here however a bedside lamp is switched I also used the light component for it.

### sensor interface

***wetter station***

For the weather station I decided for the P03/3-Modbus from Elsner.

With it I can get the weather data: Brightness (east, south and west sun), wind speed, temperature and precipitation.

With the Modbus variant I want to keep the possibility open to connect several participants (in special room sensors) to the control system via the bus system.

***heating with ETA***

My heating consists of the 20qm solar collectors and the "romantic" stove in the living room as well as an ETA SH20 twin.

During the installation I saw that a maintenance interface is attached to the display.

On inquiry with the manufacturer I got the RS232 protocol and thus the possibility by a CM1241 to access it.

### communication

For external communication each CPU contains a net_interface block.

Here a UDP/IP server is realized in a state machine which can be addressed e.g. via the PC-Tool.

Each instance of an actuator block is given a unique ID. This ID can also be found in the PC tool.
