Description:
------------

This example comprises a pair of (incomplete) multi-processor
messaging applications, which may be used as a starting-point in the
creation of multi-processor applications for ADSP-21160.

Each executable reside on different DSPs of (in this case) a pair of 
ADSP-21160 EZ-Kit Lite boards. The two DSP are arbitrarily identified as core A and core B.

There is a separate project for each core. The projects are connected
using the "Imported Projects" section of the kernel tab so that each
project imports the other. This allows the project on core B to refer
by name to the IDs of boot threads residing on core A, and vice-versa.

In addition to the two EZ-Kit boards, running this example will also
require a pair of linkport patch cables, a separate ICE emulator for
each board (e.g. one HPPCI and one HPUSB) and two VisualDSP++
sessions.

To make use of these two projects it is only necessary to add
user-defined thread types and boot threads. The messaging structure
needed to move messages between cores when required is already in
place. The "Dummy" thread type and boot threads are present only to
allow the project to build initially and may be removed once your own
threads have been added.

If message payloads are also to be moved between threads then
marshalled message types will need to be defined in the kernel
tab.

At runtime, each core runs a separate application (CoreA.dxe and
CoreB.dxe). The two programs communicate using a device driver which
supports the linkport and DMA hardware of ADSP-21160. VDK uses this
driver to pass messages between the cores.


The 21160 linkports are used as simplex connections, such that
messages only flow in one direction. Achieving full connectivity
between the two DSPs therefore requires the use of two linkports on
each board.

The Device Driver is called LinkPort, and each project has six
instances (boot I/O objects) of this driver called Link0, Link1,
Link2, Link3, Link4, and Link5, one for each link buffer. Only Link0
and Link4 are actually used in the example, however.

The ports used in the example are linkports 0 and 4 on each board, as
these are brought out to external connectors on the 21160 EZ-Kit Lite
and hence can be connected between the boards using patch cables. The
cables should connect linkport 0 on each board to linkport 4 of the
other. Both projects use the linkport 0 device for outgoing messages
and linkport 4 for incoming.

The DSP's default linkport-to-linkbuffer mapping is used, so that
buffer 0 corresponds to port 0 and buffer 4 to port 4.

Files:
------

readme.txt
	This file.
MP_Framework.dpg
	Project group that contains CoreA and CoreB projects.

ProdCons.h
	A header file containing declarations of constants and enumerations which 
	are required by both thread classes.

CoreA/DummyThread.cpp
CoreA/DummyThread.h
	A thread that acts as a random-number server, filling the payload of each 
	incoming empty message with a random number and returning it to its sender.
CoreA/LP0I_ISR.asm
CoreA/LP1I_ISR.asm
CoreA/LP2I_ISR.asm
CoreA/LP3I_ISR.asm
CoreA/LP4I_ISR.asm
CoreA/LP5I_ISR.asm
CoreA/LinkPort.cpp
CoreA/LinkPort.h
	Empty thread provided to allow project to compile.
CoreA/CoreA_21160.dpj
CoreA/CoreA_21160.vdk
	Project files for core A.

CoreB/DummyThread.cpp
CoreB/DummyThread.h
	Empty thread provided to allow project to compile.
CoreB/LP0I_ISR.asm
CoreB/LP1I_ISR.asm
CoreB/LP2I_ISR.asm
CoreB/LP3I_ISR.asm
CoreB/LP4I_ISR.asm
CoreB/LP5I_ISR.asm
CoreB/LinkPort.cpp
CoreB/LinkPort.h
	Inter-DSP communication device driver using linkport and DMA.
	Identical to the code for CoreA
CoreB/CoreB_21160.dpj
CoreB/CoreB_21160.vdk
	Project files for core B.

