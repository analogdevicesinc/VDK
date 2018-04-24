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
 * This defines a class that handles output to a buffer
 *
 * The output is ENTIRELY in ASCII.  Numbers are output as hex (128 = 0000 0080)
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#ifndef   OUTPUT_H_    
#define   OUTPUT_H_

#ifdef __cplusplus

#include <limits.h>

namespace VDK
{

    class Output
    {
    protected:
		enum { kBufferSize = 64 };

        char            m_OutputPtr[kBufferSize];   // Pointer to the buffer in memory
        unsigned int    m_OutputIndex;              // Index in the output where we're dumping characters
        unsigned int    m_CurrentByte;              // The number of bytes we've output on this char
        unsigned int    m_WordsSinceLastFlush;      // The number of words since last flush
        unsigned int    m_LastFlushIndex;           // The index at which we did the last flush
        bool            m_ShouldSTDIO;              // Should the output be going to stdio
        bool            m_ShouldHex;                // True when we should be outputting the numbers as hex
        bool            m_ShouldPrintThreadID;      // True when we should output the ThreadID of the calling thread
        ThreadID        m_LastCallingThreadID;      // ThreadID of the last calling thread
    
        // Outputs the ThreadID of the calling thread if it's different from the last time this function was called
        void        PrintThreadID();

        // Outputs a character on the stream
        // MUST BE CALLED FROM A CRITICAL REGION!
        Output&     out_char(char);

    public:
        enum { kBytesPerChar = (CHAR_BIT / 8) } ;

		Output();
        ~Output();

        // Should we be outputting the buffer to STDIO when we have finished
        bool        stdio() const
                        { return m_ShouldSTDIO; }
        bool&       stdio()
                        { return m_ShouldSTDIO; }

        // Should we output the Thread information
        bool        ShouldPrintThreadID() const
                        { return m_ShouldPrintThreadID; }
        bool&       ShouldPrintThreadID()
                        { return m_ShouldPrintThreadID; }

        // Called when we want to flush the output
        Output&     flush();

        // Outputs a newline and flush
        Output&     endl();

        // Called to output hex
        Output&     hex()
                        { m_ShouldHex = true; return *this; }
        Output&     dec()
                        { m_ShouldHex = false; return *this; }

        // Stream manipulator
        Output&     operator<<(VDK::Output& (*f)(Output&))
                        { return f(*this); }

        // Members to output variables
        Output&     operator<<(const unsigned int);
        Output&     operator<<(const char*);
        Output&     operator<<(const char);
        Output&     operator<<(const int inNum);
    };

    inline VDK::Output& flush(Output& inStream) { return inStream.flush(); }
    inline VDK::Output& endl(Output& inStream) { return inStream.endl(); }
    inline VDK::Output& hex(Output& inStream) { return inStream.hex(); }
    inline VDK::Output& dec(Output& inStream) { return inStream.dec(); }

    extern VDK::Output test_out;
}

#else /* __cplusplus */
/**
 * C callable TestOut for strings
 */
void TestOut_PrintString(char*);


/**
 * C callable TestOut for strings
 */
void TestOut_PrintNumber(const int);

/**
 * C callable flush routine
 */
void TestOut_Flush();

/**
 * C callable base changes
 */
void TestOut_Dec();
void TestOut_Hex();

#endif /* __cplusplus */


#endif    /*  OUTPUT_H_ */


