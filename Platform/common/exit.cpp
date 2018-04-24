/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include <stdio.h>
extern "C" void _Exit();
extern "C" void exit(int);

void exit(int status)
{
	fflush(stdout);
	fflush(stderr);
	_Exit();
}

