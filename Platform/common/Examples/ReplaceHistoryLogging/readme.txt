Project Name: ReplaceHistoryLogging
Project Type: VDK
Hardware Used: None Required

Overview
--------
This example demonstrates how the history logging subroutine can be replaced
with a custom version provided by the user. In this example the default history
logging mechanism is replaced with a new version which logs history events 
against the cycle count on which they occur instead of the Tick count.

This example contains only one thread type, its purpose is to provide the
history logging subroutine with some history events to log yielding to another 
thread a number of times. When the thread has done this, print a message and
pend on a semaphore that never gets posted.

File Structure:
---------------
The files included in this example are as follows:
 
	+---ReplaceHistoryLogging
	    +---Source Files
	    ¦   UserThread.cpp
	    ¦	historyLog.asm
	    ¦	main.c
	    ¦
	    +---Linker Files
	    ¦	VDK-XXXXX.ldf
	    ¦
	    +---Header Files
	    ¦   UserThread.h
	    ¦
	    +---Kernel Files
	    ¦	ReplaceHistoryLogging.vdk
	    ¦	VDK.cpp
	    ¦	VDK.h

Note: For Blackfin and TigerSharc examples the file ExceptionHandler-XXXX.asm
will be added to the Source Files for the project.	    
	    
* historyLog.asm:	File that contains the modified version of the history
					logging subroutine, it replaces the Tick value passed to the
					routine with the current cycle count, so that any history 
					events logged are measured against cycles instead of Ticks.
                                                                                
* main.c:			A replacement main() function, contains the call to replace
					the history subroutine, this is done before any VDK 
					Initialization to ensure that all events, including any that
					take place during VDK's start-up procedure, are logged using
					the modified history logging subroutine.
					Replacing main is not required to replace the history 
					mechanism but ensures that no events are logged before the 
					replacement is done.

Getting Started
---------------
1) Build the project using the Rebuild All button on the toolbar
2) Load the executable onto the target hardware/simulator
3) Run the example (F5)
4) The progress of the example can now be viewed using the VDK History window,
the events logged here should now be measured in cycles instead of Ticks. 

Note: 	The history window will still display the unit of measurement as 'Ticks'
		despite the actual measurement being cycles. Because the argument is no
		longer ticks, the IDDE will not detect which thread is running before the
		run function of the first boot thread is reached.

Note(2):The VDK History window is only available if the Fully instrumented 
        libraries are selected in the VDK kernel tab and VDK.cpp has been built
        with debug information. This is not the default case in Release
        configuration.

How to change the example to run in other processors of the same family
-----------------------------------------------------------------------
This is a generic VDK example which should run in all supported processors. To
avoid having numerous copies of the same example within the same
VisualDSP++ installation we selected a few processors that would be
representative for a particular family. This example can be modified to run in
other processors in the same family. This section describes the process to
change this example from one processor to another one in the same family.

    Blackfin processors:
    -------------------
    There is a choice between adding a generated LDF or to have a custom LDF in
    your project (the default).

    - Generated LDF. All the following steps are in the Project Options menu.
      * Change the processor to your desired Blackfin processor 
      * Select Add StartUp Code/LDF 
      * Select Add an LDF and startup code and click OK

      The example should now build and run in your chosen Blackfin processor.
      Note that you will still have in your project the exception handler's
      file name references a different blackfin processor. This is not a
      problem as the contents of this file are the same for all blackfin
      processors other than BF535. 

    - Non-generated LDF
      * Remove the LDF from your project
      * Copy the VDK LDF for the desired target processor from the VisualDSP
        installation (folder $VDSP_INSTALL/Blackfin/ldf) to your project
        folder.
      * Add the LDF copied in the previous step to your project
      * In the project options change the processor to your desired Blackfin
        processor

      The example should now build and run in your chosen Blackfin processor.
      Note that you will still have in your project the exception handler's
      file name references a different blackfin processor. This is not a
      problem as the contents of this file are the same for all blackfin
      processors other than BF535. 

    Sharc processors: 
    ----------------
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

    TigerSHARC processors:
    ---------------------
    This mechanism is only recommended to change between the different
    processors in the TS20x family. 
                                                              
      * Remove the LDF from your project
      * Copy the VDK LDF for the desired target processor from the VisualDSP
        installation (folder $VDSP_INSTALL/TS/ldf) to your project
        folder.
      * Add the LDF copied in the previous step to your project
      * In the project options change the processor to your desired TS20x
        processor

      The example should now build and run in your chosen TS20x processor.
      Note that you will still have in your project the exception handler's
      file name references a different TigerSHARC processor. This is not a
      problem as the contents of this file are the same for all TS20x
      processors.


Analog Devices, Inc.
DSP Division
Three Technology Way
Norwood, MA 02062
