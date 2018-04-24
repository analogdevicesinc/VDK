Project Name: Heap_Marshalling
Project Type: VDK
Hardware Used: None Required

Overview
--------
An example of VDK's inter-processor messaging, including marshalling of
external payloads using the heap marshalling function, and 
illustrating how messages may be circulated between processors.

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
to sender. This allows an arbitrary number of Consumers to share a single Producer.

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

This example uses payload marshalling to manage the transfer of payloads that
have been allocated from a heap. A Marshalled Message Type is defined in the 
kernel tab with HeapMarshaller as its marshalling function. 
Space is allocated on the default heap of the local processor for each message 
payload.

When a message is sent to a remote thread (on the other processor), the heap
marshalling function on the local proceesor transfers the payload to the 
remote processor (via the Internal Memory DMA) and automatically frees the heap space 
associated with the payload. The heap marshalling function on the remote 
processor allocates memory for the payload from it's local heap and puts 
the contents of the payload into the allocated memory.

	+---Heap_Marshalling
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
	        ¦   ProdCons.h
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
	        ¦   ProdCons.h
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

