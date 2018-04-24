#
#   $RCSfile$
#
#   Description: Makefile for compiling the 21160
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
PLATFORM=ADSP-21160
ASM			= EASM21K
ifeq (${VDSP_VERSION},4.0)
ASMFLAGS	= -21160
else
ASMFLAGS	= -21160 -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30 -double-size-any -si-revision any -Wwarn-error
endif
CXX			= ${UP_CXX} 
CXXFLAGS	= -proc ADSP-21160 -workaround rframe ${UP_CXXFLAGS} -double-size-any -si-revision any
ifeq (${EXTRA_SUFFIX},_SWF)
ASMFLAGS        += -swf_screening_fix -D_ADI_SWFA -Wsuppress 2500
endif
SRC		= TimerInit.cpp IMASKInit.cpp Version.cpp
CORE_VECS	= $(shell ${TYPE} IntVectors.dat)
CORE_VECS2	= $(shell ${TYPE} IntVectors2.dat)
CORE_IRQNAME= $(shell ${TYPE} IntIRQs.dat)
CORE_NOIRQNAME  = $(shell ${TYPE} IntNoIRQs.dat)
CORE_IRQS   = $(addsuffix .asm, $(CORE_IRQNAME))
CORE_NOIRQS = $(addsuffix .asm, $(CORE_NOIRQNAME))
CORE_CB7I   = seg_CB7I.asm
CORE_SRCS   = $(CORE_IRQS) $(CORE_NOIRQS) $(CORE_CB7I) VDK_hdr_SHARC.asm panic.asm adi_stack_overflow.asm
TMR_INTERRUPTS	= $(shell ${TYPE} TimerInt.dat)
CORE_ISRCS	= $(addsuffix .asm, $(CORE_VECS))
DEFINES		= ${UP_DEFINES} -D VERSION_PRODUCT_="VDK::k21160" -DVDK_INT_=INT_INT14_P
COMMA		= ,
PLUS		= +
ENDCOMMA	= ,,
ENDCODE		= \"+\",,
QUOTE		= \"
DOS_ENDL   = @cmd /c echo.
CORE_INAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA)) 
CORE_INAMES2 = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS2))$(COMMA)) 
TIMER_INTNAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(TMR_INTERRUPTS))$(COMMA)) 
CORE_HLIST	= $(filter-out $(ENDCOMMA), $(patsubst seg_%, __seg_% $(COMMA), $(CORE_VECS) )$(COMMA))
CORE_OBJS	= $(CORE_SRCS:.asm=.doj) ${SRC:.cpp=.doj}
VPATH		= ${CURDIR}:${UP_VPATH}
OUTPUT		= ${OUT_DIR}21160${EXTRA_SUFFIX}.dlb

ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2116x/21160)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
VDKGEN	  = ../../../Utilities/VdkGen/Release/VdkGen.exe
CURR_DIR=${subst \,\\,${shell cygpath -w `pwd`}}
EXAMPLES_DIR = Examples_35

ifeq (${VDSP_VERSION},5.0)
VDKGEN_FLAGS = -proc ADSP-21160 ++vdkgen_for_rt ++VDSP_5.0 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
else
ifeq (${VDSP_VERSION},4.5)
VDKGEN_FLAGS = -proc ADSP-21160 ++vdkgen_for_rt ++VDSP_4.5 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
else
VDKGEN_FLAGS = -proc ADSP-21160 ++vdkgen_for_rt ++VDSP_4.0 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
endif
endif

