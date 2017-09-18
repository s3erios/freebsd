#!/bin/sh

#sudo kldunload if_rtwn_pci
#sudo kldunload if_rtwn_usb
#sudo kldunload rtwn
#sudo kldunload rtwn-rtl8188eefw 

sudo make -C sys/modules/rtwn_pci/ unload
sudo make -C sys/modules/rtwn/ unload
sudo make -C sys/modules/rtwnfw/rtwnrtl8188ee/ unload

sudo make -C sys/modules/rtwnfw/rtwnrtl8188ee/ load
sudo make -C sys/modules/rtwn/ load
sudo make -C sys/modules/rtwn_pci/ load
