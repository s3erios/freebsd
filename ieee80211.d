#!/usr/sbin/dtrace -s


fbt:kernel:ieee80211*:entry,
fbt::r88e*:entry,
fbt::r92c*:entry,
fbt::rtwn*:entry
{
	stack(10);
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
