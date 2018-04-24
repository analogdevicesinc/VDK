ADSP-BF537 Pipelined Audio Driver Example
-----------------------------------------

Description:
------------

An Audio Driver Example for the ADSP-BF537 EZ-Kit Lite, using the
AD1854 DAC and AD1871 ADC.

This example illustrates:

- the construction of a "pipelined" device driver to provide an
  efficient, double-buffered interface to the ADC and DAC.

- the use of messaging to efficiently pass audio data between threads.

There are two VDK device drivers, AD1871 and AD1854, they support
2-channel (stereo) input and output, respectively, at 48kHz with 8-,
16-, or 24-bit sample sizes. They do not support any functionality
(e.g. volume control) which would require access to the internal
registers of the ADC and DAC, as this is not permitted by the circuit
configuration of the EZ-Kit board.


Serial Port Operation:
----------------------

The drivers communicate with the ADC and DAC using SPort0 in I2S
mode. By default, the AD1871 ADC acts as clock master for both Sport0
Rx and TX on the BF537 and also for the AD1754 DAC.

A backwards-compatibility option is provided for use with version 1.0
of the ADSP-BF537 EZ-Kit Lite (which requires the DSP to act as clock
master for both the ADC and the DAC). This option is selected by
defining the preprocessor symbol EZKIT_1_0 when compiling AD1871.cpp
and AD1854.cpp. This option can also be used on version 1.1 EZ-Kit
boards provided that pins 6, 7, and 8 of SW7 are switched OFF.


Pipelined I/O:
--------------

In the input/output APIs typically provided by operating systems, the
default behaviour is for the read and write functions (called
SyncRead() and SyncWrite() in VDK) to block until input to, or output
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
if it isn't - so that while the thread is nonblocked there is no
buffer available for I/O. Pipelining allows there to be 0, 1 or 2
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
complex than for using a non-pipelined device. Similarly, the
code for the device driver itself (AD1836 in this example) is only
moderately more involved than that for a simple non-pipelined driver
and is probably less complex (as well as more efficient) than a driver
which depends on internal buffering.


Message loopback:
-----------------

The example application uses the technique of "message loopback",
whereby messages are returned to their point of origin for reuse,
rather than being destroyed. All messages used in the system are
created at startup. This method has a number of advantages:

- It is more efficient (in runtime cycles) than constantly destroying
  and creating messages.

- Failure of message creation (i.e. due to pool exhaustion) cannot
  occur during steady-state operation.

- The loopback path regulates the flow of messages, preventing the sender
  from issuing messages at a faster rate than they can be consumed.

- Thread scheduling is driven by data flow, rather than priority.


Volume Control:
---------------

The example application does volume control in software, since there
is no access to the volume registers on the AD1854 DAC.

This software approach uses fixed-point multiplication to attenuate
the source signal by a fractional factor. To avoid signal degradation
due to loss of precision, the algorithm produces a 24-bit result from
a 16-bit input. The right-shift of 8 bits needed to convert each 32-bit
multiplication result into a right-justified 24-bit sample is incorporated
into the multiply by pre-dividing the attenuation factor by 256.

Because the value 1.0 cannot be represented in 1.15 signed fixed-point
form, negative attenuation factors are used so that -1.0 can be used
as the "full volume" setting. This introduces a 180-degree phase shift
into the output signal, but it is assumed that this is not a problem
(in any case it is easily remedied by a second multiplication by -1.0,
at the cost of some extra cycles per iteration of the attenuation
loop).

The volume processing is performed by an Attenuator thread, which acts
as the destination for a message loop of 16-bit stereo audio blocks
and also as the source of a separate (but synchronised) loop of
24-bit audio blocks. This arrangement requires the audio input device
to be operating in 16-bit mode and the output device in 24-bit mode.
Any 24-bit audio blocks received by the Attenuator thread are passed
through unaltered to the output. For maximum performance the attenuation
loop function is hand-written in assembly, using the vector multiply
instruction.

Changes of volume are initiated by the VolRamp thread. This polls
pushbuttons 1 and 2 - 10 times a second - using them as increment and
decrement volume buttons. Each time the volume setting changes the
VolRamp thread sends a control message to the Attenuator, specifying
the new attenuation value(s). The attenuation is specified in
decibels, in the (float) range 0.0 (full volume) - 48.0 (mute).

The communication between the VolRamp thread and the Attenuator is by
Remote Procedure Call (RPC) style messaging. This is a special case of
the message loopback described above, where a message is sent to a
destination and the sender immediately blocks in PendMessage() until
the same message is returned to it. This streamlines message and
payload management; automatic variables can be used as message
payloads because they will stay in existence while the sending thread
is blocked (unless it is forcibly destroyed by another thread).


Files:
------

readme.txt
	This file.

AD1871.h
AD1871.cpp
AD1854.h
AD1854.cpp
EVT_IVG9.asm
Init.asm
	The pipelined device drivers for the AD1871 ADC and the AD1854 DAC.

AudioMsg.h
	A header file containing declarations of constants and
	enumerations which are required by all thread classes.

Input.h
Input.cpp
	A thread class which reads in audio data via the AD1871 device
	driver and sends it to the a Relay thread as a stream of
	messages.

Relay.h
Relay.cpp
	A thread class which examines an audio message stream to
	determine the peak signal level (of 1 in every 100 messages)
	and uses this value to set the LEDs, using them as a bar level
	meter. Each message received is forwarded to the Input thread
	after processing.

Attenuator.h
Attenuator.cpp
VolCalc.asm
	The 16-to-24-bit volume attenuator thread, described above.

VolRamp.h
VolRamp.cpp
	The pushbutton volume-control thread, described above.

Output.h
Output.cpp
	A thread class which receives messages containing audio data
	and writes the data to the AD1854 device driver.

PipelinedAudioDriver.dpj
PipelinedAudioDriver.vdk
	VisualDSP++ project files for this project.

ExceptionHandler-BF537.asm
        User exception handler.


Operation description:
----------------------

- Turn ALL pins of SW7 ON and turn pins 1,2 of SW8 ON and 3-6 of SW8 OFF on the 
  ADSP-BF537 EZ-KIT Lite.

- Open the project "PipelinedAudioDriver.dpj" in an ADSP-BF537 EZ-KIT
  Lite session in the VisualDSP Integrated Development Environment (IDDE).

- Setup an input source (such as a radio) to the "line in" connector
  and an output source (such as headphones or powered speakers) to the
  "line out" connector of the EZ-KIT Lite.  Both connectors are 3.5mm
  stereo jack sockets. See the ADSP-BF537 EZ-KIT Lite User's Manual
  for more information on setting up the hardware.

- Under the "Project" tab, select "Build Project" (program is then
  loaded automatically into DSP).

- Select "Run" from the "Debug" tab on the menu bar of VisualDSP.

- Listen to the operation of the talk-through and observe the behaviour of the LEDs.

- Press the PB1 and PB2 pushbuttons to control the output volume.

