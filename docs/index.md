# another self-built home automation system

My main focus was a robust, self-sufficient control system with which I could not depend on a proprietary system from a single manufacturer but could expand with hardware / software from different manufacturers.

Here my choice fell on Simatic CPUs. The sensors and actuators are wired to these and they process the control program.

Via the LAN interface, the CPUs communicate with a NAS running a data logger script. Here various data are stored in a mySQL database for later evaluation.

As a central control system I use NodeRed which provides a webserver for operation (and a lot of help during development).

![AutoHome_uebersicht](images/AutoHome_uebersicht.png)

## Requirements

type | desc
-|-
robustnes|When switching on (e.g. after a power failure) the system itself should start up and be active
robustnes|Services / applications on servers should be monitored automatically and restarted if necessary
robustnes|Backups of databases and other artifacts should be created automatically.
interoperability|The different components of the system shall provide apis to exchange information and communicate with internal and external applications and systems.
interoperability|The system shall be open to different transport protocols and publishing / describing interfaces.
interoperability|For integrating iot devices the system provides shall provide a MQTT broker. That allows easy w-lan connecting of esp based devices
interoperability|The PLC shall communicate to sensors / actors hardwired, via modbus, analog voltage signals, ...
interoperability|The data formats, transport protocols and interfaces shall be derived from central managed masterdata

## Components

![auto home stack](https://www.plantuml.com/plantuml/png/0/RLB1Rjim3BtxAuZax00jq0m3WgBPrkoqKmux54QWM6fin1OrYfwR3VdtKPRjn4ju8z-Z-1wf5y-AkAch9F77qZf5geOSQuVMM8Q_2KXiqF9Nh91WZ7sbycC7hfcft3TiBgmB66hRye-vDCB3fzksI7bukaMigWNvHbXgs2hhuGTQx6XVPCQ1iB5wb3PVhZ-_RZOHHjA69gglD1DXEtKqVvHOBfDpad39bG7LC4A1CbvM97rtrhFban1bUOz9Ob4Rc3NU49IM3RshtCpY-jufc9XfuyZaYetkwo7UDB8r32u7Hgmoc7ydTUhWStANi4hJPsYqsxagZupMx26lIX4aw6Bn30Mp2qRo2XlTtt0K1MtRnhxrpsq6uRMn8eCKroZLM3mFlbJXPpSFQSLgL-7X89wLlqx_8zQ_c7ipme6-HLRrsr3MjLO-ukJANPX8HdU0fQG3X01fuqIMFE6BFIhIsV3aa0VLdVMXGrPreqfi1PwDMQ37ZVO5Iv1IUIWuZ98Dxpu-iW5OIMrGsgKQMokrqLv_bOuuQRxUr2gTOZ4vPOE_2MQy2pTjtE9gp9itrYFTxr173j0gO1T83YdnOgoMt_eF "auto home stack")

[source](images/readme_plantuml.md)

here another view of the main components

![AutoHome_Blockschaltbild](images/AutoHome_Blockschaltbild.png)

!!! warning

    of course I am not liable for any damage caused by replicas ;-)
