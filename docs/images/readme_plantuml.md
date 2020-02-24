# plantuml code for readme

until I get the plant-uml plugin working here is the code to the pictures

## Components of the auto home stack

``` plantuml

@startuml

skinparam component {
    FontColor          black
    AttributeFontColor black
    FontSize           17
    AttributeFontSize  15
    AttributeFontname  Droid Sans Mono
    BackgroundColor    #6A9EFF
    BorderColor        black
    ArrowColor         #222266
}

title auto home stack
skinparam componentStyle uml2

node "user interface"{
    frame "deprecated"{
        [dotnet pc tool] #Gray
    }
    [dotnet pc tool] <--> udp_plc
    [web client] <..> http
}

cloud {
    interface www
    [AWS] --> www
    [netatmo] -> www
}

node "ahs" {
    interface data_logger as data_logger
    interface mqtt
    [data logger server] as dls
    [rpi_nodered] as nr

    dls -up-> [dashboard]
    [dashboard] -> http
    dls <-- data_logger
    www -> nr
    http <--> nr
    nr -right-> [plc_xx]
    nr <-> dls
    nr <-down-> mqtt
    udp_plc <-> [plc_xx]
    data_logger <- [plc_xx]
    [plc_xy] -up-> [plc_xx]
    [esp] <.up.> mqtt
    [sensor] -up-> [plc_xx]
    [aktor] <-up- [plc_xx]
}

@enduml
```

## legend

``` plantuml
@startuml
node "Legend"{
    package "device"{
        [item 1] as TMS #Tomato
    }
    folder "tst"{
      [item 2] as CM #Lime
    }
}
@enduml
```

## node red

``` plantuml
@startuml

[*] --> NodeRed
udp -> NodeRed
NodeRed -> udp
udp : plc interface
mqtt --> NodeRed
NodeRed --> mqtt
mqtt : field level devices\nesp 8266,\nwatchdog services, ...
iot --> NodeRed
NodeRed --> iot
iot : other interfaces \nlike rest apis, AWS S3,\nssh to linux devices,\n...
NodeRed --> db
db : MySQL and \nInfluxDB

NodeRed : store all prozess \nand event data\nprozess data with rule engine\n
@enduml
```

## node_red rule engine

``` plantuml
@startuml
(*) --> new event

If "compare topic" then
--> [matches] "create alarm"
--> send to receiver
--> (*)
else
--> [no match] continue
Endif
--> (*)

@enduml
```
