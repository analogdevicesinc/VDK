#
#   $RCSfile$
#
#   Description: Makefile for compiling the 21267
#
#   Last modified $Date$
#	Last modified by $Author$
#   $Revision$
#   $Source$ 
#
# -----------------------------------------------------------------------------
#  Comments: 
# -----------------------------------------------------------------------------
#  Copyright (c) 2003 Analog Devices, Inc.,  All rights reserved
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
COMMA		= ,
PLUS		= +
ENDCOMMA	= ,,
ENDCODE		= \"+\",,
QUOTE		= \"
DOS_ENDL   = @cmd /c echo.
CORE_VECS       = $(shell ${TYPE} ../21261/IntVectors.dat)
CORE_VECS2       = $(shell ${TYPE} ../21261/IntVectors2.dat)
TMR_INTERRUPTS	= $(shell ${TYPE} ../21261/TimerInt.dat)
CORE_INAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA)) 
CORE_INAMES2 = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS2))$(COMMA)) 
TIMER_INTNAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(TMR_INTERRUPTS))$(COMMA)) 


#
# Projects
#
.PHONY:		all default
all:		default
default:	${CORE_OUTPUT} 


######
# Generate the .INI file for this part
#
FORCE:

ADSP-21267.ini:FORCE
	${ECHO} ""  >  $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "[ADSP-21267]" >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} ""	                                                  >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "Family = SHARC" >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "NaturalWordSize = 32" >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "MaxUnsignedInt = 4294967295"	                       >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "MaxLong = 2147483647"                          >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "MinLong = -2147483648"                         >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "Templates = 212xx\vdk\ "									>> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "HeapSupport = true"										>> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "LinkerFile = $(QUOTE)212xx\ldf\VDK-21267.ldf$(QUOTE)"	       >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "Interrupts = $(QUOTE)$(CORE_INAMES)$(QUOTE)" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "Interrupts2 = $(QUOTE)$(CORE_INAMES2)$(QUOTE)" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "AllowedTimerInterrupts = $(QUOTE)$(TIMER_INTNAMES)$(QUOTE)" >>$@
	${DOS_ENDL}	>> $@
	${ECHO} "ClockFrequency = 200" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "TickPeriod = 0.2" >>$@
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
	${DOS_ENDL}	                                     >> $@
	${ECHO} "VdkVersion = 5.0" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} ""	                                                  >> $@
	${DOS_ENDL}	                                     >> $@

clean:
	${RM} ADSP-21267.ini

squeaky:clean

install_debug: ADSP-21267.ini 
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/../212xx/ldf
	${MKDIR} ${INSTALL_DIR}/lib
	${CP} VDK-21267.ldf ${INSTALL_DIR}/../212xx/ldf
	${CP} ADSP-21267.ini ${INSTALL_DIR}/../212xx/vdk

install:install_debug

dist_clean:
	${RM} ${INSTALL_DIR}/ldf/VDK-21267.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-21267.ini
