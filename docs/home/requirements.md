# Requirements to the system

## Most important

!!! Hint

    In order to gain acceptance among stakeholders, it is absolutely necessary to guarantee the basic functions that are also available with a conventional installation.

This means that if the control level or process level devices fail, the following basic functions are retained:

* Switching the light on / off with conventional switches

* It must be possible to raise and lower the blinds by pressing buttons

* Sockets must be switched on moderately by default

* The heater must maintain the last specified temperature.

## Some more

type | description
-|-
robustnes|When the system is switched on (e.g. after a power failure) the system itself should start up and be active
robustnes|Services / applications on servers should be monitored automatically and restarted if necessary
robustnes|Backups of databases and other artifacts should be created automatically
robustnes|Basic functions like switching light on/off must work even if the control level or parts of the process level fail
interoperability|The different components of the system shall provide apis to exchange information and communicate with internal and external applications and systems.
interoperability|The system shall be open to different transport protocols and publishing / describing interfaces.
interoperability|For integrating iot devices the system shall provide a MQTT broker. That allows easy w-lan connecting of esp based devices
interoperability|The PLC shall communicate to sensors / actors hardwired, via modbus, analog voltage signals, ...
interoperability|The data formats, transport protocols and interfaces shall be derived from central managed masterdata
