# light control in basement office/living room

## The idea

Install indirect lighting in a large basement room

## Components

* Paulmann Duo Profil 2m (70267)

* Self-printed corner profile holder

* NodeMCU as mqtt gateway

* 4x Relais to controll the power supplys

## The first prototype

![light_control_prototype](light_control_prototype.jpg)

## Room with dimensions

![room_plan_downstair_living](room_plan_downstair_living.svg)

## Room with LED planning

![room_plan_downstair_led](room_plan_downstair_led.svg)

### Length calculation

Distance profile to wall ~2cm

* **Office**

(2,65 - 0,04) * 2 = 5,22

(3,69 - 0,04) * 2 = 7,3

4 * 0,66 = 2,64

5,22m + 7,3m + 2,64m = **15,16m**

* **Living-room**

(3,5 - 0,04) * 2 = 6,92

(3,69 - 0,04) * 2 = 7,3     Variante 2: 3,65

2 * 0,66 = 1,32

6,92m + 7,3m + 1,32m = **15,54m**

### Some electrical calculations

Since there are long distances we use 24V.



Area | Direction | Length
-|-|-
Office|wall|
Office|ceiling|
Living-room|wall|
Living-room|ceiling|
