Project Name: MultipleHeaps
Project Type: VDK
Hardware Used: None Required

Overview
--------
This example provides a simple case of setting up multiple heaps to be used 
in VDK projects.

The project consists of three threads: ThreadA, ThreadB and the Idle thread.
The kernel tab shows that ThreadA and the Idle thread are allocated 
in the system_heap. ThreadB will allocate its stack and context on a 
new heap called user_heap. The ID of the user_heap heap must be 8 characters
long and be the same as the one specified in heaptab.asm

To construct the heaps these steps have to be followed:
- allocate heap memory areas in the LDF (seg_heap and seg_userheap in this 
  example)
- Create an assembly file defining the heap table (like heaptab.asm)
- Define in the LDF any symbols referenced in the assembly file.

For each thread the VDK status window will display the address of its stack.

  It should be seen that ThreadA's stack is in the region of memory 
  0x58000 - 0x5AFFF 
  (seg_heap area in the LDF)

  Similarly, ThreadB's stack should be in the region of memory 
  0x5B000 - 0x5DFFF
  (seg_userheap area in the LDF)

In the example, threadA and threadB yield to each other 10 times.

File Structure:
---------------
The files included in this example are as follows:
 
	+---MultipleHeaps
	    +---Source Files
	    ¦   A.cpp
	    ¦   B.cpp 
	    ¦	heaptab.asm
	    ¦
	    +---Linker Files
	    ¦	MultipleHeaps.ldf
	    ¦
	    +---Header Files
	    ¦   A.h
	    ¦   B.h 
	    ¦
	    +---Kernel Files
	    ¦	MultipleHeaps.vdk
	    ¦	VDK.cpp
	    ¦	VDK.h
	    
* heaptab.asm:		File that defines the heaps available in the system. 

* MultipleHeaps.ldf:	The LDF has been modified to have a new memory section
						for user_heap. It also defines the symbols 
						ldf_userheap_length and ldf_userheap_space required by
						heaptab.asm

						The LDF has also added ___lib_userheap_space to the list
						of symbols to keep in case section elimination is used
						as it is not referenced directly. If this was not done,
						the heap initialisation would fail and the program would
						fall into KernelPanic.

Getting Started
---------------
1) Build the project using the Rebuild All button on the toolbar
2) Load the executable onto the target hardware/simulator
3) Run the example (F5)
4) The status of the example can then be observed with the following mechanisms
   a) Using the VDK Status window
   b) Using the VDK History window. The VDK History window is only available if
      the Fully instrumented libraries are selected in the VDK kernel tab and
      VDK.cpp has been built with debug information. This is not the default
      case in Release configuration.

How to change the example to run in other processors of the same family
-----------------------------------------------------------------------
This is a generic VDK example which can be run in all supported SHARC
processors. To avoid having numerous copies of the same example within the same
VisualDSP++ installation we selected a few processors that would be
representative for a particular family. This example can be modified to run in
other processors in the same family. This section describes the process to
change this example from one processor to another one in the same family.

There is a choice between adding the LDF manually or to use the expert
linker to copy the right LDF in for you.

- Copy the LDF manually
  * Remove the LDF from your project
  * Copy the VDK LDF for the desired target processor from the VisualDSP
    installation (folder $VDSP_INSTALL/21*/ldf) to your project
    folder.
  * Add the LDF copied in the previous step to your project
  * Compare the original LDF from this example to the default VDK LDF for the
    processor that the example was originally released for (located in the
    VDSP installation) and apply the same changes to your LDF.
  * In the project options change the processor to your desired SHARC
    processor

  The example should now build and run in your chosen SHARC processor.
  Note that this procedure works for all processors but we do not recommend
  moving examples from processors that support short word mode to
  processors that don't or vice versa because their build configurations
  are not the same.

- Use the expert linker to copy the LDF
  * Select Tools->Expert Linker->Create LDF
  * Follow the default options until the end of the process.
  * Compare the original LDF from this example to VDK-21469.ldf and apply the
    same changes to your LDF.
  * In the project options change the processor to your desired SHARC
    processor

  The example should now build and run in your chosen SHARC processor.
  Note that this procedure works for all processors but we do not recommend
  moving examples from processors that support short word mode to
  processors that don't or vice versa because their build configurations
  are not the same.

Analog Devices, Inc.
DSP Division
Three Technology Way
Norwood, MA 02062
