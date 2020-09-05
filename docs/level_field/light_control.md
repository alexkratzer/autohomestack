# Light control office/wellness-area

## The idea

Install indirect lighting in a large basement room.

![room_plan_downstair_living](room_plan_downstair_living.svg)

The room is divided into a working and a wellness-area.

Each area should be indirectly illuminated in the wall/ceiling corners through led stripes.

## Components

* Paulmann Duo Profil 2m (70267)

* Self-printed corner profile holder

* NodeMCU as mqtt gateway

* 4x Relais to controll the power supplys

* 4x 24V LED power supply

* LED Stripes

## The first prototype

To find out what it might look like and check if the `self-printed corner profile holders` work.

![light_control_prototype](light_control_prototype.jpg)

## Room with LED planning

![room_plan_downstair_led](room_plan_downstair_led.svg)

### Length calculation LED-stripes

Distance profile to wall ~2cm

Area|calculation|result|total
-|-|-|-
**Office** ceiling/white|(2,65 - 0,04) x 2|5,22m|
**Office** ceiling/white|(3,69 - 0,04) x 2 |7,3m|
**Office** ceiling/white|4 x 0,66 |2,64m|
**Office** ceiling/white|5,22m + 7,3m + 2,64m | | **15,16m**

* **Office** wall/blue

(2,65 - 0,04) x 2 = **5,22** / 3,69 - 0,04 = **3,65** / 4 x 0,66 = **2,64**

5,22m + 3,65m + 2,64m = **11,51m**

* **wellness-area** ceiling/white

(3,5 - 0,04) x 2 = **6,92** / (3,69 - 0,04) x 2 = **7,3** / 2 x 0,66 = **1,32**

6,92m + 7,3m + 1,32m = **15,54m**

* **wellness-area** wall/blue

(3,5 - 0,04) x 2 = **6,92** / 3,69 - 0,04 = **3,65** / 2 x 0,66 = **1,32**

6,92m + 3,65m + 1,32m = **11,89m**

### Length calculation power

The longest supply line is about 6,5 meters.

In total (16m office + 28m wellness-area about  = 44m 0,75mm²


![room_plan_downstair_power](room_plan_downstair_power.svg)

Since there are long distances we use 24V.

Assumption the wellness-area white leds have 9,6 watt per meter: 9,6w/m * 16m = ~150 watt total.

The power supply is divided between two cables = ~75 watt + about 20% reserve = ~90 watt.

With cable cross section 0,75mm² leads to ~5% voltage drop (which is ok).

### Summary

Area|Direction|Length LED|Length power
-|-|-|-
Office|wall/blue|11,51m|2x4
Office|ceiling/white|15,16m|2x4
wellness-area|wall/blue|11,89m|2x7m
wellness-area|ceiling/white|15,54m|2x7m

## Components in detail

### Corner profile holder

The aluminium profile mounting design.

I used `tinkercad` which is for free, runs in browser and is easy to use.

![light_control_paulmann_duo_holder](light_control_paulmann_duo_holder.png)

If you want to download the stl file [klick here](https://github.com/alexkratzer/autohomestack/tree/master/stl_print)

Now the slicing. There I used `Ultimaker Cura` with the following parameters:

* Infill Density:           40%
* Printing Temperature:     200C
* Build Plate Temperature:  60C
* Print Speed:              60mm/s
* Support:                  No

![light_control_paulmann_duo_holder_slicing](light_control_paulmann_duo_holder_slicing.png)

### Node MCU controller

As basis for the sketch I used the iot_multisensor.

The biggest difference is that the light control also controls outputs.

Hardware settings

```c
const int RELAIS1 = D2;
const int RELAIS2 = D3;
const int RELAIS3 = D8;
const int RELAIS4 = D7;
```

```c
void setup_sensor(){
  pinMode(RELAIS1, OUTPUT);
  ...
  digitalWrite(RELAIS1, HIGH);
  ...
}
```

Now we can control the relais with the `digitalWrite()` function.

```c
// activate
digitalWrite(RELAIS1, LOW)
// and to disable again
digitalWrite(RELAIS1, HIGH)
```

#### MQTT interface

To control the relais from remote we use a new the `/set_ch/` endpoint.

`v01/esp/basement_light/set_ch/` + `<RELAIS>`

```c
void setDefaultTopics(){
  topic_sub_set_ch =      default_prefix + esp_name + "/set_ch/#";
}

client.subscribe(topic_sub_set_ch.c_str());
```

The callback function gets expanded

```javascript
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String topic_set_ch = default_prefix + esp_name + "/set_ch/";

    if(t == topic_set_ch + "1"){
        set_ch(t, (char*)payload);
    }
}

void set_ch(String t, String p){
  if (p == "on") {digitalWrite(RELAIS1, LOW); sendResponse("set_ch1: on"); }
  else if (p == "off") {digitalWrite(RELAIS1, HIGH); sendResponse("set_ch1: off");}
  else sendResponse("set_out ERROR: " + p);
}
```

#### Set RELAIS from NodeRed

Internaly at NodeRed the following topic is used:

`v01/node_red_cmd/esp/<ESP_NAME>/set_ch/<RELAIS>`

* ESP_NAME example: control_light
* RELAIS: 1, 2, 3, 4

Payload: on, off, switch

This code is added to the `rule engine` function

```javascript

if(t.startsWith('v01/node_red_cmd')){
    ...
    if(t.startsWith('v01/node_red_cmd/esp/set_ch')){

        mqtt_msg.push({ topic: 'v01/esp/basement_light/set_ch/1', payload: p });
    }
    ...
}

```
