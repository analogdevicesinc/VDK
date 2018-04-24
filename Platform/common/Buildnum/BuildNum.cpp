/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

// BuildNum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"
#include "malloc.h"


int main(int argc, char* argv[])
{
	SYSTEMTIME	theTime;
	unsigned long int		out_val,
							mask_year,
							mask_month,
							mask_day,
							mask_hour,
							mask_min,
							mask_sec,
							machine_ID;


	  DWORD volumeSerialNumber;     // volume serial number
	  DWORD fileSystemFlags;        // file system options


	GetSystemTime( &theTime );

	GetVolumeInformation(
	  "C:\\",						// root directory
	  0,							// volume name buffer
	  0,							// length of name buffer
	  &volumeSerialNumber,			// volume serial number
	  0,							// maximum file name length
	  &fileSystemFlags,			// file system options
	  0,							// file system name buffer
	  0								// length of file system name buffer
	);

	mask_year	= theTime.wYear;
	mask_year	= (( mask_year - 2000 ) << 29) & 0xE0000000;  // 3 bits of year

	mask_month	= theTime.wMonth;
	mask_month	= (( mask_month )		<< 25) & 0x1E000000;  // 4 bits of month

	mask_day	= theTime.wDay;
	mask_day	= (( mask_day )			<< 20) & 0x01F00000;  // 5 bits of day

	mask_hour	= theTime.wHour;
	mask_hour	= (( mask_hour )		<< 15) & 0x000F8000;  // 5 bits of hour

	mask_min	= theTime.wMinute;
	mask_min	= (( mask_min )			<<  9) & 0x00007E00;  // 6 bits of minute

	mask_sec	= theTime.wSecond;
	mask_sec	= (( mask_sec / 30)		<<  8) & 0x00000100;  // 1 bits of sec

#if 0 // we don't consider machine IDs any more
	// 8 bits of machine ID
//	volumeSerialNumber = 0x1cbae864;	//	(Jeremy)
//	volumeSerialNumber = 0x2c788364;	//	(Russ)
	volumeSerialNumber = 0xd840c356;	//	(Ken)
	
	machine_ID = 0;
	machine_ID = ((volumeSerialNumber & 0xFF000000) >> 24) ^ machine_ID;
	machine_ID = ((volumeSerialNumber & 0x00FF0000) >> 16) ^ machine_ID;
	machine_ID = ((volumeSerialNumber & 0x0000FF00) >>  8) ^ machine_ID;
	machine_ID =  (volumeSerialNumber & 0x000000FF)        ^ machine_ID;
#endif
					
	out_val = mask_year | mask_month | mask_day | mask_hour | mask_min | mask_sec ;

	printf("#define VERSION_BUILD_NUMBER_ 0x%0lX\n", out_val);


	return 0;
}


  


/*
		printf("Month: %d\n", (theTime.wMonth)*(60)*(24)*(12));
	printf("Day: %d\n", (theTime.wDay)*(60)*(24));
	printf("Hour: %d\n", (theTime.wHour)*(60));
	printf("Minute: %d\n", theTime.wMinute);

typedef struct _SYSTEMTIME { 
    WORD wYear; 
    WORD wMonth; 
    WORD wDayOfWeek; 
    WORD wDay; 
    WORD wHour; 
    WORD wMinute; 
    WORD wSecond; 
    WORD wMilliseconds; 
} SYSTEMTIME, *PSYSTEMTIME; 

*/
