#!/usr/sbin/dtrace -qs

/* Run with sudo ./readwrite.d -q */

fbt::rtwn_pci_read_*:entry
{
	addr = arg1;
}

fbt::rtwn_pci_write_*:entry
{
	addr = arg1;
	val = arg2;
	printf("%s( 0x%x 0x%x ) %a\n", probefunc, addr, val, caller);

}

fbt::rtwn_pci_read_1:return
{
	printf("%s( 0x%x ) = 0x%x %a\n", probefunc, addr, args[0], caller);
}

/* Exit */
fbt::r92ce_start_xfers:entry
{
	exit(0);
}
