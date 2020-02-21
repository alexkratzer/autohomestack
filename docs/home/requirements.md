# Requirements to the system

type | description
-|-
robustnes|When switching on (e.g. after a power failure) the system itself should start up and be active
robustnes|Services / applications on servers should be monitored automatically and restarted if necessary
robustnes|Backups of databases and other artifacts should be created automatically
robustnes|Basic functions like switching light on/off must work even if the control level or parts of the process level fail
interoperability|The different components of the system shall provide apis to exchange information and communicate with internal and external applications and systems.
interoperability|The system shall be open to different transport protocols and publishing / describing interfaces.
interoperability|For integrating iot devices the system shall provide a MQTT broker. That allows easy w-lan connecting of esp based devices
interoperability|The PLC shall communicate to sensors / actors hardwired, via modbus, analog voltage signals, ...
interoperability|The data formats, transport protocols and interfaces shall be derived from central managed masterdata