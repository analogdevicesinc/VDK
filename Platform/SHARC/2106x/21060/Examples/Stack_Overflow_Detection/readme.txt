
Description:
------------

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

The Waiter thread needs to run at as high a priority as is practical,
so that it runs as soon as the semaphore is posted.

After printing its message the Waiter destroys the Recurser thread, on
the assumption (correct in this case) that it is the failing
thread. However, this approach is not applicable to the general case,
as there is (at present) no way for the code to determine which thread
was running at the time of the overflow interrupt. Instead, during
development (i.e. with debugging and instrumentation) it is better to
set a breakpoint on the puts() line, or immediately after it. If/when
the program stops at this breakpoint, the VDK History window will show
which thread was active at the point where the Waiter's status changed
from SemaphoreBlocked to Ready (or Running). This Thread (i.e. the
thread which caused the overflow interrupt) will show a
SemaphorePosted event arrow at the point where the interrupt occurred.

In fact, during application development it may be sufficient simply to
define the CB7I interrupt handler and set a breakpoint there,
dispensing with the semaphore and the Waiter thread entirely. However,
when this breakpoint is hit there may not have been any VDK history
events since the failing thread was switched in, so the history
display may appear to end abruptly at that ThreadSwitched event.

Even if the CB7 interrupt is masked (as it will be if no interrupt
handler is defined for it in the IDDE Kernel tab) the hardware will
still latch the interrupt if a stack overflow occurs. Therefore, if
an application behaves unexpectedly (when running under the debugger)
it may be worthwhile to check the Register->Core->IRQs window to see
if the CB7 bit has been set to 1 in the IRPTL register.



Files:
------
	Recurser.cpp
	Recurser.h
		A thread whose only purpose is to overflow its own stack
		and hence invoke the CB7 interrupt.
	Waiter.cpp
	Waiter.h
		A thread which creates a Recurser thread, pends on the
		'kOverflow' semaphore, and then prints the message
		"Thread stack overflowed!"  (and destroys the
		Recurser) once the semaphore is posted.
	CB7I.asm
		An interrupt service routine which is invoked whenever a
		CB7 (circular buffer 7 overflow) interrupt occurs.
	cc.dpj
		The IDDE project file
	cc.vdk
		The VDK file required by the IDDE project
