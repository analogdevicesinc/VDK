#
#   $RCSfile$
#
#   Description: Makefile for compiling BuildNum - the build number generator
#
#   Last modified $Date $
#	Last modified by $Author $
#   $Revision$
#   $Source $ 
#
# -----------------------------------------------------------------------------
#  Comments: makes BuildNum - the build number generator
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
CP=cp 
RMDIR=rm -rf
MKDIR=mkdir -p
TYPE=cat

ifeq (${VSINSTALLDIR},)
  MSVS  = "${MSDEVDIR}/Bin/MSDev"
  OPTIONS       = /make 
  PROJECT_FILE = BuildNum.dsw
else
  MSVS    = "$(VSINSTALLDIR)/devenv.exe"
  OPTIONS       = /build "Debug"
  PROJECT_FILE = BuildNum.sln
endif

Debug/BuildNum.exe:	BuildNum.dsp
Debug/BuildNum.exe:	BuildNum.cpp
Debug/BuildNum.exe:	StdAfx.cpp
Debug/BuildNum.exe:	StdAfx.h
Debug/BuildNum.exe:	${PROJECT_FILE}
Debug/BuildNum.exe:	
	${MSVS} ${TARGET} ${PROJECT_FILE} ${OPTIONS}

dist_clean: FORCE
	${RM} debug/*
	${RMDIR} debug
	${RM} *.plg

FORCE:


