#
#   $RCSfile$
#
#   Description: Makefile for compiling the 21161
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
# Additional suffixes
###########################################################
.SUFFIXES: .asm .doj
.asm.doj:
	${ASM} ${ASMFLAGS} ${DEFINES} ${subst -I,-i ,${INCLUDE_PATH}} `cygpath -w $<` -o $@


.cpp.doj:
	${CXX} -c ${CXXFLAGS} ${DEFINES} `cygpath -w $<` -o $@


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
PLATFORM=ADSP-21161
ASM			= EASM21K
ASMFLAGS	= -21161 -double-size-any -si-revision any -Wwarn-error
CXX			= ${UP_CXX} 
CXXFLAGS	= -21161 ${UP_CXXFLAGS} -workaround 21161-anomaly-45 -double-size-any -si-revision any
ifeq (${EXTRA_SUFFIX},_SWF)
ASMFLAGS        += -swf_screening_fix -D_ADI_SWFA -Wsuppress 2500
endif
SRC		= TimerInit.cpp IMASKInit.cpp Version.cpp
CORE_VECS	= $(shell ${TYPE} IntVectors.dat)
CORE_VECS2	= $(shell ${TYPE} IntVectors2.dat)
TMR_INTERRUPTS	= $(shell ${TYPE} TimerInt.dat)
CORE_ISRCS	= $(addsuffix .asm, $(CORE_VECS))
CORE_ISRCS2	= $(addsuffix .asm, $(CORE_VECS2))
CORE_IRQNAME= $(shell ${TYPE} IntIRQs.dat)
CORE_NOIRQNAME  = $(shell ${TYPE} IntNoIRQs.dat)
CORE_IRQS   = $(addsuffix .asm, $(CORE_IRQNAME))
CORE_NOIRQS = $(addsuffix .asm, $(CORE_NOIRQNAME))
CORE_CB7I   = seg_CB7I.asm
CORE_SRCS   = $(CORE_IRQS) $(CORE_NOIRQS) $(CORE_CB7I) VDK_hdr_SHARC.asm panic.asm adi_stack_overflow.asm
DEFINES		= ${UP_DEFINES} -D VERSION_PRODUCT_="VDK::k21161" -DVDK_INT_=INT_INT14_P
COMMA		= ,
ENDCOMMA	= ,,
ENDCODE         = \"+\",,
QUOTE		= \"
DOS_ENDL   = @cmd /c echo.
PLUS		= +
CORE_INAMES     = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA)) 
CORE_INAMES2     = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS2))$(COMMA)) 
TIMER_INTNAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(TMR_INTERRUPTS))$(COMMA)) 
CORE_HLIST	= $(filter-out $(ENDCOMMA), $(patsubst seg_%, __seg_% $(COMMA), $(CORE_VECS) )$(COMMA))
CORE_OBJS	= $(CORE_SRCS:.asm=.doj) ${SRC:.cpp=.doj}
VPATH		= ${CURDIR}:${UP_VPATH}
OUTPUT		= ${OUT_DIR}21161${EXTRA_SUFFIX}.dlb
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2116x/21161)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif

ifeq ($(findstring debug,$(OUTPUT)),debug)
CORE_OUTPUT	= ./debug/VDK-CORE-21161${EXTRA_SUFFIX}.dlb
INSTALL_TYPE= debug
else
CORE_OUTPUT	= ./release/VDK-CORE-21161${EXTRA_SUFFIX}.dlb
INSTALL_TYPE= release
endif


#
# Projects
#
all:		default
default:	$(CORE_OUTPUT) 

######
# Generate the core hooks
#

seg_int_hook.asm:	IntVectors.dat
	${ECHO} ".extern " >  $@
	${ECHO} "${CORE_HLIST} ;" 				>> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} ".SECTION/DM 	seg_int_hooks;" 	>>  $@
	${DOS_ENDL}                                             >> $@
	${ECHO} ".global __hook_seg_int_all;"	>> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} ".var __hook_seg_int_all[] =" 	>> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "${CORE_HLIST} ;" 				>> $@
	${DOS_ENDL}                                             >> $@

######
# Generate the version information. It will be different in all libs
#
 
