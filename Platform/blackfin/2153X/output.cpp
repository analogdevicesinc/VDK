/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is an example file for source code formatting
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 * Output methods for test suite compiles
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "VDK_Thread.h"
#include "output.h"


#define BITS_PER_INT_       (CHAR_BIT * sizeof(int))


/**
 * STDIO primitve
 */
extern "C" int PRIMITIVE_WRITE(int fileID, int aligned, unsigned char* buf, size_t n);

static const char hex_nums[] = "0123456789ABCDEF";


/** ----------------------------------------------------------------------------
 * FUNC   : Output::Output()
 *
 * DESC   : Initializes the Output class
 *
 * PARAMS : N/A
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
VDK::Output::Output()
{
    // Setup some variables
    m_OutputIndex = 0;
    m_CurrentByte = 0;
    m_WordsSinceLastFlush = 0;
    m_LastFlushIndex = 0;
    m_ShouldSTDIO = true;
    m_ShouldHex = true;
    m_ShouldPrintThreadID = true;
    m_LastCallingThreadID = (VDK::ThreadID)(-1);

    // Clear the output buffer
	for (unsigned int i = 0; i < kBufferSize; ++i)
		m_OutputPtr[i] = 0;
}


/** ----------------------------------------------------------------------------
 * FUNC   : Output::~Output()
 *
 * DESC   : Destroys the Output class
 *
 * PARAMS : N/A
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
VDK::Output::~Output()
{
    flush();
}


/** ----------------------------------------------------------------------------
 * FUNC   : Output::PrintThreadID
 *
 * DESC   : Prints the ThreadID of the calling thread if it's different from last time
 *
 * PARAMS : N/A
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
void
VDK::Output::PrintThreadID()
{
    // Should we output the ThreadID???
    if ((m_ShouldPrintThreadID == true) && (GetThreadID() != m_LastCallingThreadID))
    {
        // Don't want to print the ThreadID next time
        m_LastCallingThreadID = GetThreadID();

        // Put the Hex encoded ThreadID
        for (int i = 8 * kBytesPerChar - 4; i >= 0; i -= 4)
            out_char(hex_nums[(m_LastCallingThreadID >> i) & 0xF]);

        // Output the separater
        out_char(' ');
        out_char('-');
        out_char('-');
        out_char(' ');
    }
}

/** ----------------------------------------------------------------------------
 * FUNC   : Output::out_char()
 *
 * DESC   : Puts a character into the stream
 *
 * PARAMS : inChar -- the character to write
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
VDK::Output&
VDK::Output::out_char(char inChar)
{
    // Put the byte on the output
    m_OutputPtr[m_OutputIndex] |= (inChar << (m_CurrentByte * 8));
    ++m_CurrentByte;

	if (inChar == '\n')
		flush();

    // See if we've looped over
    if (m_CurrentByte >= kBytesPerChar)
    {
        // Increment the current location counter, and the number of words
        // since the last flush
        ++m_WordsSinceLastFlush;
        ++m_OutputIndex;
        m_CurrentByte = 0;

        // Flush the output buffer if necessary
        if(m_WordsSinceLastFlush >= kBufferSize)
            flush();

        // Roll over the output buffer pointer if necessary
        if (m_OutputIndex >= kBufferSize)
            m_OutputIndex = 0;

        // Clear the byte that's written
        m_OutputPtr[m_OutputIndex] = 0;
    }
    return *this;
}



/** ----------------------------------------------------------------------------
 * FUNC   : Output::flush()
 *
 * DESC   : Flushes the output buffer
 *
 * PARAMS : N/A
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
VDK::Output&
VDK::Output::flush()
{
    VDK::PushCriticalRegion();

    // Output to stdio if necessary
    if (m_ShouldSTDIO == true)
    {
        // Output the packed bytes
        while(m_WordsSinceLastFlush-- != 0)
        {
            // Output the bytes packed into the current work
            for (int j = 0; j < (kBytesPerChar * 8); j += 8)
                putchar((m_OutputPtr[m_LastFlushIndex] >> j) & 0xFF);

            // Increment the count
            ++m_LastFlushIndex;
            if(m_LastFlushIndex >= kBufferSize)
                m_LastFlushIndex = 0;
        }

        // Output the partially packed byte
        for (int i = 0; i < (m_CurrentByte * 8); i += 8)
            putchar((m_OutputPtr[m_LastFlushIndex] >> i) & 0xFF);


        // Flush stdout
        fflush(stdout);
    }

    // We don't have any partial words written
    m_CurrentByte = 0;

    // We want to increment the output index
    ++m_OutputIndex;
    if (m_OutputIndex >= kBufferSize)
        m_OutputIndex = 0;
    m_OutputPtr[m_OutputIndex] = 0;

    // We want to reset everything
    m_WordsSinceLastFlush = 0;
    m_LastFlushIndex = m_OutputIndex;

    VDK::PopCriticalRegion();

    return *this;
}


/** ----------------------------------------------------------------------------
 * FUNC   : Output::endl()
 *
 * DESC   : Prints a '\n' and flushes the output buffer
 *
 * PARAMS : N/A
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
VDK::Output&
VDK::Output::endl()
{
    VDK::PushCriticalRegion();
    out_char('\n');
    VDK::PopCriticalRegion();
    return *this;
}



/** ----------------------------------------------------------------------------
 * FUNC   : Output::operator<<
 *
 * DESC   : Outputs a number
 *
 * PARAMS : inStream -- the stream to write the string onto
 *          inNum -- the number to output
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
VDK::Output&
VDK::Output::operator<<(const unsigned int inNum)
{
    // Stop interrupts
    VDK::PushCriticalRegion();

    // Output the ThreadInfo
    VDK::Output::PrintThreadID();

    // Output the number
    if (m_ShouldHex == true)
        for (int i = 8 * kBytesPerChar - 4; i >= 0; i -= 4)
            out_char(hex_nums[(inNum >> i) & 0xF]);
    else
    {
        unsigned int    num = inNum;
        unsigned int    count = 0;
        char            dec_store[16];

        // calculate the number
        do
        {
            unsigned int temp = num / 10u;
            unsigned int remainder = num - (10u * temp);
            dec_store[count] = hex_nums[remainder]; // hex_nums works b/c the decimal numbers are first
            ++count;
            num = temp;
        } while (num > 0);

        // output the number
        for (int i = 1; i <= count; ++i)
            out_char(dec_store[count - i]);
    }

    // We can re-enable interrupts now
    VDK::PopCriticalRegion();

    return *this;
}


/** ----------------------------------------------------------------------------
 * FUNC   : Output::operator<<
 *
 * DESC   : Outputs a string
 *
 * PARAMS : inStr -- the string to output
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
VDK::Output&
VDK::Output::operator<<(const int inNum)
{
	unsigned int the_num = inNum;

    // Disable interrupts
    VDK::PushCriticalRegion();

    // Output the thread info
    VDK::Output::PrintThreadID();

    // Output the sign bit if necessary
    if (m_ShouldHex == false)
        if (inNum < 0)
        {
            out_char('-');
			the_num = (unsigned)(~inNum) + 1;
        }

    // Output the number
    *this << (the_num);

    // Reenable interrupts
    VDK::PopCriticalRegion();

    return *this;
}


/** ----------------------------------------------------------------------------
 * FUNC   : Output::operator<<
 *
 * DESC   : Outputs a string
 *
 * PARAMS : inStr -- the string to output
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
VDK::Output&
VDK::Output::operator<<(const char *inStr)
{
    // Stop interrupts
    VDK::PushCriticalRegion();

    // Print the ThreadID
    VDK::Output::PrintThreadID();

    // Output the string
    while(*inStr)
    {
        out_char(*inStr);
        inStr++;
    }

    // Restart interrupts
    VDK::PopCriticalRegion();

    return *this;
}

/** ----------------------------------------------------------------------------
 * FUNC   : Output::operator<<
 *
 * DESC   : Outputs a character
 *
 * PARAMS : inChar -- that character to output
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
VDK::Output&
VDK::Output::operator<<(char inChar)
{
    // Stop interrupts
    VDK::PushCriticalRegion();

    // Print the threadID
    VDK::Output::PrintThreadID();

    out_char(inChar);

    // Restart interrupts
    VDK::PopCriticalRegion();

    return *this;
}


/** ----------------------------------------------------------------------------
 * FUNC   : TestOut_PrintString
 *
 * DESC   : C Callable routine to print a string
 *
 * PARAMS : inStr -- The String
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
extern "C"
void
TestOut_PrintString(const char *inStr)
{
    VDK::test_out << inStr;
}


/** ----------------------------------------------------------------------------
 * FUNC   : TestOut_PrintNumber
 *
 * DESC   : C Callable routine to print a number to the Test Output
 *
 * PARAMS : inNum -- The number
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
extern "C"
void
TestOut_PrintNumber(const int inNum)
{
    VDK::test_out << inNum;
}



/** ----------------------------------------------------------------------------
 * FUNC   : TestOut_Flush
 *
 * DESC   : C Callable routine to Flush the buffer
 *
 * PARAMS : N/A
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
extern "C"
void
TestOut_Flush()
{
    VDK::test_out << VDK::flush;
}



/** ----------------------------------------------------------------------------
 * FUNC   : TestOut_Dec
 *
 * DESC   : C Callable routine to change the buffer to dec
 *
 * PARAMS : N/A
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
extern "C"
void
TestOut_Dec()
{
    VDK::test_out << VDK::dec;
}


/** ----------------------------------------------------------------------------
 * FUNC   : TestOut_Hex
 *
 * DESC   : C Callable routine to change the buffer to hex
 *
 * PARAMS : N/A
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
extern "C"
void
TestOut_Hex()
{
    VDK::test_out << VDK::hex;
}




