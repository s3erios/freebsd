#!/usr/sbin/dtrace -s
# Run with sudo ./readwrite.d -q

fbt::rtwn_pci_read_*:entry
{
	addr = arg1;
}

fbt::rtwn_pci_write_*:entry
{
	addr = arg1;
	val = arg3;
}

fbt::rtwn_pci_read_1:return
{
	printf("%s( %x ) = %x\n", probefunc, addr, arg1 );
}

fbt::rtwn_pci_write_*:return
{
	printf("%s( %x %x )\n", probefunc, addr, val);
}
