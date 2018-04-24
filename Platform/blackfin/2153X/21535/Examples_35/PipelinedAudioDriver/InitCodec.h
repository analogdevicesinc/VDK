#include <defBF535.h>
#include <sysreg.h>

/********************************************************************************/
/***** Prototypes															*****/
/********************************************************************************/

/********************************************************************************/
/***** Global Variables														*****/
/********************************************************************************/


/********************************************************************************/
/***** Symbolic constants													*****/
/********************************************************************************/
// symbolic names for codec registers, used for CodecRegs[]
#define RESET					0x0000
#define MASTER_VOLUME			0x0200
#define HEADPHONES_VOLUME		0x0400
#define MASTER_VOLUME_MONO		0x0600	
#define PC_BEEP					0x0A00
#define PHONE_VOLUME			0x0C00
#define MIC_VOLUME				0x0E00
#define LINE_IN_VOLUME			0x1000
#define CD_VOLUME				0x1200
#define VIDEO_VOLUME			0x1400
#define AUX_VOLUME				0x1600
#define PCM_OUT_VOLUME			0x1800
#define RECORD_SELECT_CONTROL	0x1A00
#define RECORD_GAIN				0x1C00
#define GENERAL_PURPOSE			0x2000
#define THREE_D_CONTROL			0x2200
#define SUBSECTION_READY		0x2600
#define EXTENDED_AUDIO_ID		0x2800
#define EXTENDED_AUDIO_CTL		0x2A00
#define PCM_DAC_RATE			0x2C00
#define PCM_ADC_RATE			0x3200
#define JACK_SENSE				0x7200
#define SERIAL_CONFIGURATION	0x7400
#define MISC_CONTROL_BITS		0x7600
#define VENDOR_ID_1				0x7C00
#define VENDOR_ID_2				0x7E00

// just a substitution
#define Idle asm("idle;");


