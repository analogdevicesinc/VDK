Project Name: MP_Framework
Project Type: VDK
Hardware Used: None Required

Overview
--------
This example comprises a pair of (incomplete) multi-processor
messaging applications, which may be used as a starting-point in the
creation of multi-processor applications for ADSP-BF561.

There is a separate project for each core. The projects are connected
using the "Imported Projects" section of the kernel tab so that each
project imports the other. This allows the project on core B to refer
by name to the IDs of boot threads residing on core A, and vice-versa.

To make use of these two projects it is only necessary to add
user-defined thread types and boot threads. The messaging structure
needed to move messages between cores when required is already in
place. The "Dummy" thread type and boot threads are present only to
allow the project to build initially and may be removed once your own
threads have been added.

If message payloads are also to be moved between threads then
marshalled message types will need to be defined in the kernel
tab. However, on ADSP-BF561 it will generally be more efficient to
place payloads in common (L2 or external) memory. This will allow
unmarshalled message types to be used, where the payload address is
transferred between the cores without translation or copying of the
payload contents. Note that care is needed in such an arrangement to
ensure that payload memory blocks are always ultimately freed by the
same core as allocated them.

At runtime, each core runs a separate application (CoreA.dxe and
CoreB.dxe). The two programs communicate using a device driver which
supports the internal memory DMA hardware of ADSP-BF561. VDK uses this
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

File Structure:
---------------
The files included in this example are as follows:

	+---MP_Framework
	    +---CoreA_BF561
	        +---Source Files
	        ¦   CoreBInit.cpp
	        ¦   DummyThread.cpp
	        ¦   EVT_IVG12.asm
	        ¦   ExceptionHandler-BF561.asm
	        ¦   IMDMADevice.cpp
	        ¦   shared_variables.cpp
	        ¦
	        +---Linker Files
	        ¦	VDK-BF561.ldf
	        ¦
	        +---Header Files
	        ¦   CoreBInit.h
	        ¦   DummyThread.h
	        ¦   IMDMADevice.h
	        ¦   
	        +---Kernel Files
	        ¦	CoreA_BF561.vdk
	        ¦	VDK.cpp
	        ¦	VDK.h
	    +---CoreB_BF561
	        +---Source Files
	        ¦   DummyThread.cpp
	        ¦   EVT_IVG12.asm
	        ¦   ExceptionHandler-BF561.asm
	        ¦   IMDMADevice.cpp
	        ¦
	        +---Linker Files
	        ¦	VDK-BF561.ldf
	        ¦
	        +---Header Files
	        ¦   DummyThread.h
	        ¦   IMDMADevice.h
	        ¦   local_shared_symbols.h
	        ¦   
	        +---Kernel Files
	        ¦	CoreB_BF561.vdk
	        ¦	VDK.cpp
	        ¦	VDK.h



Analog Devices, Inc.
DSP Division
Three Technology Way
Norwood, MA 02062

