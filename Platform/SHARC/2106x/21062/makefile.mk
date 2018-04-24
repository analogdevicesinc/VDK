#
#   $RCSfile$
#
#   Description: Makefile for compiling the 21062
#
#   Last modified $Date$
#	Last modified by $Author$
#   $Revision$
#   $Source$ 
#
# -----------------------------------------------------------------------------
#  Comments: 
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

###########################################################
# Default commands
###########################################################
RM=rm -f
CP=cp -p
RMDIR=rm -rf
MKDIR=mkdir -p
TYPE=cat
ECHO=@echo -n

CORE_VECS       = $(shell ${TYPE} ../21060/IntVectors.dat)
COMMA           = ,
ENDCOMMA        = ,,
ENDCODE		= \"+\",,
QUOTE           = \"
DOS_ENDL   = @cmd /c echo.
PLUS		= +
CORE_INAMES     = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA))
CORE_HINAMES	= $(filter-out $(ENDCODE), $(patsubst seg_%,% $(QUOTE)$(PLUS)$(QUOTE)$(COMMA), $(CORE_VECS))$(COMMA))


###########################################################
#
# Projects
#

######
# Generate the .INI file for this part
#
FORCE:

ADSP-21062.ini: FORCE
	${ECHO} ""  >  $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "[ADSP-21062]" >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} ""                                                          >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "Family = SHARC" >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "NaturalWordSize = 32" >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "MaxUnsignedInt = 4294967295"                               >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "MaxLong = 2147483647"                          >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "MinLong = -2147483648"                         >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "Templates = 21k\vdk\ "									>> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "HeapSupport = true"										>> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "LinkerFile = $(QUOTE)21k\ldf\VDK-21062.ldf$(QUOTE)"               >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "Interrupts = $(QUOTE)$(CORE_INAMES)$(QUOTE)" >>$@
	${DOS_ENDL}                                             >> $@
	${ECHO} "ClockFrequency = 40" >>$@
	${DOS_ENDL}                                             >> $@
	${ECHO} "TickPeriod = 1" >>$@
	${DOS_ENDL}                                             >> $@
	${ECHO} "IdleThreadStack = 256"				>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "DefaultTimerInterrupt = TMZHI"		>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "DefaultHistoryBufferSize = 256"		>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "NumContextAreas = 1" 				>>$@
	${DOS_ENDL}						>> $@	
	${ECHO} "NumThreadStacks = 1" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "NumStackSizes = 1" >>$@
	${DOS_ENDL}                                             >> $@
	${ECHO} "VdkVersion = 5.0" >>$@
	${DOS_ENDL}                                             >> $@
	${ECHO} ""                                                          >> $@
	${DOS_ENDL}                                             >> $@

clean:
	${RM} ADSP-21062.ini

squeaky: clean

install_debug: ADSP-21062.ini 
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/ldf
	${CP} VDK-21062.ldf ${INSTALL_DIR}/ldf
	${CP} ADSP-21062.ini ${INSTALL_DIR}/vdk

install:install_debug

dist_clean:
	${RM} ${INSTALL_DIR}/ldf/VDK-21062.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-21062.ini
	${RM} ${INSTALL_DIR}/lib/VDK-*-21060.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

