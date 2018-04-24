An Audio Driver Example for the ADSP-BF535 EZ-Kit Lite

* A device driver for the AD1885 Audio Codec
* Full duplex operation at 48KHz
* Supports "split open" behaviour
* Simultaneously open by one thread for reading and another for writing
* Uses the "pipelined" I/O model

* A digital audio feedthrough application
* Copies ADC input to DAC output
* Exploits the pipelined driver semantics
* Uses one thread for input and another for output
* Uses the messaging API to pass audio data between threads
* Uses a memory pool to store audio data
* Each block holds 1 millisecond of audio

The Example AD1855 Device Driver

* Is intended to illustrate:
* The object-based device driver model (introduced in VisualDSP++ 3.0)
* Split-open operation
* Device activation from an ISR
* Use of DeviceFlags for I/O completion
* A simple framework for 2-level pipelined I/O

* Is not intended to be:
* A comprehensive guide to AD1885 operation
* The only (or most efficient) solution possible
* A typical DMA device
* A highly time-critical example
* 6250 DSP cycles per sample doesn't present any latency problems

The Example Feedthough Application

* Is intended to illustrate:
* Handling each direction of a full-duplex device by a separate thread
* Utilization of pipelined I/O semantics
* Use of a memory pool for message payload allocation
* Use of message return for flow control and message/buffer recycling

* Is not intended to be:
* The only possible structure
* Output could originate empty messages, as in the Producer-Consumer example provided with VisualDSP++ 3.1
* Messages and buffers could be destroyed and reallocated instead of being recycled

The Device Driver Model


* Traditional synchronous read/write model has limitations for realtime I/O
* Where does the data come from/go to when the thread isn't blocking?
* Buffering within the device driver is wasteful of memory space, processing time and bandwidth
* Memory-to-memory copying can limit throughput
* Fully asynchronous I/O complicates tracking of I/O completion
* Would require additional Kernel support
* Pipelined Driver semantics permit efficient double (or multi-) buffering without excessive complexity
* Well suited to streaming realtime I/O


Synchronous I/O Vs. Pipelined I/O of Depth 2


* Synchronous read/write N
* Queues buffer N for input/output
* Blocks until I/O on buffer N completes
* Returns result of op. N
* Can only have zero or one buffers queued   
* No user buffer available while thread is busy
* Driver must provide additional buffering


* Pipelined read/write 0
* Queues Buffer 0 for input/output
* Doesn't block
* Returns zero
* Pipelined read/write N
* Queues buffer N for input/output
* Blocks until I/O on
buffer N-1 completes
* Returns result of op. N-1
* A zero-sized buffer blocks until the pipe is empty


Managing Latencies In Device Drivers


* Do the minimum necessary in ISR
* Dependant on tolerance to service latency
* May be as little as calling VDK_ISR_ACTIVATE_DEVICE_()
* e.g. if DMA chaining is being used
* Time-critical devices may need more work to be done in ISR
* i.e. if activation overhead is unacceptable
* Do as much as possible in activation handler
* C/C++ environment
* Access to driver instance variables
* But beware of variability in latency
* Only PostDeviceFlag() on buffer completion
* Avoids extra thread context switches


