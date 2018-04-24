/* ===========================================================================
 *
 *
 *	These are the AC'97 support routines used both in the 
 *  devicedrivers SpeakerMic and Phone.
 *
 * ===========================================================================*/

#ifndef _AC97_H_
 #define _AC97_H_

#define BUFSIZE 32
 
 #include "VDK.h"

 #if defined (__ECC__)

  struct ThreadCtrl {
	int lstrd;
	int lstwr;
	ThreadCtrl() {lstrd = 0; lstwr = 0;};
  };

//  extern volatile DMADescriptor rx_desc[];	

  extern volatile short AC97in[2][BUFSIZE][2];
  extern volatile short *PhoneInPtr;
  extern volatile short *MicInPtr;

  extern volatile short AC97out[2][BUFSIZE][2];
  extern volatile short *PhoneOutPtr;
  extern volatile short *SpeakerPtr;

  extern volatile unsigned AC97Cmd;
  
  extern volatile short rxBuffer[2][16];
  extern volatile short *rxPtr;
  extern volatile short txBuffer[2][16];
  extern volatile short *txPtr;

  extern int AC97actInCntr;
  extern int AC97actOutCntr;

  void InitAC97(); 

 #else
  .section/data data1;
  .global _AC97in;
  .type _AC97in, STT_OBJECT;
  .global _PhoneInPtr;
  .type _PhoneInPtr, STT_OBJECT;
  .global _MicInPtr;
  .type _MicInPtr, STT_OBJECT;
  .global _rx_desc;
  .type _rx_desc, STT_OBJECT;

  .global _AC97out;
  .type _AC97out, STT_OBJECT;
  .global _PhoneOutPtr;
  .type _PhoneOutPtr, STT_OBJECT;
  .global _SpeakerPtr;
  .type _SpeakerPtr, STT_OBJECT;

  .global _AC97Cmd;
  .type _AC97Cmd, STT_OBJECT;

  .global _rxBuffer;
  .type _rxBuffer, STT_OBJECT;
  .global _rxPtr;
  .type _rxPtr, STT_OBJECT;

  .global _txBuffer;
  .type	_txBuffer, STT_OBJECT;
  .global _txPtr;
  .type _txPtr, STT_OBJECT;

 #endif
#endif
