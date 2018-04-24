Project Name: IPC_DD_Example
Project Type: VDK
Hardware Used: None Required

Overview
--------
Using the device driver framework, this example gives POSIX style PIPES to
VDK threads. As the project is set up, it can handle 4 uni-directional PIPES.
To add more pipes, a user needs to add more device flags, and change 
kNumChannels in IPC.cpp to the number of new device channels.

File Structure:
---------------
The files included in this example are as follows:
 
	+---IPC_DD_Example
	    +---Source Files
	    ¦   IPC.cpp
	    ¦   thread1.cpp
	    ¦   thread2.cpp
            ¦
	    +---Linker Files
	    ¦	VDK-XXXXX.ldf
	    ¦
	    +---Header Files
	    ¦   IPC.h
	    ¦   thread1.h
	    ¦   thread2.h
	    ¦   
	    +---Kernel Files
	    ¦	IPC_DD.vdk
	    ¦	VDK.cpp
	    ¦	VDK.h

* IPC.cpp:     The device driver is contained in this file.

* thread1.cpp: Thread1 is the source of the pipe named '1'

* thread1.cpp: Thread2 is the destination of the pipe named '1'

	    
Note: For Blackfin and TigerSharc examples the file ExceptionHandler-XXXX.asm
will be added to the Source Files for the project.
	    
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
