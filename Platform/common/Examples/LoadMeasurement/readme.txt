Project Name: LoadMeasurement
Project Type: VDK
Hardware Used: None Required

Overview
--------
A common requirement of many applications is the utility to monitor the work
load that the application is under over a period of time. The VDK History
Window (accessed through View>VDK Windows>History) will provide a graph of the
target load for this purpose. However the manner in which these figures are
calculated has some limitations, as it will merely measure the amount of time
the application has spent in the idle thread (i.e. doing nothing). Then uses
this figure as the basis for the target load calculation.

This example aims to present an alternative scheme for measuring the application
load. The advantage of the scheme used in this example is that instead of 
simply measuring the number of ticks an application spends in the Idle thread,
this method measures the amount of work done by a user written idle thread
(essentially just a lowest priority thread) and compares this to the amount of
work achieved when only this thread is running.

As a result of this implementation the Target Load figures in the VDK History
window will no longer be displayed correctly.

File Structure:
---------------
The files included in this example are as follows:
 
	+---LoadMeasurement
	    +---Source Files
	    ¦   InitThread.cpp
	    ¦   UserIdle.cpp 
	    ¦	Worker.cpp
	    ¦
	    +---Linker Files
	    ¦	VDK-XXXXX.ldf
	    ¦
	    +---Header Files
	    ¦   InitThread.h
	    ¦   UserIdle.h 
	    ¦	Worker.h
	    ¦
	    +---Kernel Files
	    ¦	LoadMeasurement.vdk
	    ¦	VDK.cpp
	    ¦	VDK.h
	    
This example contains three user defined thread types. These are as follows:

* UserIdle:   A user defined replacement for the VDK Idle thread. This thread
              will continually increment a global variable to provide a measure
              of 'work done'.

* InitThread: Sets up the two distinct stages of the example. Also monitors the
              value of the 'work done' global variable at regular periods of
              time and uses this to determine and report the application load.
              
* Worker:     Simulates an application that has a varying amount of workload.
              There can be many instances of this thread type executing
              concurrently.

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

This example is designed to run in two stages. The first of these is a
calibration stage in which the only thread permitted to run (besides the
InitThread, which must run for monitoring purposes) is the User Idle Thread. The
objective of this stage is to determine how much work is done when the idle
thread has 100% of the execution time.

In the second stage of the example the full application is allowed to execute.
The User Idle Thread executes in the same way as before with the 'work done'
global variable being sampled at the same period as in the previous stage. This
time the amount of work done each period is used, in conjunction with the 100%
idle 'work done' value determined in the calibration stage, to calculate the
application load as a percentage value.
    
Notes:
------
1) The worker threads in this application do not do any actual work. As the
purpose of this example is to demonstrate the alternative load measurement
scheme then the underlying example is not of great importance. With this in mind
the aim of the Worker thread is to produce an application which has a varying
amount of work load over a period of time.

2) If you wish to apply this load measurement scheme to an application, then
you should note that if your application destroys any threads with the Destroy
Now parameter set to 'false'. Then the User Idle thread will also have to
contain a call to the FreeDestroyedThreads() function.



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
