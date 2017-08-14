#!/bin/sh

sudo ifconfig wlan0 create wlandev rtwn0 wlanmode monitor channel 1:g
sudo ifconfig wlan0 up
#sudo ifconfig wlan0 1.1.1.1
