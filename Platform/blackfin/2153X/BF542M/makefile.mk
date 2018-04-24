#
#   $RCSfile$
#
#   Description: Makefile for compiling the BF542M
#
#   Last modified $Date$
#	Last modified by $Author$
#   $Revision$
#   $Source$ 
#
# -----------------------------------------------------------------------------
#  Comments: 
# -----------------------------------------------------------------------------
#  Copyright (c) 2008 Analog Devices, Inc.,  All rights reserved
#
#  This software is the property of Analog Devices, Inc.
#
#  ANALOG DEVICES (ADI) MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
#  REGARD TO THIS SOFTWARE.  IN NO EVENT SHALL ADI BE LIABLE FOR INCIDENTAL
#  OR CONSEQUENTIAL DAMAGES IN CONNECTION WITH OR ARISING FROM THE FURNISHING,
#  PERFORMANCE, OR USE OF THIS SOFTWARE.
#

###########################################################
# Default commands
###########################################################
RM=rm -f
CP=cp -p
RMDIR=rm -rf
MKDIR=mkdir -p
TYPE=cat
ECHO=@echo -n

###########################################################
# Setup the variables that are passed to sub calls of make
# These lines should change as they progress up the calls,
# but should basically remain the same from level to level
###########################################################
CORE_VECS	= $(shell ${TYPE} ../21535/IntVectors.dat)
COMMA		= ,
ENDCOMMA	= ,,
ENDCODE		= \"+\",,
QUOTE		=\"
DOS_ENDL   = @cmd /c echo.
PLUS		= +
CORE_INAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA)) 
CORE_HINAMES     = $(filter-out $(ENDCODE), $(patsubst seg_%,% $(QUOTE)$(PLUS)$(QUOTE)$(COMMA), $(CORE_VECS))$(COMMA)) 

######
# Generate the .INI file for this part
#
FORCE:

ADSP-BF542M.ini:FORCE
	${ECHO}   ""                   						>  $@
	${DOS_ENDL}						>> $@
	${ECHO} "[ADSP-BF542M]"								>> $@
	${DOS_ENDL}						>> $@
	${ECHO} ""                          				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Family = Blackfin"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "NaturalWordSize = 32"						>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "MaxUnsignedInt = 4294967295"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "MaxLong = 2147483647"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "MinLong = -2147483648"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Templates = $(QUOTE)blackfin\vdk\\$(QUOTE)"			>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "HeapSupport = true"						>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "LinkerFile = $(QUOTE)blackfin\ldf\VDK-BF542M.ldf$(QUOTE)"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Interrupts = $(QUOTE)$(CORE_INAMES)$(QUOTE)" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "ExceptionHandler = ExceptionHandler-BF542M.asm" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Template = VDK_NMI_Asm_Source.asm" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_C_Template = VDK_NMI_C_Source.c" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Cpp_Template = VDK_NMI_CPP_Source.cpp" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "AsmIsrCodeTemplate = BF542M_ISR_Asm_Source.asm"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "CIsrCodeTemplate = VDK_ISR_C_Source.c"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "CppIsrCodeTemplate = VDK_ISR_CPP_Source.cpp"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "ClockFrequency = 270" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "TickPeriod = 0.1" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "VdkVersion = 5.0" >>$@
	${DOS_ENDL}						>> $@	
	${ECHO} "IdleThreadStack = 256"				>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "DefaultTimerInterrupt = EVT_IVTMR"		>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "DefaultHistoryBufferSize = 256"		>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "NumContextAreas = 0" >>$@
	${DOS_ENDL}						>> $@	
	${ECHO} "NumThreadStacks = 1" >>$@
	${DOS_ENDL}						>> $@	
	${ECHO} "NumStackSizes = 1" 				>> $@
	${DOS_ENDL}						>> $@
	${ECHO}  ""                    				>> $@
	${DOS_ENDL}						>> $@

install: ADSP-BF542M.ini 
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/ldf
	${MKDIR} ${INSTALL_DIR}/lib
	${CP} VDK-BF542M.ldf ${INSTALL_DIR}/ldf
	${CP} ADSP-BF542M.ini ${INSTALL_DIR}/vdk
	
dist_clean:
	${RM} ${INSTALL_DIR}/ldf/VDK-BF542M.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-BF532.ini
	${RM} ${INT_VEC_SRCS}

squeaky: clean

clean:
	${RM}  ADSP-BF542M.ini

