###
### call with: 
### py.test -s ~/repo/autohome-common/py/test_device_status.py
###

import pytest
import device_status

def test_get_topic():
    print ("\n==== get_topic ====" )
    print ('check that return string includes passed param')
    assert device_status.get_topic('device_name') ==  'v01/rpi/device_name/status/log'

def test_parse_temp():
    print ('passed value is /1000')
    assert device_status.parse_temp(12345) ==  'Temperature=12.3, '

def test_parse_mem():
    input_value = """MemTotal:         492588 kB
MemFree:          419576 kB
MemAvailable:     432732 kB
SwapCached:            0 kB
SwapTotal:        116212 kB
SwapFree:         116212 kB"""
    output_value = "MemTotal=481.0, MemFree=409.7, MemFreePercent=85.2, MemAvailable=422.6, MemAvailablePercent=87.9, SwapTotal=113.5, SwapUsed=0.0"
    print ('input value is converted')
    assert device_status.parse_mem(input_value) ==  output_value