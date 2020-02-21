# Raspberry setup

Most of the control components are running as docker image on a raspberry.

Here are a few steps to set up:

* enable ssh server

* copy the puplic key to the raspberry for easier access

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
