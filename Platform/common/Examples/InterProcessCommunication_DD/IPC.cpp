/* =============================================================================
 *
 *  Description: This is a C++ implementation of a VDK Device Driver for IPC
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "VDK.h"
#include "IPC.h"
#include <ctype.h>
#include <string.h>

#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")

#ifdef VDK_INCLUDE_IO_

/* globals */
// The number of Channels must be equal to the number of Device Flags given
// to this device in the IDDE.
const unsigned int      kNumChannels = 4;

// Number of chars a channel holds
const unsigned int      kChannelSize = 32;

// A thread can never have a threadID of -1, so this is a valid ID
const VDK::ThreadID     kNoOwner = static_cast<VDK::ThreadID>(-1);

// Meaning that a thread has no channel open
const int                       kInvalidIndex = -1;



/******************************************************************************
 * Each channel has an owner and a recipient thread on a channel.  The
 * recipient is used by writes to determine if the there is someone listening
 * for data, if no thread is listening, the writing thread can just dump the
 * data to the channel without worrying about writing too much data.  A
 * reading thread checks to make sure that the owner hasn't closed the channel
 * whenever it goes to block.  If the channel is closed by the owner, the
 * listening thread will not block.
 *
 * Each channel is also named so that a thread can specify which channel it
 * wants to open.
 */
struct DataChannel
{
        VDK::ThreadID   owner;
        VDK::ThreadID   recipient;
        int                             w_pos;
        int                             r_pos;
        char                    name;
        char                    data[kChannelSize];
};

/* statics */

DataChannel             channels[kNumChannels];


/* externs */

/* preprocessor macros */



/******************************************************************************
 * Initializes the IPC device driver global data
 *
 * Typical device drivers initialize all global data and setup ISRs.  Since
 * this device driver has no ISRs, no ISR setup takes place.
 *
 * This function is called within a critical region from the dispatch function
 * below.
 */
void
IPC_Init()
{
	// Initialize all the channels so that they're not used.
	for (int i = 0; i < kNumChannels; ++i)
    {
		channels[i].owner = kNoOwner;
		channels[i].recipient = kNoOwner;
    }
}


/******************************************************************************
 * Opens a channel
 *
 * For this device driver, the device flags specifiy how the channel is opened.
 * For example, the following call:
 *
 *      VDK::OpenDevice(kIPC, "W4");
 *
 * would open a channel named '4' for writing.  The call in the receiving
 * thread would look like:
 *
 *      VDK::OpenDevice(kIPC, "R4");
 *
 * One note: A channel MUST BE OPENED for writing before it can be opened for
 * reading.
 *
 * Typical device drivers would check their state, and allocate a space for
 * a data structure that is device specific.  Since all data for the channels
 * is stored in a global array of data structures, the only information that
 * needs to be stored is the index of the data structure that specifies
 * the information.
 *
 * A device dispatch function is not called within a critical or unscheduled
 * region during an Open, so any dependence on global data structures needs
 * to be protected in one.
 */
int
IPC_Open(VDK::DispatchUnion &inUnion)
{
	bool	read_channel;
	int		index = kNumChannels;	// the index of the channel we're openning
	char	channel_name;			// the letter name of the channel we're trying to open

	// Setup the thread specific data
	*inUnion.OpenClose_t.dataH = reinterpret_cast<void*>(kInvalidIndex);

	// Check that we have valid flags
	if (inUnion.OpenClose_t.flags == 0)
		return VDK::DispatchThreadError(VDK::kOpenFailure, 0);

	// Setup variables about the type of channel we're trying to open
    if ((toupper(inUnion.OpenClose_t.flags[0]) != 'R') && 
        (toupper(inUnion.OpenClose_t.flags[0]) != 'W'))
		return VDK::DispatchThreadError(VDK::kOpenFailure, 0);
	read_channel = (toupper(inUnion.OpenClose_t.flags[0]) == 'R')? true:false;
	channel_name = inUnion.OpenClose_t.flags[1];

	// We need to be in an unscheduled region from now on b/c we will be depending
	// on values in the data structures
	VDK::PushUnscheduledRegion();

	if (read_channel == false)
	{
		// We're openning a write channel, so find a free one.
		for (index = 0; index < kNumChannels; ++index)
		{
			// is the channel already open???
			if ((channels[index].owner != kNoOwner) &&
				(channels[index].name == channel_name))
			{
				index = kNumChannels;
				break;
			}

			// Is this channel available, if so, we can go on.
			if ((channels[index].owner == kNoOwner) &&
			    (channels[index].recipient == kNoOwner))
			{
				// We have found a valid channel, so let's open it, and get it ready
				channels[index].name = channel_name;
				channels[index].owner = VDK::GetThreadID();
				channels[index].recipient = kNoOwner;
				channels[index].w_pos = 0;
				channels[index].r_pos = 0;
				break;
			}
		}
	}
	else
	{
		// We're openning a channel for reading, so find it
		for (index = 0; index < kNumChannels; ++index)
			if ((channels[index].owner != kNoOwner) &&
				(channels[index].name == channel_name) &&
				(channels[index].recipient == kNoOwner))
			{
				channels[index].recipient = VDK::GetThreadID();
   				channels[index].r_pos = channels[index].w_pos;
				break;
			}
	}

	// If we didn't find the channel, throw an error, and exit
	if (index == kNumChannels)
    {
        VDK::PopUnscheduledRegion();
		return VDK::DispatchThreadError(VDK::kOpenFailure, 0);
    }

	// Save the channel id num in the thread
    // We use the HIGH bit of the id number to store whether or not
    // the thread has the device open for reading or writing.
    index |= ((read_channel == true)? INT_MIN : 0);
	*inUnion.OpenClose_t.dataH = reinterpret_cast<void*>(index);

	// We're done depending on the data structures
	VDK::PopUnscheduledRegion();

	return 0;
}


