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

### Reverse Engineering

Maybe going forward I will reverse-engineer device drivers from Windows using IdaPro. But why when 
the Linux drivers are sufficient as documentation?

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