ver_core.txt: 
	${ECHO} "" > $@
	${ECHO} "Library_Built: " >> $@
	@date +%c >>$@ 
	${ECHO}   "Library_Name: VDSP++ Kernel Library" >>$@ 
	${DOS_ENDL} >> $@
	${ECHO} "Library_Version: ${LIB_VERSION}"  >>$@ 
	${DOS_ENDL} >> $@
	${ECHO}  "Platform: ${PLATFORM}"   >> $@ 
	${DOS_ENDL} >> $@
	${ECHO}   "SpecialOptions: ${VEROPTIONS}"  >> $@ 
	${DOS_ENDL} >> $@
		
######
# Generate the .INI file for this part
#
FORCE:

ADSP-21161.ini: FORCE
	${ECHO} ""  >  $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "[ADSP-21161]" >> $@
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
	${ECHO} "Templates = 211xx\vdk\ "									>> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "HeapSupport = true"										>> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "LinkerFile = $(QUOTE)211xx\ldf\VDK-21161.ldf$(QUOTE)"               >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "Interrupts = $(QUOTE)$(CORE_INAMES)$(QUOTE)" >>$@
	${DOS_ENDL}                                             >> $@
	${ECHO} "Interrupts2 = $(QUOTE)$(CORE_INAMES2)$(QUOTE)" >>$@
	${DOS_ENDL}                                             >> $@
	${ECHO} "AllowedTimerInterrupts = $(QUOTE)$(TIMER_INTNAMES)$(QUOTE)" >>$@
	${DOS_ENDL}                                             >> $@
	${ECHO} "ClockFrequency = 100" >>$@
	${DOS_ENDL}                                             >> $@
	${ECHO} "TickPeriod = 0.5" >>$@
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

$(CORE_IRQS):   IntIRQs.dat
	${ECHO} "#include $(QUOTE)VDK_IntVector.h$(QUOTE)" > $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "INTVECTOR( $(basename $@), __InvalidIntIRQ )" >> $@
	${DOS_ENDL}                                             >> $@

$(CORE_NOIRQS): IntNoIRQs.dat
	${ECHO} "#include $(QUOTE)VDK_IntVector.h$(QUOTE)" > $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "INTVECTOR( $(basename $@), __InvalidInt )" >> $@
	${DOS_ENDL}                                             >> $@

$(CORE_CB7I):  FORCE
	${ECHO} "#include $(QUOTE)VDK_IntVector.h$(QUOTE)" > $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "INTVECTOR( seg_CB7I, _adi_stack_overflowed)" >> $@
	${DOS_ENDL}                                             >> $@

$(CORE_OBJS):	VDK_IntVector.h BuildNumber.h $(CORE_SRCS) 

$(CORE_OUTPUT):	 ${CORE_OBJS} seg_int_hook.doj VDK_ISR_API.doj ver_core.txt
	@${MKDIR} debug
	@${MKDIR} release
	elfar -tx ver_core.txt -c $(CORE_OUTPUT) ${CORE_OBJS} seg_int_hook.doj
	@echo "****************************************"
	@echo "$(CORE_OUTPUT) build completed at:"
	@date +%c
	@echo "****************************************"

clean:
	${RM} ADSP-21161.ini
	${RM} ${CORE_OBJS}
	${RM} *.ii
	${RM} ${CORE_CB7I}
	${RM} seg_int_hook.doj
	${RM} ver_core.txt

squeaky:
	${RM} ${CORE_OBJS}
	${RM} ${CORE_IRQS}
	${RM} ${CORE_NOIRQS}
	${RM} ADSP-21161.ini
	${RM} seg_int_hook.doj seg_int_hook.asm
	${RM} debug/VDK-*-21161*.dlb
	${RM} release/VDK-*-21161*.dlb
	${RM} ver_core.txt

install_debug: ADSP-21161.ini 
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/../211xx/ldf
	${MKDIR} ${INSTALL_DIR}/lib
	${CP} VDK-21161.ldf ${INSTALL_DIR}/../211xx/ldf
	${CP} ADSP-21161.ini ${INSTALL_DIR}/../211xx/vdk
	${CP} debug/VDK-*-21161*.dlb ${INSTALL_DIR}/lib

install:install_debug
	${CP} release/VDK-*-21161*.dlb ${INSTALL_DIR}/lib

dist_clean:
	${RM} ${INSTALL_DIR}/ldf/VDK-21161.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-21161.ini
	${RM} ${INSTALL_DIR}/lib/VDK-*-21161.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