/******************************************************************************
 * Close a channel
 *
 * This device driver could have an error when the opening and receiving
 * thread is the same thread.  Care should be taken.
 *
 * Typical device drivers would free up the memory allocated when the device
 * was opened, and clear up any device specific data structures.
 
 * A device dispatch function called to close the device not necessarily called
 * in a critical or unscheduled region, so any dependence on global data
 * structures should be protected in a critical or unscheduled region.
 * Additionally, when closing a device, the dispatch function is not necessarily
 * called within the context of the thread that opened the device.
 */
int
IPC_Close(VDK::DispatchUnion &inUnion)
{
	int	index = reinterpret_cast<int>(*inUnion.OpenClose_t.dataH);

	// Did we get a valid index
	if (index != kInvalidIndex)
	{
        // Was it open for reading or writing?
        bool    was_read = index & INT_MIN;

        // Strip off the high bit
        index &= INT_MAX;

        // Typical device drivers would need to clean up their global data
        // structures in a critical (not unscheduled) region, but since this
        // device driver does not depend on ISRs, this is good enough (and
        // leaves interrupt servicing on as long as possible).
		VDK::PushUnscheduledRegion();

        // Clean up the channel structures
		if (was_read)
			channels[index].owner = kNoOwner;
        else
            channels[index].recipient = kNoOwner;

        // If any thread is waiting on data to be read from or written to this
        // device, we want the thread to stop blocking.
    	DeviceFlagID	dev_flag = static_cast<DeviceFlagID>(index + kIPC_ch1);
        VDK::PostDeviceFlag(dev_flag);

        // We're done depending on global data structures.
		VDK::PopUnscheduledRegion();
	}

	return 0;
}


/******************************************************************************
 * Write to a channel
 *
 * When a thread attempts to write more data to a channel than there is space
 * for, the thread pends on the on a device flag which is posted when the
 * reading thread has performed any read.
 *
 * A typical device driver's write function is not necessarily called within a
 * critical or unscheduled region, so any dependence on global data structures
 * must be protected.
 */
