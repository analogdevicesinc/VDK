#
#   $RCSfile$
#
#   Description: Makefile for compiling the BF561
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
# Additional suffixes
###########################################################
.SUFFIXES: .asm .doj
.asm.doj:
	${ASM} ${DEFINES} ${ASMFLAGS} ${subst -I,-i ,${INCLUDE_PATH}} `cygpath -w $<` -o $@

.cpp.doj:
	${CXX} ${DEFINES} -c ${CXXFLAGS} `cygpath -w $<` -o $@

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
VPATH		= ${CURDIR}:${UP_VPATH}
ASM			= EASMBLKFN
ASMFLAGS	= -proc ADSP-BF561 ${UP_ASMFLAGS} 
CXX			= ${UP_CXX}
CXXFLAGS	= -proc ADSP-BF561 ${UP_CXXFLAGS} ${INCLUDE_PATH} 
SRC			= TimerInit.cpp Version.cpp InitPlatform.cpp
ifeq (${NESTED_MAKE}, YES)
CORE_VECS	= $(shell ${TYPE} ../../21535/IntVectors.dat)
else
CORE_VECS	= $(shell ${TYPE} ../../21535/IntVectors.dat)
endif
CORE_SRCS	= vdk_hdr_bf53x.asm panic.asm ExceptionPanic.asm InterruptPanic.asm adi_stack_overflow.asm
OBJS		= ${UP_OBJS}
ifeq (${NESTED_MAKE}, YES)
DEFINES		= ${UP_DEFINES} 
else
DEFINES		= ${UP_DEFINES} -DVERSION_PRODUCT_="VDK::kBF561"
endif

CORE_VECS	= $(shell ${TYPE} ../21535/IntVectors.dat)
COMMA		= ,
ENDCOMMA	= ,,
ENDCODE		= \"+\",,
QUOTE		=\"
DOS_ENDL   = @cmd /c echo.
PLUS		= +
CORE_INAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA)) 
CORE_OBJS	= $(CORE_SRCS:.asm=.doj) ${SRC:.cpp=.doj} 
PLATFORM    = ADSP-BF561
VERS_DATE = $(shell date  +%a%b%e%Y%T")
OUTPUT		= ${OUT_DIR}BF561.dlb
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/blackfin/2153X/BF561)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
CORE_HINAMES     = $(filter-out $(ENDCODE), $(patsubst seg_%,% $(QUOTE)$(PLUS)$(QUOTE)$(COMMA), $(CORE_VECS))$(COMMA)) 
VDKGEN          = ../../../Utilities/VdkGen/Release/VdkGen.exe
CURR_DIR        = ${subst \,\\,${shell cygpath -w `pwd`}}

ifeq (${VDSP_VERSION},5.0)
VDKGEN_FLAGS = -proc ADSP-BF561 ++vdkgen_for_rt ++VDSP_5.0 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
else
ifeq (${VDSP_VERSION},4.5)
VDKGEN_FLAGS = -proc ADSP-BF561 ++vdkgen_for_rt ++VDSP_4.5 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
else
VDKGEN_FLAGS = -proc ADSP-BF561 ++vdkgen_for_rt ++VDSP_4.0 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
endif
endif

EXAMPLES_DIR =Examples_35

