#
#   $RCSfile$
#
#   Description: Makefile for compiling the RTOS
#
#   Last modified $Date$
#	Last modified by $Author$
#   $Revision$
#   $Source$ 
#
# -----------------------------------------------------------------------------
#  Comments: contains the most basic RTOS definitions
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

ifeq (${VSINSTALLDIR},)
  MSVS  = "${MSDEVDIR}/Bin/MSDev"
  OPTIONS       = /make 
  PROJECT_FILE = internals_generator.dsw
else
  ifeq (${VSTUDIO},2005_EXPRESS)
    MSVS    = "$(DEVENVDIR)/VCExpress.exe"
    OPTIONS       = /build Debug
    PROJECT_FILE = internals_generator_2005.sln
  else
    ifeq (${VSTUDIO},2005_FULL)
      MSVS    = "$(DEVENVDIR)/devenv.exe"
      OPTIONS       = /build Debug
      PROJECT_FILE = internals_generator_2005.sln
    else
      MSVS    = "$(VSINSTALLDIR)/devenv.exe"
      OPTIONS       = /build "Debug"
      PROJECT_FILE = internals_generator.sln
    endif
  endif
endif


Debug/internals_generator.exe:	internals_generator.dsp
Debug/internals_generator.exe:	internals_generator.cpp
Debug/internals_generator.exe:	CmdLine_Parser.cpp
Debug/internals_generator.exe:	CmdLine_Parser.h
Debug/internals_generator.exe:	${PROJECT_FILE}
Debug/internals_generator.exe:	
	echo ${MSVS} ${TARGET} ${PROJECT_FILE} ${OPTIONS}
	${MSVS} ${TARGET} ${PROJECT_FILE} ${OPTIONS}

dist_clean:
	${RM} debug/*
	${RMDIR} debug
	${RM} *.plg