unsigned int
IPC_Write(VDK::DispatchUnion &inUnion)
{
	int				index = reinterpret_cast<int>(*inUnion.ReadWrite_t.dataH);

	// Make sure I'm the owner
	if ((index == kInvalidIndex) ||
		(channels[index & INT_MAX].owner != VDK::GetThreadID()))
		return VDK::DispatchThreadError(VDK::kWriteFailure, 0);

    // Strip off the high bit
    index &= INT_MAX;

	// Setup some variables
	DeviceFlagID	dev_flag = static_cast<DeviceFlagID>(index + kIPC_ch1);
	DataChannel		*ch_ptr = &(channels[index]);
	char			*input_ptr = inUnion.ReadWrite_t.data;
	unsigned int	bytes_left_to_write = inUnion.ReadWrite_t.dataSize;
	unsigned int	total_copied = 0;
	unsigned int	space_available;
	unsigned int	amount_copied;

    // Typical device drivers would need to execute most of their code in a
    // critical region, not an unscheduled one.  Since this device driver has no
    // ISR routines, no critical regions are required, only unscheduled ones. 
    // This will cause some headaches when calling PendDeviceFlag (below), but
    // in the interest of keeping interrupts serviced as long as possible, this is
    // the correct architecture for this particular device driver.
	VDK::PushUnscheduledRegion();

	// Write the bytes
	while(bytes_left_to_write != 0)
	{
        // Most of the code below deals with circular buffering index pointers, etc...
        // Many device drivers (like codecs, FIFOs, UARTS), will use code similar to this.
        // Although there may be some optimization to be gained from combining this code with
        // IPC_Read(...), etc, for clarity sake this code is kept in two locations.

		// Figure out how much space we can write
		if ((ch_ptr->r_pos <= ch_ptr->w_pos) || (ch_ptr->recipient == kNoOwner))
			space_available = kChannelSize - ch_ptr->w_pos;
		else
			space_available = ch_ptr->r_pos - ch_ptr->w_pos;

		// How much do we want to copy
		amount_copied = (space_available < bytes_left_to_write)? space_available : bytes_left_to_write;

		// Copy the data
		memcpy(&ch_ptr->data[ch_ptr->w_pos], input_ptr, amount_copied);

		// Post the device flag so anything waiting for a write can continue
        // reading data from the channel
		if (ch_ptr->recipient != kNoOwner)
			VDK::PostDeviceFlag(dev_flag);

		// Update the variables to keep circular buffering going.
		bytes_left_to_write -= amount_copied;
		input_ptr += amount_copied;
		total_copied += amount_copied;
		ch_ptr->w_pos += amount_copied;

		// Wait if we have to
		if ((ch_ptr->recipient != kNoOwner) &&
		    ((ch_ptr->w_pos == ch_ptr->r_pos) || 
		    ((ch_ptr->w_pos == kChannelSize) && (ch_ptr->r_pos == 0))))
		{
            // We need to be in a critical region when we enter PendDeviceFlag,
            // and we're currently in an unscheduled one.  (Remember PendDeviceFlag
            // has its own PopCriticalRegion, so, this push is balanced there.)
            // Additionally, since we are pushing a critical region, and there was
            // already a single unscheduled region, after the critical region push
            // we will be in 1 critical region, and 2 unscheduled ones.  So, 
            // we also want to pop the unscheduled region from before the while loop.
            // All this code is for managing the regions correctly.
			VDK::PushCriticalRegion();
			VDK::PopUnscheduledRegion();
			VDK::PendDeviceFlag(dev_flag, inUnion.ReadWrite_t.timeout);

            // We will begin depending on information in the data structures again, so
            // want to re-enter Unscheduled regions
			VDK::PushUnscheduledRegion();

            // See if we had an error
            if (VDK::GetLastThreadError() == VDK::kDeviceTimeout)
            {
                total_copied = VDK::GetLastThreadErrorValue();
                bytes_left_to_write = 0;
            }
		}

        // Fix up the copy index
		if (ch_ptr->w_pos == kChannelSize)
			ch_ptr->w_pos = 0;
	}

	// This PopUnscheduledRegion takes us out of the one entered
    // before the while loop (or during the while loop if this thread
    // ever waited on a device flag.
	VDK::PopUnscheduledRegion();

	// Return the number of bytes copied
	return total_copied;
}


/******************************************************************************
 * Read from a channel
 *
 * When a thread tries to read more data then is available, it blocks on a
 * device flag that the writing thread posts when there is more data availble.
 *
 * A typical device driver's read function is not called within a critical
 * or unscheduled region, so any dependence on global data structures must
 * be protected.
 */