VDK_FILES_LIST  = ${subst /,\\,${shell ls -1 ${EXAMPLES_DIR}/*/*/*.vdk}} ${subst /,\\,${shell ls -1 ${EXAMPLES_DIR}/*/*/*/*.vdk}}
EXAMPLES_LIST   = ${shell find ${EXAMPLES_DIR}/* -type d \! -name CVS -prune -type d \! -name Produce_Consumer_MP -prune  -exec ls -1d {} \;}
MP_EXAMPLES_LIST = ${shell find Examples_35/Payload_Marshalling/* Examples_35/*MP_* Examples_35/*_MP* -type d \! -name CVS -prune -type d \! -name Produce_Consumer_MP -prune -exec ls -1d {} \;}
CURR_DIR	= ${subst \,\\,${shell cygpath -w `pwd`}}
NEW_FLAGS       = UP_CXX="${CXX}"                                       \
                          UP_CXXFLAGS="${UP_CXXFLAGS}"          \
                          UP_ASMFLAGS="${ASMFLAGS}"             \
                          TOP_DIR="${TOP_DIR}"                           \
                          UP_VPATH="${VPATH}"                           \
                          OUT_DIR="${OUT_DIR}"                          \
                          FAMILY="${FAMILY}"                            \
                          UP_DEFINES="${DEFINES}"                       \
                          REVISION="${REVISION}"                        \
                          CORE="${CORE}" \
                          DEVICE="${DEVICE}"                            \
                          UP_INCLUDE_PATH="${INCLUDE_PATH}"     \
                          API_VERSION="${API_VERSION}"     \
                          LIB_VERSION="${LIB_VERSION}"     \
                          VDSP_VERSION="${VDSP_VERSION}"


ifeq ($(findstring debug,$(OUTPUT)),debug)
CORE_OUTPUT	= ./debug/VDK-CORE-BF561.dlb
INSTALL_TYPE= debug
else
CORE_OUTPUT	= ./release/VDK-CORE-BF561.dlb
INSTALL_TYPE= release
endif

#
# Projects
#
all:		default
default:	build_BF561
squeaky: squeaky_current_dir clean
build_libs:	$(OUTPUT) $(CORE_OUTPUT)

# Revision 0.3 used to be BF566 directory but not anymore
ifeq (${REVISION},all)
build_BF561: build_BF561_0_2 build_BF561_0_3 build_BF561_0_5 build_BF561_none build_BF561_any
clean: clean_current_dir clean_BF561_0_2 clean_BF561_0_3 clean_BF561_0_5 clean_BF561_none clean_BF561_any
install: install_examples install_BF561_0_2 install_BF561_0_3 install_BF561_0_5 install_BF561_none install_BF561_any
install_debug: install_examples install_debug_BF561_0_2 install_debug_BF561_0_3 install_debug_BF561_0_5 install_debug_BF561_none install_debug_BF561_any
else
ifeq (${REVISION},0.2)
build_BF561: build_BF561_0_2
clean: clean_current_dir clean_BF561_0_2
install: install_examples install_BF561_0_2
install_debug: install_examples install_debug_BF561_0_2
else
ifeq (${REVISION},0.3)
build_BF561: build_BF561_0_3
clean: clean_current_dir clean_BF561_0_3
install: install_examples install_BF561_0_3
install_debug: install_examples install_debug_BF561_0_3
else
ifeq (${REVISION},0.5)
build_BF561: build_BF561_0_5
clean: clean_current_dir clean_BF561_0_5
install: install_examples install_BF561_0_5
install_debug: install_examples install_debug_BF561_0_5
else
ifeq (${REVISION},any)
build_BF561: build_BF561_any
clean: clean_current_dir clean_BF561_any
install: install_examples install_BF561_any
install_debug: install_examples install_debug_BF561_any
else
ifeq (${REVISION},none)
build_BF561: build_BF561_none
clean: clean_current_dir clean_BF561_none
install: install_examples install_BF561_none
install_debug: install_examples install_debug_BF561_none
else
build_BF561: build_BF561_0_2 build_BF561_0_3 build_BF561_0_5 build_BF561_none build_BF561_any
clean: clean_current_dir clean_BF561_0_2 clean_BF561_0_3 clean_BF561_0_5 clean_BF561_none clean_BF561_any
install: install_examples install_BF561_0_2 install_BF561_0_3 install_BF561_0_5 install_BF561_none install_BF561_any
install_debug: install_examples install_debug_BF561_0_2 install_debug_BF561_0_3 install_debug_BF561_0_5 install_debug_BF561_none install_debug_BF561_any
endif
endif
endif
endif
endif
endif

# 0.2 libs
build_BF561_0_2:
	${MKDIR} bf561_rev_0.2
	"${MAKE}" -f../makefile.mk  -C bf561_rev_0.2 build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision 0.2" UP_CXXFLAGS+="-si-revision 0.2 " UP_ASMFLAGS+="-si-revision 0.2" NESTED_MAKE="YES"

clean_BF561_0_2:
	${MKDIR} bf561_rev_0.2
	"${MAKE}" -f../makefile.mk  -C bf561_rev_0.2 clean_libs  NESTED_MAKE="YES"

install_BF561_0_2:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf561_rev_0.2" INST_DIR="bf561_rev_0.2"

install_debug_BF561_0_2:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf561_rev_0.2" INST_DIR="bf561_rev_0.2"

# 0.3 libs
build_BF561_0_3:
	${MKDIR} bf561_rev_0.3
	"${MAKE}" -f../makefile.mk  -C bf561_rev_0.3 build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision 0.3" UP_CXXFLAGS+="-si-revision 0.3 " UP_ASMFLAGS+="-si-revision 0.3" NESTED_MAKE="YES"

clean_BF561_0_3:
	${MKDIR} bf561_rev_0.3
	"${MAKE}" -f../makefile.mk  -C bf561_rev_0.3 clean_libs  NESTED_MAKE="YES"

install_BF561_0_3:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf561_rev_0.3" INST_DIR="bf561_rev_0.3"

install_debug_BF561_0_3:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf561_rev_0.3" INST_DIR="bf561_rev_0.3"

# 0.5 libs
build_BF561_0_5:
	${MKDIR} bf561_rev_0.5
	"${MAKE}" -f../makefile.mk  -C bf561_rev_0.5 build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision 0.5" UP_CXXFLAGS+="-si-revision 0.5 " UP_ASMFLAGS+="-si-revision 0.5" NESTED_MAKE="YES"

clean_BF561_0_5:
	${MKDIR} bf561_rev_0.5
	"${MAKE}" -f../makefile.mk  -C bf561_rev_0.5 clean_libs  NESTED_MAKE="YES"

install_BF561_0_5:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf561_rev_0.5" INST_DIR="bf561_rev_0.5"

install_debug_BF561_0_5:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf561_rev_0.5" INST_DIR="bf561_rev_0.5"

# No workaround libraries
build_BF561_none:
	${MKDIR} bf561_rev_none
	"${MAKE}" -f../makefile.mk  -C bf561_rev_none build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision none" UP_CXXFLAGS+="-si-revision none " UP_ASMFLAGS+="-si-revision none" NESTED_MAKE="YES"

clean_BF561_none:
	${MKDIR} bf561_rev_none
	"${MAKE}" -f../makefile.mk  -C bf561_rev_none clean_libs  NESTED_MAKE="YES"
install_BF561_none:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf561_rev_none" INST_DIR=""

install_debug_BF561_none:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf561_rev_none" INST_DIR=""

# All workaround libraries
build_BF561_any:
	${MKDIR} bf561_rev_any
	"${MAKE}" -f../makefile.mk  -C bf561_rev_any build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision any" UP_CXXFLAGS+="-si-revision any " UP_ASMFLAGS+="-si-revision any" NESTED_MAKE="YES"

clean_BF561_any:
	${MKDIR} bf561_rev_any
	"${MAKE}" -f../makefile.mk  -C bf561_rev_any clean_libs  NESTED_MAKE="YES"
install_BF561_any:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf561_rev_any" INST_DIR="bf561_rev_any"

install_debug_BF561_any:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf561_rev_any" INST_DIR="bf561_rev_any"

######
# 2153X dlb build
#
${OUTPUT}:			${OBJS} ver.txt
	${MKDIR} release debug
	elfar -tx ver.txt -c ${OUTPUT} ${OBJS}
	@echo "****************************************"
	@echo "${OUTPUT} build completed at:"
	@date +%c
	@echo "****************************************"

######
# Generate the version information. It will be different in all libs
#

ver.txt: 
	${ECHO}   "Library_Built: " > $@
	@date +%c >>  $@
	${ECHO}   "Library_Name: VDSP++ Kernel Library" >>  $@
	${DOS_ENDL} >> $@
	${ECHO} "Library_Version: ${LIB_VERSION}"  >>  $@
	${DOS_ENDL} >> $@
	${ECHO}  "Platform: ${PLATFORM}"   >>   $@
	${DOS_ENDL} >> $@
	${ECHO}   "SpecialOptions: ${VEROPTIONS}"  >>   $@
	${DOS_ENDL} >> $@

######
# Generate the version information. It will be different in all libs
#

ver_core.txt: 
	${ECHO}   "Library_Built: " > $@
	${DOS_ENDL} >> $@
	${ECHO}   "Library_Name: VDSP++ Kernel Library - Processor specific section" >>  $@
	${DOS_ENDL} >> $@
	${ECHO} "Library_Version: ${LIB_VERSION}"  >>  $@
	${DOS_ENDL} >> $@
	${ECHO}  "Platform: ${PLATFORM}"   >>   $@
	${DOS_ENDL} >> $@
	${ECHO}   "SpecialOptions: ${VEROPTIONS}"  >>   $@
	${DOS_ENDL} >> $@

######
# Generate the .INI file for this part
#
FORCE:

ADSP-BF561.ini:FORCE
	${ECHO}   ""                   						>  $@
	${DOS_ENDL}						>> $@
	${ECHO} "[ADSP-BF561]"								>> $@
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
	${ECHO} "LinkerFile = $(QUOTE)blackfin\ldf\VDK-BF561.ldf$(QUOTE)"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Interrupts = $(QUOTE)$(CORE_INAMES)$(QUOTE)" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "ExceptionHandler = ExceptionHandler-BF561.asm" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Template = VDK_NMI_Asm_Source.asm" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_C_Template = VDK_NMI_C_Source.c" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Cpp_Template = VDK_NMI_CPP_Source.cpp" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "AsmIsrCodeTemplate = BF561_ISR_Asm_Source.asm"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "CIsrCodeTemplate = VDK_ISR_C_Source.c"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "CppIsrCodeTemplate = VDK_ISR_CPP_Source.cpp"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "ClockFrequency = 400" >>$@
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
	${ECHO}  ""                   				>> $@
	${DOS_ENDL}						>> $@
(CORE_OBJS):	VDK_IntVector.h BuildNumber.h $(CORE_SRCS) 

$(CORE_OUTPUT):	 ${CORE_OBJS} VDK_ISR_API.doj ver_core.txt
	${MKDIR} release debug
	elfar -tx ver_core.txt -c $(CORE_OUTPUT) ${CORE_OBJS} 
	@echo "****************************************"
	@echo "$(CORE_OUTPUT) build completed at:"
	@date +%c
	@echo "****************************************"


${VDKGEN}:
	"${MAKE}" -fmakefile.mk -C../../../Utilities/VdkGen
	@echo "**************************************"
	@echo "VdkGen Built"
	@echo "**************************************"

install_examples: ADSP-BF561.ini 
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/ldf
	${CP} VDK-BF561.ldf ${INSTALL_DIR}/ldf
	${CP} ADSP-BF561.ini ${INSTALL_DIR}/vdk
	${CP} ${VDKGEN} ${INSTALL_DIR}/vdk
	@echo "**************Creating LDF Vdk.cpp and Vdk.h files for examples ***********"
	@echo ""
	${foreach example_dir,${EXAMPLES_LIST}, cp VDK-BF561.ldf ${example_dir};}
	${foreach example_dir,${EXAMPLES_LIST}, cp ../templates/ExceptionHandler-BF561.asm.tf ${example_dir}/ExceptionHandler-BF561.asm; }
	${foreach the_kernel_file,${VDK_FILES_LIST}, ${INSTALL_DIR}/vdk/Vdkgen.exe ${VDKGEN_FLAGS} '$(shell cygpath -w ${CURR_DIR}/${the_kernel_file})'; }
	# MP messages require the LDFs in a different directory
	${RM} ${EXAMPLES_DIR}/Payload_Marshalling/VDK-BF561.ldf
	${foreach example_dir,${MP_EXAMPLES_LIST}, ${RM} ${example_dir}/VDK-BF561.ldf;}
	${foreach example_dir,${MP_EXAMPLES_LIST}, cp VDK-BF561.ldf ${example_dir}/COREA;}
	${foreach example_dir,${MP_EXAMPLES_LIST}, cp VDK-BF561.ldf ${example_dir}/COREB;}

	# Same for exception handler
	${RM} ${EXAMPLES_DIR}/Payload_Marshalling/ExceptionHandler-BF561.asm
	${foreach example_dir,${MP_EXAMPLES_LIST}, ${RM} ${example_dir}/ExceptionHandler-BF561.asm;}
	${foreach example_dir,${MP_EXAMPLES_LIST}, cp ../templates/ExceptionHandler-BF561.asm.tf ${example_dir}/COREA/ExceptionHandler-BF561.asm; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, cp ../templates/ExceptionHandler-BF561.asm.tf ${example_dir}/COREB/ExceptionHandler-BF561.asm; }

	# Remove rbld files
	${foreach example_dir,${EXAMPLES_LIST}, ${RM} ${example_dir}/*.rbld; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, ${RM} ${example_dir}/Core[AB]/*.rbld; }

	@echo "**************LDF Vdk.cpp and Vdk.h for examples created ***********"
	@echo ""
	"${MAKE}" -i -f../../../../common/Examples/makefile.mk -C${EXAMPLES_DIR} UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/"No\ Hardware\ Required"/VDK/BF561 install
	# This directory is empty (was put in by mistake) so we remove it
	${RM} -r ${INSTALL_DIR}/Examples/No\ Hardware\ Required/VDK/BF561/Produce_Consumer_MP
install_debug_libs:
	${MKDIR} ${INSTALL_DIR}/lib/${INST_DIR}	
	${CP} ${SI_REV}/debug/VDK-*-BF561*.dlb ${INSTALL_DIR}/lib/${INST_DIR}
install_libs: install_debug_libs
	${CP} ${SI_REV}/release/VDK-*-BF561*.dlb ${INSTALL_DIR}/lib/${INST_DIR}
			
dist_clean:
	${RM} ${INSTALL_DIR}/ldf/VDK-BF561.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-BF561.ini
	${RM} ${INSTALL_DIR}/lib/VDK-CORE-BF561.dlb
	${RM} ${INSTALL_DIR}/lib/VDK-*-BF561*.dlb
	${RM} ${INT_VEC_SRCS}
	"${MAKE}" -i -f../../../../common/Examples/makefile.mk -C${EXAMPLES_DIR} UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/VDK/BF561 dist_clean

squeaky_current_dir: 
	${RM} ADSP-BF561.ini
	@echo "**** CLEANING EXAMPLES ****"
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreA/VDK-BF561.ldf; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreA/VDK.cpp; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreA/VDK.h; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreB/VDK-BF561.ldf; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreB/VDK.cpp; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreB/VDK.h; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreA/ExceptionHandler-BF561.asm; }
	${foreach example_dir,${MP_EXAMPLES_LIST}, rm ${example_dir}/CoreB/ExceptionHandler-BF561.asm; }

clean_libs:
	${RM} *.ii
	${RM} *.ti
	${RM} ${CORE_OBJS}
	${RM} ${OBJS}
	${RM} ver.txt
	${RM} ver_core.txt

clean_current_dir:
	${RM} ADSP-BF561.ini
	${RM} *.s
	${RM} ${CORE_SRCS}
	${RM} ${INT_VEC_SRCS}
	${RM} ${INT_RVEC_SRCS}
	${RM} seg_int_hook.asm


