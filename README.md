What is opensn0w?
=================

opensn0w is a portable utility designed for the easy injection of kernel 
and boot loader patches along with exploit injection. Currently, we
remove the signature checks in iBEC and iBSS (both of which are boot
loaders) and signature and sandboxing checks in the kernel.

Why opensn0w?
=============

opensn0w is a portable and open source jailbreaking utility. opensn0w
has been compiled on both little endian and big endian systems. These
include the Intel i386/x86_64 processor architecture and the Cell
Broadband Engine in the PlayStation 3. We have successfully booted an
iPod touch 4th generation running iOS 5.0.1 on both of these platforms.

How does opensn0w work?
=======================

opensn0w first connects to the target device in DFU (device firmware
update) mode. We then detect which processor is in the target system,
such as the Samsung S5L8930XSI (Apple A4) and then the appropriate
exploit is injected and then we upload our patched iBSS which loads
a patched iBEC which will load our owned kernel. On the
s5l8930/8922/8920 we use the limera1n exploit made by geohot
to own DFU signature checks. Support for the s5l8720 and the
s5l8900 has not yet been implemented.

Why not just use redsn0w? It has a graphical user interface!
============================================================

opensn0w is open source. redsn0w is not. opensn0w will work 
on the most arcane platforms such as a Linux based toaster 
with libusb and a EHCI controller. redsn0w will not.

But copyright! Are you using other peoples' code?
=================================================

Of course, we honor the GPL, and all our code is open.
We used code from xpwn, written by planetbeing and the iPhone
Dev Team and also libirecovery and syringe from the Chronic
Dev Team. Their code is their property, we just modified
it to fit with our project's needs.

It's too complicated to use at the moment!
==========================================

We're working on the actual project as quick as we can. We 
have lives too. :).

Just make sure to install development packages for:
  - readline
  - zlib
  - libssl (openssl)
  - libcurl
  - libusb-1.0
  - libpng

And punch in make, and you're done!