VDK_FILES_LIST= ${subst /,\\,${shell ls -1 Examples_35/*/*.vdk}} ${subst /,\\,${shell ls -1 Examples_35/*/Core?/*.vdk}} ${subst /,\\,${shell ls -1 Examples_35/*/*/Core?/*.vdk}}
EXAMPLES_LIST   = ${shell find ${EXAMPLES_DIR}/* -type d \! -name CVS -prune -exec ls -1d {} \;}
MP_EXAMPLES_LIST = ${shell find Examples_35/Payload_Marshalling/* Examples_35/*MP_* Examples_35/*_MP* -type d \! -name CVS -prune -exec ls -1d {} \;}


ifeq ($(findstring debug,$(OUTPUT)),debug)
CORE_OUTPUT	= ./debug/VDK-CORE-21160${EXTRA_SUFFIX}.dlb
INSTALL_TYPE= debug
else
CORE_OUTPUT	= ./release/VDK-CORE-21160${EXTRA_SUFFIX}.dlb
INSTALL_TYPE= release
endif

#
# Projects
#
.PHONY:		all default
all:		default
default:	${CORE_OUTPUT} 


seg_int_hook.asm:	IntVectors.dat 
	${ECHO} " .extern "	> $@
	${ECHO} " ${CORE_HLIST} ;" 			  >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} ".SECTION/DM 	seg_int_hooks;" 	>>  $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} ".global __hook_seg_int_all;"	>> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} ".var __hook_seg_int_all[] =" 	>> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "${CORE_HLIST} ;" 				>> $@
	${DOS_ENDL}	                                     >> $@

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

ADSP-21160.ini:FORCE
	${ECHO} ""  >  $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "[ADSP-21160]" >> $@
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
	${ECHO} "Templates = 211xx\vdk\ "									>> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "HeapSupport = true"										>> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "LinkerFile = $(QUOTE)211xx\ldf\VDK-21160.ldf$(QUOTE)"	       >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "Interrupts = $(QUOTE)$(CORE_INAMES)$(QUOTE)" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "Interrupts2 = $(QUOTE)$(CORE_INAMES2)$(QUOTE)" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "AllowedTimerInterrupts = $(QUOTE)$(TIMER_INTNAMES)$(QUOTE)" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "ClockFrequency = 80" >>$@
	${DOS_ENDL}	                                     >> $@
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
	${DOS_ENDL}	                                     >> $@
	${ECHO} "VdkVersion = 5.0" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} ""	                                                  >> $@
	${DOS_ENDL}	                                     >> $@

$(CORE_IRQS):	IntIRQs.dat
	${ECHO} "#include $(QUOTE)VDK_IntVector.h$(QUOTE)" > $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "/* disable incorrect warning 2500 in swf libraries. Possible Shadow Write FIFO Anomaly Detected. Assembler ignores delay branch */ " >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} ".MESSAGE/SUPPRESS 2500; " >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "INTVECTOR( $(basename $@), __InvalidIntIRQ )" >> $@
	${DOS_ENDL}                                             >> $@

$(CORE_NOIRQS):	IntNoIRQs.dat
	${ECHO} "#include $(QUOTE)VDK_IntVector.h$(QUOTE)" > $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "/* disable incorrect warning 2500 in swf libraries. Possible Shadow Write FIFO Anomaly Detected. Assembler ignores delay branch */ " >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} ".MESSAGE/SUPPRESS 2500; " >> $@
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
	@echo "--------------------"
	@echo "$(CORE_OUTPUT) build completed at:"
	@date +%c
	@echo "--------------------"

clean:
	${RM} ADSP-21160.ini
	${RM} ${CORE_OBJS}
	${RM} ${CORE_CB7I}
	${RM} seg_int_hook.doj 
	${RM} ver_core.txt

squeaky: clean
	${RM} *.ii
	${RM} *.ti
	${RM} ${CORE_OBJS}
	${RM} ${CORE_IRQS}
	${RM} ${CORE_NOIRQS}
	${RM} ADSP-21160.ini
	${RM} seg_int_hook.doj seg_int_hook.asm
	${RM} debug/VDK-*-21160*.dlb
	${RM} release/VDK-*-21160*.dlb
	@echo "**** CLEANING EXAMPLES ****"
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK-21160.ldf; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK.cpp; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK.h; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreA/VDK-21160.ldf; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreA/VDK.cpp; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreA/VDK.h; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreB/VDK-21160.ldf; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreB/VDK.cpp; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreB/VDK.h; }

