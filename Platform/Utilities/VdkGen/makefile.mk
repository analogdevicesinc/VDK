#
#   $RCSfile$
#
#   Description: Makefile for compiling VdkGen
#
#   Last modified $Date$
#	Last modified by $Author$
#   $Revision$
#   $Source$
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#  Copyright (c) 2001 Analog Devices, Inc.,  All rights reserved
#
#  This software is the property of Analog Devices, Inc.
#
#  ANALOG DEVICES (ADI) MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
#  REGARD TO THIS SOFTWARE.  IN NO EVENT SHALL ADI BE LIABLE FOR INCIDENTAL
#  OR CONSEQUENTIAL DAMAGES IN CONNECTION WITH OR ARISING FROM THE FURNISHING,
#  PERFORMANCE, OR USE OF THIS SOFTWARE.
#

RM=rm -f
RMDIR=rm -rf
MKDIR=mkdir

Release/VdkGen.exe:
	@echo "No need to rebuild VdkGen"

dist_clean:
