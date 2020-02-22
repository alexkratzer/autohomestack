"""
get status of linux os and hardware
results will be puplished at MQTT for further processing

use python3, if using python replace subprocess with commands


copy this script to clients:
scp ~/repo/autohome-common/py/device_status.py rpi:~
* get logfile from rpi
* scp rpi:~/device_status.log ~/Desktop/LOG_device_status.log
* deprecated scp ~/repo/autohome-common/py/device_status.py tc@192.168.1.39:~ #lost after reboot

current clients:
192.168.1.39    og_floor    piCorePlayer
192.168.1.40    eg_living   piCorePlayer
192.168.1.41    og_bath     piCorePlayer
192.168.1.211   rpi4        node-red + other container
192.168.1.200   syn         NAS and home automation server

crontab -e

crontab piCorePlayer
*/10 * * * * /usr/local/bin/python /home/tc/device_status.py

crontab rpi4
*/10 * * * * /usr/bin/python /home/pi/device_status.py

"""

import time
import subprocess
import paho.mqtt.client as paho

DEBUG = False #False -> result is puplished at MQTT

ListDevices = [['og_floor','192.168.1.39'],['eg_living','192.168.1.40'],['og_bath','192.168.1.41']]

# TODO: add more status cmds
# https://thebetterparent.com/2019/02/raspberry-pi-terminal-commands-a-quick-guide-for-raspberry-pi-users/

CMD_TEMP = 'cat /sys/devices/virtual/thermal/thermal_zone0/temp'
CMD_MEM = "cat /proc/meminfo | egrep \'Mem|Swap\'"

broker="192.168.1.211"
port=1883

####################################################### CODE ###################################################

def get_topic(device_name):
    return 'v01/rpi/'+ device_name +'/status/log'

def parse_temp(val):
    return "Temperature={}, ".format(str(round(int(val) / 1000,1)))

def parse_loadavg(val):
    sp = val.split(' ')
    return f"loadavg_1m={sp[0]}, loadavg_5min={sp[1]}, loadavg_15min={sp[2]}, la?={sp[3]}, la??={sp[4]}, "

def parse_mem(val):
    #print(f'___TEST: input val {val}')
    val_line = val.split('\n')

    for val in val_line:
        key, value = val.split(':')
        #print('key=' + key.strip() + ', value=' + value.strip())
        if key=='MemTotal':
            MemTotalLine = value.strip().split(' ')
            MemTotalMBInt = round(int(MemTotalLine[0])/1024,1)
            Mem_multipl = 100/MemTotalMBInt
            #print('MemTotal: ' + MemTotalLine[0], ' MemTotalMB: ' + str(MemTotalMBInt), ' Mem_multipl: ' + str(Mem_multipl))

        elif key=='MemFree':
            MemFreeLine = value.strip().split(' ')
            MemFreelMBInt = round(int(MemFreeLine[0])/1024,1)
            MemFreePercent = round(Mem_multipl * MemFreelMBInt,1)
            #print('MemFree: ' + MemFreeLine[0], ' MemFreeMB: ' + str(MemFreelMBInt) + ' percent: ' + str(MemFreePercent))

        elif key=='MemAvailable':
            MemAvailableLine = value.strip().split(' ')
            MemAvailableMBInt = round(int(MemAvailableLine[0])/1024,1)
            MemAvailablePercent = round(Mem_multipl * MemAvailableMBInt,1)
            #print('MemAvailable: ' + MemAvailableLine[0], ' MemAvailableMB: ' + str(MemAvailableMBInt) + ' percent: ' + str(MemAvailablePercent))

        elif key=='SwapTotal':
            SwapTotalLine = value.strip().split(' ')
            SwapTotalMBInt = round(int(SwapTotalLine[0])/1024,1)

        elif key=='SwapFree':
            SwapFreeLine = value.strip().split(' ')
            SwapFreeMBInt = round(int(SwapFreeLine[0])/1024,1)
            SwapUsed = SwapTotalMBInt - SwapFreeMBInt

    #retval = f'MemTotal={MemTotalMBInt}, MemFree={MemFreelMBInt}, MemFreePercent={MemFreePercent}, MemAvailable={MemAvailableMBInt}, MemAvailablePercent={MemAvailablePercent}, SwapTotal={SwapTotalMBInt}, SwapUsed={SwapUsed}'
    #print(f'TEST: return val {retval}')
    return f'MemTotal={MemTotalMBInt}, MemFree={MemFreelMBInt}, MemFreePercent={MemFreePercent}, MemAvailable={MemAvailableMBInt}, MemAvailablePercent={MemAvailablePercent}, SwapTotal={SwapTotalMBInt}, SwapUsed={SwapUsed}'
    #return 'MemTotal=' + str(MemTotalMBInt) + ', MemFree=' + str(MemFreelMBInt) + ', MemFreePercent=' + str(MemFreePercent) + ', MemAvailable=' + str(MemAvailableMBInt) + ', MemAvailablePercent=' + str(MemAvailablePercent) + ', SwapTotal=' + str(SwapTotalMBInt) + ', SwapUsed=' + str(SwapUsed)

