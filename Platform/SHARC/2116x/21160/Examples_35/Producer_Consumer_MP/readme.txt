Description:
------------

An example of VDK's inter-processor messaging, showing most of the API
functions (with the exception of DestroyMessage() and
MessageAvailable()) and illustrating how messages may be circulated
between processors.

This version is similar to the single processor example, but uses the
multiprocessor messaging extensions to allow the producer and consumer
to reside on different DSPs of (in this case) a pair of ADSP-21160
EZ-Kit Lite boards. The two DSP are arbitrarily identified as core A
and core B.

This requires a separate project for each core. The projects are
linked using the "Imported Projects" section of the kernel tab. This
allows the project on core B to refer to the thread ID of the Producer
residing on core A.

In addition to the two EZ-Kit boards, running this example will also
require a pair of linkport patch cables, a separate ICE emulator for
each board (e.g. one HPPCI and one HPUSB) and two VisualDSP++
sessions.

At runtime, each core runs a separate application (Producer_21160.dxe
and Consumer_21160.dxe). The two programs communicate using a device
driver which supports the linkport and DMA hardware of ADSP-21160. VDK
uses this driver to pass messages between the cores.

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

This example, like the single-processor producer-consumer example, is
based on the Producer-Consumer problem in the book "Operating Systems:
Design and Implementation" by Andrew S Tanenbaum (pages 58 and 70).

This example extends the one given by Tanenbaum in that the Producer does not 
use a hard-coded destination for the Consumer, but rather returns the messages 
to sender. This allows an arbitrary number (one in this example) of Consumers
to share a single Producer.

As provided, the Producer and Consumer threads share the same priority level, 
but the algorithm will continue to work with a Producer either of higher or 
lower priority to the Consumers.

The number of Consumers in the system may also be changed, provided that the 
"Maximum messages" setting (in the kernel tab) is greater than or equal to N 
times the number of Consumers (N is defined in Consumer.cpp, and is presently 
set to 10).

If the Consumer thread type is added into the core A project, then
Consumers can exist on either core, i.e. both local to, and remote
from, the Producer.

Files:
------

readme.txt
	This file.
Producer_Consumer_MP.dpg
	Project group that contains CoreA and CoreB projects.

ProdCons.h
	A header file containing declarations of constants and enumerations which 
	are required by both thread classes.

CoreA/Producer.cpp
CoreA/Producer.h
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
	Inter-DSP communication device driver using linkport and DMA.
CoreA/Producer_21160.dpj
CoreA/Producer_21160.vdk
	Project files for core A.

CoreB/Consumer.cpp
CoreB/Consumer.h
	A thread which fetches random numbers from the producer and prints them.
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
CoreB/Consumer_21160.dpj
CoreB/Consumer_21160.vdk
	Project files for core B.

