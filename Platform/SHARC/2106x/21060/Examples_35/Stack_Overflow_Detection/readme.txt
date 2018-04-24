Project Name: StackOverflowDetection
Project Type: VDK
Hardware Used: None Required

Overview
--------
This example demonstrates the use of an interrupt handler to detect
and report thread stack overflow conditions.

This capability is unique to the SHARC hardware platform, and so this
is not available as a generic facility within VDK.

The SHARC C runtime environment uses circular buffer 7 (i.e.
registers I7, B7 and L7) to implement the C runtime stack, and VDK
context-switches these registers (amongst others) between each
thread's own stack.

Any attempt by thread code to MODIFY the I7 (stack pointer) register
to a location outside the address range defined by B7 (stack base) and
L7 (stack length) will cause the I7 to "wrap around" according to the
normal DAG modulo addressing.  It will also cause the CB7 interrupt
flag to be asserted.

Normally this interrupt is masked (ignored) but when unmasked (as in
this example) it will invoke its service routine, CB7I_Entry.  All the
service routine does here is to Post the kOverflow semaphore and
return. Posting this semaphore will wake up the high-priority 'Waiter'
thread, which will then take some appropriate action to deal with
and/or report the erring thread. 

There are two additional thread types in the system, these are 
'Recurser' and 'NonRecurser'. These thread types are used to simulate
a simple multi threaded application, where one of the threads will 
overflow its stack. When this application is configured, there should 
be many instances of NonRecurser and one instance of Recurser. It is 
then the responsibility of Waiter to determine which thread has blown
its stack.

The Waiter thread needs to run at as high a priority as is practical,
so that it runs as soon as the semaphore is posted.

After printing its message the Waiter destroys the Recurser thread. It
can determine which thread ID the Recuser is by tracing through the 
history buffer and identifying the thread which was running when the
Stack Overflow interrupt was triggered.


File Structure:
---------------
The files included in this example are as follows:
 
	+---StackOverflowDetection
	    +---Source Files
	    ¦   CB7I.cpp
	    ¦   NonRecurser.cpp
	    ¦	Recurser.cpp
	    ¦	Waiter.cpp
	    ¦
	    +---Linker Files
	    ¦	VDK-21XXX.ldf
	    ¦
	    +---Header Files
	    ¦   NonRecurser.h
	    ¦	Recurser.h
	    ¦	Waiter.h
	    ¦
	    +---Kernel Files
	    ¦	so.vdk
	    ¦	VDK.cpp
	    ¦	VDK.h
	    
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
This is a generic VDK example which should run in all supported SHARC
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
