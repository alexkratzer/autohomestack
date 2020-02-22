# Raspberry setup

## Docker

Most of the control components are running as docker image on a raspberry.

Here are a few steps to set up:

* enable ssh server

* copy your puplic key to the raspberry for easier access

```bash
cat ~/.ssh/id_rsa.pub | ssh pi_name@192.168.xxx.xxx -p 22 'mkdir -p ~/.ssh && cat >> ~/.ssh/authorized_keys'
```

* get docker

```bash
`sudo apt-get update && sudo apt-get upgrade -y`
`curl -sSL https://get.docker.com | sh`
`sudo usermod -aG docker $USER`
```

* setup container **mqtt mosquitto**

```bash
docker run -itd -p 1883:1883 -p 9001:9001 --name mosquitto_ah eclipse-mosquitto
```

* setup container **node red**

Mind that as long as we run the mosquitto and node-red container at the same host the `--link mosquitto_ah:broker` is necessary.

If changing the deployment to different machines update the node-red-mqtt-server from broker to the new ip address

```bash
sudo chown -R 1000:1000 /home/pi/.node-red
docker run -it -p 1880:1880 -p 7724:7724/udp -p 7725:7725/udp -v /home/pi/.node-red:/data --restart=always --name nodered_ah --link mosquitto_ah:broker nodered/node-red
```

* setup container **grafana**

```bash
docker volume create grafana-storage
docker run -d -p 3000:3000 -v grafana-storage:/var/lib/grafana --name=grafana_ah grafana/grafana
```

* setup pi hole

The network ad blocking / monitoring software [pi-hole.net](https://pi-hole.net)

is not really connected to the home automation system but it is always a good idear to have control over your network.

## Script device_status.py

### Download

This script [device_status.py](../scripts/device_status.py) is executed from the main raspberry.

Some unit tests are [here](../scripts/test_device_status.py) available (I used pytest).

dependencies:

```bash
pip3 install paho-mqtt
```

If not using python3 replace `subprocess` with `commands`

### Purpose

Its purpose is to get the status (temperature, ram usage, ...) of devices with ssh interface.

Results will be puplished at MQTT for further processing.

current clients:

ip|name|component
-|-|-
192.168.1.39|og_floor|piCorePlayer
192.168.1.40|eg_living|piCorePlayer
192.168.1.41|og_bath|piCorePlayer
192.168.1.211|rpi4|node-red + other container
192.168.1.200|syn|NAS and home automation server

### Usage

* copy this script to clients:

```bash
scp ~/repo/path_to_script/device_status.py rpi:~
```

* To trigger this script we setup a crontab entry which executes this script every 10 minutes

```bash
crontab -e
*/10 * * * * /usr/bin/python /home/pi/device_status.py
```

* get logfile from rpi for debugging purpose

```bash
scp rpi:~/device_status.log ~/Desktop/LOG_device_status.log
```
