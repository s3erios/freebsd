#!/bin/sh

sudo ifconfig wlan0 create wlandev rtwn0 wlanmode monitor channel 6:g
#sudo ifconfig wlan0 create wlandev rtwn0 channel 6 ssid test111
sudo ifconfig wlan0 up
#sudo ifconfig wlan0 1.1.1.1
