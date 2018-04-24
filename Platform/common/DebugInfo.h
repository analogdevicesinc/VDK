/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: A struct for storing debug state relevent to a single thread
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$
 *
 * -----------------------------------------------------------------------------
 *  Comments: The VDK::DebugInfo class is a repository for additional optional
 *  state related to a thread.  This class/struct has no member functions, and
 *  the members variables are not required for release run-time operation.
 *  The information stores is for debug purposes only.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#ifndef DEBUGINFO_H
#define DEBUGINFO_H

namespace VDK
{

	class DebugInfo 
	{
	    
	public:

	    unsigned int	m_nNumTimesRun;

		// Time stats in clock cycles
	    int 			m_llCreationCycle[64/(CHAR_BIT*sizeof(int))];
		int  			m_llRunStartCycles[64/(CHAR_BIT*sizeof(int))];
	    int				m_llRunLastCycles[64/(CHAR_BIT*sizeof(int))];
	    int		  		m_llRunTotalCycles[64/(CHAR_BIT*sizeof(int))];

		// Time stats in TICKs
	    Ticks 			m_tCreationTime;
	    Ticks 		  	m_tRunStartTime;
	    Ticks 			m_tRunLastTime;
	    Ticks 			m_tRunTotalTime;
	};

}
#endif //DEBUGINFO_H

