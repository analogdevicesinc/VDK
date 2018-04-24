Project Name: Producer_Consumer_MP
Project Type: VDK
Hardware Used: None Required

Overview
--------
An example of VDK's inter-processor messaging, showing most of the API
functions (with the exception of DestroyMessage() and
MessageAvailable()) and illustrating how messages may be circulated
between processors.

This version is similar to the single processor example, but uses the
multiprocessor messaging extensions to allow the producer and consumer
to reside on different cores of the dual-core ADSP-BF561 DSP.

This requires a separate project for each core. The projects are
linked using the "Imported Projects" section of the kernel tab. This
allows the project on core B to refer to the thread ID of the Producer
residing on core A.

At runtime, each core runs a separate application (CoreA.dxe and
CoreB.dxe). The two programs communicate using a device driver which
supports the Internal Memory DMA hardware of ADSP-BF561. VDK uses this
driver to pass messages between the cores.

The Device Driver is called IMDMADevice, and each project has two
instances (boot I/O objects) of this driver called IMDMA0 and IMDMA1,
one for each channel of the internal memory DMA hardware.

Channel 0 is used for transferring messages from core A to core B,
hence IMDMA0 is opened for writing only in the CoreA project and for
reading in the CoreB project. Similarly, channel 1 is used for
transferring messages from core B to core A, and so IMDMA1 is opened
for reading only in the CoreA project and for writing in the CoreB
project.

This example, like the single-processor producer-consumer example, is
based on the Producer-Consumer problem in the book "Operating Systems:
Design and Implementation" by Andrew S Tanenbaum (pages 58 and 70).

This example extends the one given by Tanenbaum in that the Producer does not 
use a hard-coded destination for the Consumer, but rather returns the messages 
to sender. This allows an arbitrary number (two in this example) of Consumers
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

	+---Producer_Consumer_MP
	    +---Producer_BF561
	        +---Source Files
	        ¦   CoreBInit.cpp
	        ¦   EVT_IVG12.asm
	        ¦   ExceptionHandler-BF561.asm
	        ¦   IMDMADevice.cpp
	        ¦   Producer.cpp
	        ¦   shared_variables.cpp
	        ¦
	        +---Linker Files
	        ¦	VDK-BF561.ldf
	        ¦
	        +---Header Files
	        ¦   CoreBInit.h
	        ¦   IMDMADevice.h
	        ¦   Producer.h
	        ¦   
	        +---Kernel Files
	        ¦	Producer_BF561.vdk
	        ¦	VDK.cpp
	        ¦	VDK.h
	    +---Consumer_BF561
	        +---Source Files
	        ¦   Consumer.cpp
	        ¦   EVT_IVG12.asm
	        ¦   ExceptionHandler-BF561.asm
	        ¦   IMDMADevice.cpp
	        ¦
	        +---Linker Files
	        ¦	VDK-BF561.ldf
	        ¦
	        +---Header Files
	        ¦   Consumer.h
	        ¦   IMDMADevice.h
	        ¦   local_shared_symbols.h
	        ¦   
	        +---Kernel Files
	        ¦	Consumer_BF561.vdk
	        ¦	VDK.cpp
	        ¦	VDK.h
	        
Getting Started
---------------
1) Build boths projects using the Rebuild All button on the toolbar
2) Load each executable their respective cores in the target hardware/simulator
   Ensure that when loading the resulting executables: 
     Producer_BF561 is loaded into core P0
     Consumer_BF561 is loaded into core P1
3) Run both executables



Analog Devices, Inc.
DSP Division
Three Technology Way
Norwood, MA 02062