def parse_uptime(val):
    #remove all , since they are used for splitting at node red
    return 'uptime=' + val.replace(',' , ';') + ', '

#def static_info():
    # list_files = subprocess.getoutput(["ls", "-l"])
#    pri_log('############### get static_info info #################')
#    pri_log('cat /proc/cpuinfo')
#    pri_log(subprocess.getoutput('cat /proc/cpuinfo | grep model'))
#    pri_log('lscpu -  Shows CPU Architecture Info')
#    pri_log(subprocess.getoutput('lscpu | egrep \'Arch|Byte|Vendor|Model|CPU\'  '))
#    pri_log(f'local IP address {subprocess.getoutput("hostname -I")}')
#   r2 = subprocess.getoutput('df -h')
#    return subprocess.getoutput('cat /proc/cpuinfo | grep model') + "\n" + subprocess.getoutput('lscpu | egrep \'Arch|Byte|Vendor|Model|CPU\' ')

def device_info():
    ret_tmp = subprocess.getoutput(CMD_TEMP)   
    ret_mem = subprocess.getoutput(CMD_MEM)
    ret_uptime = subprocess.getoutput('uptime')
   
    return parse_temp(ret_tmp) + parse_uptime(ret_uptime) + parse_mem(ret_mem)

def syn_info():
    ret_loadavg = subprocess.getoutput("cat /proc/loadavg")
    ret_mem = subprocess.getoutput(CMD_MEM)
        
    return parse_loadavg(ret_loadavg) + parse_mem(ret_mem)  
    

def get_remote_status(addresse):
    try:
        ret_tmp = subprocess.getoutput('ssh tc@'+addresse+' ' +CMD_TEMP)       
        ret_mem = subprocess.getoutput('ssh tc@'+addresse+' '+CMD_MEM)
        ret_uptime = subprocess.getoutput('ssh tc@'+addresse+ ' uptime')

        return parse_temp(ret_tmp) + parse_uptime(ret_uptime) + parse_mem(ret_mem)

    except Exception as e:
        pri_log('get_remote_status except: {}'.format(str(e)))
        return str(e)


#def on_publish(client,userdata,result):             #create function for callback
#    pri_log("data published client={}, userdata={}, result={}".format(client,userdata,result))

def pup(topic, msg):
    if not DEBUG:
        client1= paho.Client("control1")    
        #client1.on_publish = on_publish                          #assign function to callback
        client1.connect(broker,port)                                 #establish connection
        ret= client1.publish(topic, msg)                   #publish
        pri_log('retval from mqtt puplish: {}'.format(ret))
        client1.disconnect()
    else:
        pri_log(f'NO puplish [topic:{topic} msg:{msg}]')


def pri_log(msg):
    if DEBUG:
        print(msg + "\n")
    else:
        file = open("device_status.log","a") 
        file.write(msg) 
        file.close() 

def main():
    pri_log(f'##################### device_status [DEBUG={DEBUG}] ############################\n')

    # get status of host device
    pri_log('get status of host')
    pup(get_topic('main_node'), device_info())    

    # get status of remote device
    for key, ip in ListDevices:
        pri_log(f'get status of {key} / {ip}')
        pup(get_topic(key), get_remote_status(ip))

    pri_log('get status of syn')
    pup(get_topic('syn'), syn_info())


if __name__ == "__main__":
    main()