unsigned int
IPC_Read(VDK::DispatchUnion &inUnion)
{
	int		index = reinterpret_cast<int>(*inUnion.ReadWrite_t.dataH);

	// Make sure I'm the recipient
	if ((index == kInvalidIndex) ||
		(channels[index & INT_MAX].recipient != VDK::GetThreadID()))
		return VDK::DispatchThreadError(VDK::kReadFailure, 0);

    // Strip off the high bit
    index &= INT_MAX;

	// Setup some variables
	DeviceFlagID	dev_flag = static_cast<DeviceFlagID>(index + kIPC_ch1);
	DataChannel		*ch_ptr = &(channels[index]);
	char			*output_ptr = inUnion.ReadWrite_t.data;
	unsigned int	bytes_left_to_read = inUnion.ReadWrite_t.dataSize;
	unsigned int	total_copied = 0;
	unsigned int	data_available;
	unsigned int	amount_copied;

    // Typical device drivers would need to execute most of their code in a
    // critical region, not an unscheduled one.  Since this device driver has no
    // ISR routines, no critical regions are required, only unscheduled ones. 
    // This will cause some headaches when calling PendDeviceFlag (below), but
    // in the interest of keeping interrupts serviced as long as possible, this is
    // the correct architecture for this particular device driver.
	VDK::PushUnscheduledRegion();

	// Read the bytes
	while(bytes_left_to_read != 0)
	{
        // Most of the code below deals with circular buffering index pointers, etc...
        // Many device drivers (like codecs, FIFOs, UARTS), will use code similar to this.
        // Although there may be some optimization to be gained from combining this code with
        // IPC_Write(...), etc, for clarity sake this code is kept in two locations.

		// Figure out how much data there is
		if (ch_ptr->r_pos > ch_ptr->w_pos)
			data_available = kChannelSize - ch_ptr->r_pos;
		else
			data_available = ch_ptr->w_pos - ch_ptr->r_pos;

		// How much do we want to copy
		amount_copied = (data_available < bytes_left_to_read)? data_available : bytes_left_to_read;
	
		// Copy the data
		memcpy(output_ptr, &ch_ptr->data[ch_ptr->r_pos], amount_copied);

		// Update the variables
		bytes_left_to_read -= amount_copied;
		output_ptr += amount_copied;
		total_copied += amount_copied;
		ch_ptr->r_pos += amount_copied;

		// Post the device flag so anything waiting to write can get it
		VDK::PostDeviceFlag(dev_flag);

		// Wait if we have to
		if ((ch_ptr->r_pos == ch_ptr->w_pos) || 
		    ((ch_ptr->r_pos == kChannelSize) && (ch_ptr->w_pos == 0)))
		{
            // Check to make sure that the writing thread hasn't closed the channel
            if (ch_ptr->owner == kNoOwner)
            {
                total_copied = VDK::DispatchThreadError(VDK::kReadFailure, 0);
                bytes_left_to_read = 0;
            }

            // We need to be in a critical region when we enter PendDeviceFlag,
            // and we're currently in an unscheduled one.  (Remember PendDeviceFlag
            // has its own PopCriticalRegion, so, this push is balanced there.)
            // Additionally, since we are pushing a critical region, and there was
            // already a single unscheduled region, after the critical region push
            // we will be in 1 critical region, and 2 unscheduled ones.  So, 
            // we also want to pop the unscheduled region from before the while loop.
            // All this code is for managing the regions correctly.
			VDK::PushCriticalRegion();
			VDK::PopUnscheduledRegion();
			VDK::PendDeviceFlag(dev_flag, inUnion.ReadWrite_t.timeout);

            // We will begin depending on information in the data structures again, so
            // want to re-enter Unscheduled regions
			VDK::PushUnscheduledRegion();

            // See if we had an error
            if (VDK::GetLastThreadError() == VDK::kDeviceTimeout)
            {
                total_copied = VDK::GetLastThreadErrorValue();
                bytes_left_to_read = 0;
            }
		}

        // Fix up the last bytes
		if (ch_ptr->r_pos >= kChannelSize)
			ch_ptr->r_pos = 0;
	}

	// This PopUnscheduledRegion takes us out of the one entered
    // before the while loop (or during the while loop if this thread
    // ever waited on a device flag.
	VDK::PopUnscheduledRegion();

	// Return the number of bytes copied
	return total_copied;
}

/******************************************************************************
 * Dispatch Function for the IPC Device Driver
 *
 * This function is called in a critical region when a device
 * is being Initialized or Activated.
 */
 
void*
IPC::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
{
    void    *ret_val = 0;
    switch(inCode)
    {
        case VDK::kIO_Init:      
                // This is called in a critical region, and only called once
                // at system boot
                IPC_Init();
            break;
        case VDK::kIO_Activate:  
                // A pipe doesn't use any interrupts, so this should never be
                // called.
            break;
        case VDK::kIO_Open:      
                ret_val = reinterpret_cast<void*>(IPC_Open(inUnion));
            break;
        case VDK::kIO_Close:     
                ret_val = reinterpret_cast<void*>(IPC_Close(inUnion));
            break;
        case VDK::kIO_SyncRead:      
                ret_val = reinterpret_cast<void*>(IPC_Read(inUnion));
            break;
        case VDK::kIO_SyncWrite:     
                ret_val = reinterpret_cast<void*>(IPC_Write(inUnion));
            break;
        case VDK::kIO_IOCtl:
                // A pipe doesn't really need any I/O control, so do nothing.
            break;
        default:
            break;
    }
    return ret_val;
}

#endif /* VDK_INCLUDE_IO_ */

/* ========================================================================== */
