#!/usr/sbin/dtrace -s

dtrace:::BEGIN {
	self->start = 0;
}

fbt:if_rtwn_pci:r88ee_start_xfers:entry {
	self->start = 0;
}

fbt:kernel:ieee80211*:entry,
fbt:kernel:ifioctl:entry,
fbt::ieee802*:entry,
fbt:rtwn::entry,
fbt:if_rtwn_pci::entry /probefunc != "rtwn_pci_read_1" && probefunc != "rtwn_pci_read_2" && probefunc != "rtwn_pci_read_4" && probefunc != "rtwn_pci_write_1" && probefunc != "rtwn_pci_write_2" && probefunc != "rtwn_pci_write_4" && probefunc != "ieee80211_get_counter"/
{
	printf("%s\n", probefunc);
}

/*
syscall::read:entry
{
	self->follow = 1;
}

syscall::read:entry
/self->follow/
{}
*/
