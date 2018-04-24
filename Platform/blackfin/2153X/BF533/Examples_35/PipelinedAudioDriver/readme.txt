ADSP-BF533 Pipelined Audio Driver Example
-----------------------------------------

Description:
------------

An Audio Driver Example for the ADSP-BF533 EZ-Kit Lite, using the AD1836A codec.

This example illustrates:

- the construction of a "pipelined" device driver to provide an
  efficient, double-buffered interface to the AD1836A codec.

- the use of messaging to efficiently pass audio data between threads.


The device driver support both 2-channel (stereo) and 4-channel
("quadraphonic") input and output at 48kHz with a 16-bit sample
size. It does not yet support the higher-resolution (96KHz/24-bit)
modes of the AD1836, nor does it allow access to the third pair of
output channels.

The driver communicates with the codec using SPort0 in I2S mode, which
requires the ADSP-BF533 EZ-KIT Lite to be set up as follows:

- Turn pins 5 and 6 of SW9 ON, these will connect RSCLK0 -> TSCLK0 and
  RFS0 -> TFS0.

SW9 is the red 6-pin switch block found close to the centre of the
upper side of the board.


The driver provides full duplex operation, so that audio input and
output may occur simultaneously, and allows "split-open" operation
where one thread (an audio input thread) opens the device for reading
and a different (audio output) thread opens it for writing. This is
the basis of the audio-handling technique used in this example.


Pipelined I/O:
--------------

In the input/output APIs typically provided by operating systems, the
default behaviour is for the read and write functions (called
SyncRead() and SuncWrite() in VDK) to block until input to, or output
from, a specified buffer is complete.

This model has drawbacks for streaming I/O, as the buffer is only
available to the device driver while the API call is blocked. If the
stream of data to or from the device is to continue unbroken when the
read or write function returns then the driver must provide internal
buffering of data. This complicates the design of the driver, reduces
its efficiency (due to the need to shuffle data between internal and
user buffers) and introduces the question of "how much buffering is enough?".

The crux of the problem is that there can only be 0 or 1 buffers
waiting at any time - 1 if the reading/writing thread is blocked and 0
if itsn't - so so that while the thread is nonblocked there is no
buffer avaiable for I/O. Pipelining allows there to be 0, 1 or 2
buffers waiting, according to circumstance:

0 - device is idle

1 - a buffer has been handed-over to the driver by a read or write
    call but the calling thread has not been blocked, I/O to/from this
    buffer continues asynchronously while the thread continues execution.

2 - another read or write call has been made to the device before I/O on
    the previous buffer has completed. The new buffer is queued and the
    calling thread is blocked until the old buffer has been completed and
    the new one swapped in.

(Each read or write call returns the number of elements transferred by
the *previous* operation, hence the term "pipelined". If there was no
previous operation then this will be zero.)

If the calling thread is able to ensure that it always queues the next
buffer before the previous one completes then it can achieve
uninterrupted I/O to or from the device. The size of the buffer
determines how much time is available for the thread to get the next
buffer ready.

The main loop of the calling thread (which can be seen in the Input
and Output thread types in the example project) is only a little more
complex than than for using a non-pipelined device. Similarly, the
code for the device driver itself (AD1836 in this example) is only
moderately more involved than that for a simple non-pipelined driver
and is probably less complex (as well as more efficient) than a driver
which depends on internal buffering.


Message loopback:
-----------------

The example application uses the technique of "message loopback",
whereby messages are returned to their point of origin for reuse,
rather than being destroyed. All messages used in the system are
created (by the Input thread) at startup. This method has a number of
advantages:

- It is more efficient (in runtime cycles) than constantly destroying
  and creating messages.

- Failure of message creation (i.e. due to pool exhaustion) cannot
  occur during steady-state operation.

- The loopback path regulates the flow of messages, preventing the sender
  sender from issuing messages at a faster rate than they can be consumed.

- Thread scheduling is driven by data flow, rather than priority.


Files:
------

readme.txt
	This file.

AD1836.h
AD1836.cpp
EVT_IVG9.asm
Talkthrough.h
	The pipelined device driver for the AD1836A codec.

AudioMsg.h
	A header file containing declarations of constants and
	enumerations which are required by all thread classes.

Input.h
Input.cpp
	A thread class which reads in audio data via the AD1836 device
	driver and sends it to the a Relay thread as a stream of
	messages.

Relay.h
Relay.cpp
	A thread class which examines an audio message stream to
	determine the peak signal level (of 1 in every 100 messages)
	and uses this value to set the LEDs, using them as a bar level
	meter. Each message received is forwarded to the Input thread
	after processing.

Output.h
Output.cpp
	A thread class which receives messages containing audio data
	and writes them to the AD1836 device driver.

PipelinedAudioDriver.dpj
PipelinedAudioDriver.vdk
	VisualDSP++ project files for this project.

ExceptionHandler-BF533.asm
	User exception handler.


Operation description:
----------------------

- Open the project "PipelinedAudioDriver.dpj" in an ADSP-BF533 EZ-KIT
  Lite session in the VisualDSP Integrated Development Environment (IDDE).

- Under the "Project" tab, select "Build Project" (program is then
  loaded automatically into DSP).

- Turn pin 5 and 6 of SW9 on the ADSP-BF533 EZ-KIT Lite ON, these will
  connect RSCLK0 -> TSCLK0 and RFS0 -> TFS0.

- Setup an input source (such as a radio) to the audio in RCA
  connectors and an output source (such as headphones or powered
  speakers) to the audio out RCA connectors of the EZ-KIT Lite.  See
  the ADSP-BF533 EZ-KIT Lite User's Manual for more information on
  setting up the hardware.

  The following ascii-art represents an edge-on view of the BF533 EZ-Kit
  Lite baord, showing the locations of the RCA audio connectors as (0) :

     (0) (0) (0)            (0) (0) (0)   (0) (0)   Left  channel
     (0) (0) (0)            (0) (0) (0)   (0) (0)   Right channel
    ----------------------------------------------

          ^                  ^   ^   ^     ^   ^
          |                  |   |   |     |   |
          |                 Out Out Out   Inp Inp
          |                  3   2   1     1   2

     Video Connectors         Audio Connectors

  As supplied, the example project runs in stereo mode, taking input
  on "Inp 1" and passing it through to "Out 1". The other inputs and
  outputs are unused.


- Select "Run" from the "Debug" tab on the menu bar of VisualDSP.

- Listen to the operation of the talk-through and observe the behaviour of the LEDs.


