/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is an example file for header file formatting
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: A control structure for DeviceDrivers
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#ifndef DEVICE_CONTROL_BLOCK_H_
#define DEVICE_CONTROL_BLOCK_H_

#include <VDK_API.h>

namespace VDK
{
	/**
	 * This class abstracts the device control block from the Thread.
	 *
	 * Device Drivers each have a place where they can store thread specific
	 * data.  This class abstractst that control block.
	 *
	 * This class will be used MUCH more once Protocol Stacks are implemented
	 */
	class DeviceControlBlock
	{
	public:
		/**
		 * Construct the Device Control Block
		 */
		DeviceControlBlock()
	    {
		    m_DD_DataPtr = 0;
	    }

// BEGIN_INTERNALS_STRIP
		/**
		 * Get the address of the device data pointer.
		 */
		void**	    DD_DataPointer()
				        { return &(m_DD_DataPtr); }
        /**
         * Returns the ID of the Device this DCB is used for
         */
        IOID    WhichDevice() const
                        { return m_DeviceID; }
        IOID&   WhichDevice()
                        { return m_DeviceID; }
// END_INTERNALS_STRIP
	protected:
		void	    *m_DD_DataPtr;
        IOID    m_DeviceID;
	};
} // namespace VDK



#endif /* DEVICE_CONTROL_BLOCK_H_ */


