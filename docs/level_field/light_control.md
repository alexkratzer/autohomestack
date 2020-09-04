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

![light_control_paulmann_duo_holder](light_control_paulmann_duo_holder.png)

Now the slicing.

I used the following parameters:

* Infill Density:           40%
* Printing Temperature:     200C
* Build Plate Temperature:  60C
* Print Speed:              60mm/s
* Support:                  No

![light_control_paulmann_duo_holder_slicing](light_control_paulmann_duo_holder_slicing.png)

