# RTL8188EE Driver

## About

I wrote this to document my own approach, organize my own thoughts, and help any future kernel hackers.
Note: THIS IS STILL A WORK AND PROGRESS AND CURRENTLY DOES NOT WORK!

## Required Readings

* FreeBSD Architecture Handbook, in particular [Chapter 11. PCI Drivers](https://www.freebsd.org/doc/en_US.ISO8859-1/books/arch-handbook/pci.html)
* [Linux Device Drivers, Third Edition](https://lwn.net/Kernel/LDD3/) Explains how Linux drivers and DMA works.
* FreeBSD [ieee80211(9)](https://www.freebsd.org/cgi/man.cgi?query=ieee80211) man page, 802.11 network layer.
* FreeBSD [net80211(9)](https://www.freebsd.org/cgi/man.cgi?query=net80211) man page, standard interface to IEEE 80211 devices.

## How drivers work

Device drivers do nothing more than maintain state, and read/write data to the device. The code must 
provide an interface to the operating system, typically in the form of a struct of function 
pointers. The FreeBSD kernel will call the respective function based on the IEEE 802.11 stack.

I am used to strictly linear or MFC code, so understanding the aforementioned helped me understand 
how driver code worked.

## General Approach

Given that my device is PCI, I copied the sys/dev/rtwn/rtl8192c/ directory, and renamed all file and 
function names. I proceeded to comment all function code, using the C pre-processor headers, such as 
the following:

```c
void
function_name() {
#if 0
    // Previous code used as reference material
#else
    printf("RTL8188EE: Undefined function: %s\n", __func__);
#endif
}
```

Next, I am gradually filling in whatever Undefined function appears in the terminal messages,
function-by-function. I am comparing to see how the Linux driver implements the same objective in rtl8188ee.
If there are any differences, I am carrying over those changes to the FreeBSD driver. Given that the FreeBSD
and Linux stacks and rtwn vs realtek drivers are not a one-to-one match, the code does not perfectly match.
The best way to find the equivalent function is to find the matching 'define' value and observe the
surrounding code. (Overall, I noticed that the FreeBSD functions only do one task, while the Linx functions
do multiple tasks in a single function).

## About the RTL8188EE

The RTL8188ee is a single-band chip. This means it can only do 2.4Ghz, not 5Ghz.
The closest to documentation I was able to locate from the Realtek website was this link:
http://www.realtek.com.tw/products/productsView.aspx?Langid=1&PNid=21&PFid=48&Level=5&Conn=4&ProdID=272
According to Google searches, this is a collection of drivers, amongst which is the RTL8188EE.

## Porting from Linux

Given that I was unable to find any documentation on this device, I reviewed the Linux driver code 
and used it as documentation - I did not copy, as that would violate the GPL.

### Driver Details

### Reading the eeprom

The following is the process of reading the EEPROM/ROM.

#### Power On

Takes place in efuse power switch()

##### Pre-Warmup

```
write_byte(addr = EFUSE_ACCESS=0xcf, 0xCF); // Takes place in 

read_word(addr = SYS_FUNC_EN=0x2).
If the 4th bit (LOADER_CLK_EN) is not set or the 2nd bit (ANA8M) is not set, then set those bits
and write the result back to the SYS_FUNC_EN=0x2 address.

read_word(addr = SYS_CLK=0x0008)
If the BIT(5)/REG_SYS_CLKR is not set OR BIT(1)/ANA8M is not set, set those bits and write it back 

```
##### Power Up Phase

The RTL8188ee card does not need to do the second phase of the bootup


##### Read the Efuse Proces

This is a little complex

Provide this function an uint16_t/u16 offset value. At the initial eeprom value, its 0x0000.

##### read_efuse_byte()

read_efuse_byte process

write's to the EFUSE_CTRL(0x30) + 1 (so 0x31) address with the value (offset & 0x00FF) - This wipes out the higher order byte.

Read EFUSE_CTRL+2 (so 0x32), shift the value by this equation:
((_offset >> 8) & 0x03) | (readbyte & 0xfc)
and write it back to EFUSE_CTRL+2

Read EFUSE_CTRL+3 (0x33), and that value by 0x7f, write it back to EFUSE_CTRL+3

read_dword from EFUSE_CTRL (0x30), store that value in value32.
This read and evaluation loop, explanation below the code:

while(!(((value32 >> 24) & 0xff) & 0x80) && (retry < 10000)) {
	read_word from EFUSE_CTRL
	increment retry++

The value32 >> 24 captures out the highest bit
Wipes away all but the lowest 0x000000ff (which seems redundant to me)
Logic-Ands the result with 0x80, which means extract out the 8th bit.
The entire result is then negated. So if the 8th bit is set, its False.
The second clause means it only happens at most 10000 times.

Not sure why its done this way, instead of just !(value32 & 0x80000000)

Delay by 50 microseconds?
Final read_byte from EFUSE_CTRL, clear out all but the lower 8 bits, set that as the unsigned 8-bit bpuf value.

## Common Commands

### Build Kernel and Modules

```
make buildkernel -j 4 KERNCONF=MYKERNEL NO_CLEAN=1 MODULES_OVERRIDE="rtwn rtwn_pci rtwn_usb"
make reinstallkernel -j 4 KERNCONF=MYKERNEL MODULES_OVERRIDE="rtwn rtwn_pci rtwn_usb"
```

### Clear the dmesg(8) buffer

```
sysctl kern.msgbuf_clear=1
```

## References

* [OpenGrok source browsing suite](http://src.illumos.org/source/) - I used this very extensively 
when browsing FreeBSD and Linux kernel source. Significiantly more efficient than using grep. Thank 
you [illumos](https://www.illumos.org/)!

## TODO

I would love to port my code over to NetBSD, OpenBSD and illumos.
