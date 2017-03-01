#!/bin/sh

sudo sh -c "echo > /var/log/messages"
sudo kldunload if_rtwn_pci
sudo make buildkernel -j 4 KERNCONF=GENERIC NO_CLEAN=1 MODULES_OVERRIDE="rtwn rtwn_pci rtwn" && sudo make reinstallkernel -j 4 KERNCONF=GENERIC MODULES_OVERRIDE="rtwn rtwn_pci rtwn"
sudo sysctl kern.msgbuf_clear=1
sudo kldload if_rtwn_pci
