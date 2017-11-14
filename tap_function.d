#!/usr/sbin/dtrace -qs

/*
 * Run with sudo ./tap_function.d -q
 */

dtrace:::BEGIN
{
	self->start = 0;
	printf("Start on\t%s\n", "rtwn_init");
	printf("End on\t%s\n", "r88ee_start_xfers"); 
}

/* fbt::r92c_set_chan:entry */
fbt::rtwn_init:entry
{
	self->start = 1;
	printf("Start on %s\n", probefunc);
	printf("---Beginning of %s stack---\n", probefunc);
	stack(10);
	printf("---End of %s stack---\n", probefunc);
}

/* fbt::r92c_set_chan:return */
fbt::r88ee_start_xfers:return
{
/*	self->rtwn_bb_init = 0; */
	printf("Exit on %s\n", probefunc);
	exit(0);
}

/*
fbt:kernel:ieee80211*:entry,
fbt:kernel:ifioctl:entry,
fbt::ieee802*:entry,
fbt:rtwn::entry,
fbt:if_rtwn_pci::entry /probefunc != "rtwn_pci_read_1" && probefunc != "rtwn_pci_read_2" && probefunc != "rtwn_pci_read_4" && probefunc != "rtwn_pci_write_1" && probefunc != "rtwn_pci_write_2" && probefunc != "rtwn_pci_write_4"/
{
	if(self->start == 1) {
		printf("%s\n", probefunc);
	}
}
*/

fbt::rtwn_pci_read_*:entry
{
	addr = arg1;
}

fbt::rtwn_pci_write_*:entry
{
	if (self->start == 1) {
		addr = arg1;
		val = arg2;
		printf("%s( 0x%x 0x%x ) %a\n", probefunc, addr, val, caller)    ;
	}
}

fbt::rtwn_pci_read_1:return,
fbt::rtwn_pci_read_2:return,
fbt::rtwn_pci_read_4:return
{
	if(self->start == 1) {
		printf("%s( 0x%x ) = 0x%x %a\n", probefunc, addr, args[0], caller);
	}
}