${VDKGEN}:
	"${MAKE}" -fmakefile.mk -C../../../Utilities/VdkGen
	@echo "**************************************"
	@echo "VdkGen Built"
	@echo "**************************************"

install_debug: ADSP-21160.ini ${VDKGEN}
	"${MAKE}" -fmakefile.mk -C../../../Utilities/VdkGen
	${MKDIR} ${INSTALL_DIR}/../211xx/ldf
	${MKDIR} ${INSTALL_DIR}/lib
	${MKDIR} ${INSTALL_DIR}/vdk
	${CP} VDK-21160.ldf ${INSTALL_DIR}/../211xx/ldf
	${CP} ADSP-21160.ini ${INSTALL_DIR}/../211xx/vdk
	${CP} debug/VDK-*-21160*.dlb ${INSTALL_DIR}/lib
	${CP} ${VDKGEN} ${INSTALL_DIR}/vdk
	@echo "**************Creating LDF Vdk.cpp and Vdk.h files for examples ***********"
	@echo ""
	${foreach example_dir,${EXAMPLES_LIST}, cp VDK-21160.ldf ${example_dir}; }
	${foreach the_kernel_file,${VDK_FILES_LIST}, ${INSTALL_DIR}/vdk/Vdkgen.exe ${VDKGEN_FLAGS} '$(shell cygpath -w ${CURR_DIR}/${the_kernel_file})'; }
	${RM} ${EXAMPLES_DIR}/MultipleHeaps/VDK-21160.ldf
	# MP messages require the LDFs in a different directory
	${RM} ${EXAMPLES_DIR}/Payload_Marshalling/VDK-21160.ldf
	${RM} ${EXAMPLES_DIR}/*MP_*/VDK-21160.ldf
	${RM} ${EXAMPLES_DIR}/*_MP*/VDK-21160.ldf
	${foreach example_dir,${MP_EXAMPLES_LIST}, cp VDK-21160.ldf ${example_dir}/CoreA; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, cp VDK-21160.ldf ${example_dir}/CoreB; }

	${RM} ${EXAMPLES_DIR}/MP_Framework/VDK-21160.ldf

	#Remove the rbld files
	${foreach example_dir,${EXAMPLES_LIST}, ${RM} ${example_dir}/*.rbld; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, ${RM} ${example_dir}/Core?/*.rbld; }
	@echo "**************LDF Vdk.cpp and Vdk.h for examples created ***********"
	"${MAKE}" -i -f../../../../common/examples/makefile.mk -C${EXAMPLES_DIR} UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/"No\ Hardware\ Required"/VDK/21160 install
	${MKDIR} ${INSTALL_DIR}/Examples/ADSP-21160\ EZ-KIT\ Lite/VDK
	${RMDIR} ${INSTALL_DIR}/Examples/ADSP-21160\ EZ-KIT\ Lite/VDK/*MP*
	mv -f ${INSTALL_DIR}/Examples/No\ Hardware\ Required/VDK/21160/*MP* ${INSTALL_DIR}/Examples/ADSP-21160\ EZ-KIT\ Lite/VDK
	${RMDIR} ${INSTALL_DIR}/Examples/ADSP-21160\ EZ-KIT\ Lite/VDK/Payload_Marshalling
	mv -f ${INSTALL_DIR}/Examples/No\ Hardware\ Required/VDK/21160/Payload_Marshalling ${INSTALL_DIR}/Examples/ADSP-21160\ EZ-KIT\ Lite/VDK



install:install_debug
	${CP} release/VDK-*-21160*.dlb ${INSTALL_DIR}/lib

dist_clean:
	"${MAKE}" -i -f../../../../common/examples/makefile.mk -C${EXAMPLES_DIR} UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/"No\ Hardware\ Required"/VDK/21160 dist_clean
	${RM} ${INSTALL_DIR}/ldf/VDK-21160.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-21160.ini
	${RM} ${INSTALL_DIR}/lib/VDK-*-21160*.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release


