#!/bin/sh

sudo sh -c "echo > /var/log/messages"
sudo kldunload if_rtwn_pci
sudo kldunload if_rtwn
sudo kldunload rtwn-rtl8188eefw
sudo make buildkernel -j 4 KERNCONF=GENERIC NO_CLEAN=1 MODULES_OVERRIDE="rtwn rtwn_pci rtwn" && sudo make reinstallkernel -j 4 KERNCONF=GENERIC MODULES_OVERRIDE="rtwn rtwn_pci"
#sudo sysctl kern.msgbuf_clear=1
sudo kldload if_rtwn
sudo kldload if_rtwn_pci
#sudo kldload rtwn-rtl8188eefw
#sudo sh -c "dmesg > /tank/shared/dmesg.freebsd"
#sudo ifconfig wlan0 create wlandev rtwn0
#sudo ifconfig wlan0 1.1.1.1
