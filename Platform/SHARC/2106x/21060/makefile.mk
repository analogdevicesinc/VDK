#
#   $RCSfile$
#
#   Description: Makefile for compiling the 21060
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
DOS_ENDL   = @cmd /c echo.


###########################################################
# Setup the variables that are passed to sub calls of make
# These lines should change as they progress up the calls,
# but should basically remain the same from level to level
###########################################################
PLATFORM=ADSP-21060
ASM			= EASM21K
ifeq (${VDSP_VERSION},4.0)
ASMFLAGS	= -21060 
else
ASMFLAGS	= -21060 -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30 -double-size-any -si-revision any -Wwarn-error
endif
CXX			= ${UP_CXX} 
CXXFLAGS	= ${UP_CXXFLAGS} -double-size-any -proc ADSP-21060 -si-revision any
SRC		= TimerInit.cpp IMASKInit.cpp Version.cpp
CORE_VECS	= $(shell ${TYPE} IntVectors.dat)
CORE_IRQNAME= $(shell ${TYPE} IntIRQs.dat)
CORE_NOIRQNAME	= $(shell ${TYPE} IntNoIRQs.dat)
CORE_IRQS	= $(addsuffix .asm, $(CORE_IRQNAME))
CORE_NOIRQS	= $(addsuffix .asm, $(CORE_NOIRQNAME))
CORE_CB7I   = seg_CB7I.asm
CORE_SRCS	= $(CORE_IRQS) $(CORE_NOIRQS) $(CORE_CB7I) VDK_hdr_SHARC.asm panic.asm adi_stack_overflow.asm
DEFINES		= ${UP_DEFINES} -D VERSION_PRODUCT_="VDK::k21060"
COMMA		= ,
ENDCOMMA	= ,,
ENDCODE		= \"+\",,
QUOTE		= \"
DOS_ENDL   = @cmd /c echo.
PLUS		= +
CORE_INAMES     = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA)) 
CORE_HINAMES     = $(filter-out $(ENDCODE), $(patsubst seg_%,% $(QUOTE)$(PLUS)$(QUOTE)$(COMMA), $(CORE_VECS))$(COMMA)) 
CORE_HLIST	= $(filter-out $(ENDCOMMA), $(patsubst seg_%, __seg_% $(COMMA), $(CORE_VECS) )$(COMMA))
CORE_OBJS	= $(CORE_SRCS:.asm=.doj) ${SRC:.cpp=.doj}
VPATH		= ${CURDIR}:${UP_VPATH}
OUTPUT		= ${OUT_DIR}21060.dlb
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2106x/21060)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
VDKGEN          = ../../../Utilities/VdkGen/Release/VdkGen.exe
CURR_DIR=${subst \,\\,${shell cygpath -w `pwd`}}
EXAMPLES_DIR = Examples_35

ifeq (${VDSP_VERSION},5.0)
VDKGEN_FLAGS = -proc ADSP-21060 ++vdkgen_for_rt ++VDSP_5.0 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
else
ifeq (${VDSP_VERSION},4.5)
VDKGEN_FLAGS = -proc ADSP-21060 ++vdkgen_for_rt ++VDSP_4.5 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
else
VDKGEN_FLAGS = -proc ADSP-21060 ++vdkgen_for_rt ++VDSP_4.0 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
endif
endif
VDK_FILES_LIST= ${subst /,\\,${shell ls -1 ${EXAMPLES_DIR}/*/*.vdk}}
EXAMPLES_LIST   = ${shell find ${EXAMPLES_DIR}/* -type d \! -name CVS -prune -exec ls -1d {} \;}

ifeq ($(findstring debug,$(OUTPUT)),debug)
CORE_OUTPUT	= ./debug/VDK-CORE-21060.dlb
INSTALL_TYPE= debug
else
CORE_OUTPUT	= ./release/VDK-CORE-21060.dlb
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
	${ECHO} " .extern "	> $@
	${ECHO} " ${CORE_HLIST} ;" 			  >> $@
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
FORCE:

ADSP-21060.ini: FORCE
	${ECHO} ""  >  $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "[ADSP-21060]" >> $@
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
	${ECHO} "Templates = 21k\vdk\ "			>> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "HeapSupport = true"						>> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "LinkerFile = $(QUOTE)21k\ldf\VDK-21060.ldf$(QUOTE)"               >> $@
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
	${DOS_ENDL}						>> $@
	${ECHO} "VdkVersion = 5.0" >>$@
	${DOS_ENDL}                                             >> $@
	${ECHO} ""                                                          >> $@
	${DOS_ENDL}                                             >> $@

$(CORE_IRQS):	IntIRQs.dat
	${ECHO} "#include $(QUOTE)VDK_IntVector.h$(QUOTE)" > $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "INTVECTOR( $(basename $@), __InvalidIntIRQ )" >> $@
	${DOS_ENDL}                                             >> $@

$(CORE_NOIRQS):	IntNoIRQs.dat
	${ECHO} "#include $(QUOTE)VDK_IntVector.h$(QUOTE)" > $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "INTVECTOR( $(basename $@), __InvalidInt )" >> $@
	${DOS_ENDL}                                             >> $@

$(CORE_CB7I):  FORCE
	${ECHO} "#include $(QUOTE)VDK_IntVector.h$(QUOTE)" > $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "INTVECTOR( seg_CB7I, _adi_stack_overflowed)" >> $@
	${DOS_ENDL}                                             >> $@

$(CORE_OBJS):	VDK_IntVector.h  BuildNumber.h $(CORE_SRCS) 

$(CORE_OUTPUT):	${CORE_OBJS} seg_int_hook.doj VDK_ISR_API.doj ver_core.txt
	${MKDIR} release debug
	elfar -tx ver_core.txt -c $(CORE_OUTPUT) ${CORE_OBJS} seg_int_hook.doj
	@echo "****************************************"
	@echo "$(CORE_OUTPUT) build completed at:"
	@date +%c
	@echo "****************************************"

clean:
	${RM} ${CORE_OBJS}
	${RM} ${CORE_IRQS}
	${RM} ${CORE_NOIRQS}
	${RM} ${CORE_CB7I}
	${RM} seg_int_hook.doj seg_int_hook.asm
	${RM} *.ii
	${RM} *.ti
	${RM} ADSP-21060.ini
	${RM} ver_core.txt

squeaky: clean
	${RM} debug/VDK-*-21060.dlb
	${RM} release/VDK-*-21060.dlb
	@echo "**** CLEANING EXAMPLES ****"
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK-21060.ldf; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK.cpp; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK.h; }

${VDKGEN}:
	"${MAKE}" -fmakefile.mk -C../../../Utilities/VdkGen
	@echo "**************************************"
	@echo "VdkGen Built"
	@echo "**************************************"

install_debug: ADSP-21060.ini ${VDKGEN}
	"${MAKE}" -fmakefile.mk -C../../../Utilities/VdkGen
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/ldf
	${MKDIR} ${INSTALL_DIR}/lib
	${CP} VDK-21060.ldf ${INSTALL_DIR}/ldf
	${CP} ADSP-21060.ini ${INSTALL_DIR}/vdk
	${CP} debug/VDK-*-21060.dlb ${INSTALL_DIR}/lib
	${CP} ${VDKGEN} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/Examples/No\ Hardware\ Required/VDK/21060
	@echo "**************Creating LDF Vdk.cpp and Vdk.h files for examples ***********"
	@echo ""
	${foreach example_dir,${EXAMPLES_LIST}, cp VDK-21060.ldf ${example_dir}; }
	${foreach the_kernel_file,${VDK_FILES_LIST}, ${INSTALL_DIR}/vdk/Vdkgen.exe ${VDKGEN_FLAGS} '$(shell cygpath -w ${CURR_DIR}/${the_kernel_file})'; }
	${RM} ${EXAMPLES_DIR}/MultipleHeaps/VDK-21060.ldf
	${foreach example_dir,${EXAMPLES_LIST}, ${RM} ${example_dir}/*.rbld; }
	@echo "**************LDF Vdk.cpp and Vdk.h for examples created ***********"
	"${MAKE}" -i -f../../../../common/examples/makefile.mk -C${EXAMPLES_DIR} UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/"No\ Hardware\ Required"/VDK/21060 install
install:install_debug
	${CP} release/VDK-*-21060.dlb ${INSTALL_DIR}/lib
	
dist_clean:
	"${MAKE}" -i -f../../../../common/examples/makefile.mk -C${EXAMPLES_DIR} UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/"No\ Hardware\ Required"/VDK/21060 dist_clean
	${RM} ${INSTALL_DIR}/ldf/VDK-21060.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-21060.ini
	${RM} ${INSTALL_DIR}/lib/VDK-*-21060.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

