#
#   $RCSfile$
#
#   Description: Makefile for compiling the 21532
#
#   Last modified $Date$
#	Last modified by $Author$
#   $Revision$
#   $Source$
#
# -----------------------------------------------------------------------------
#  Comments:
# -----------------------------------------------------------------------------
#  Copyright (c) 2002 Analog Devices, Inc.,  All rights reserved
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
ASMFLAGS	= -proc ADSP-BF532 ${UP_ASMFLAGS}
CXX			= ${UP_CXX}
CXXFLAGS	= -proc ADSP-BF532 ${UP_CXXFLAGS} ${INCLUDE_PATH}
SRC			= TimerInit.cpp Version.cpp InitPlatform.cpp
ifeq (${NESTED_MAKE}, YES)
CORE_VECS	= $(shell ${TYPE} ../../21535/IntVectors.dat)
else
CORE_VECS	= $(shell ${TYPE} ../21535/IntVectors.dat)
endif
PLATFORM    = ADSP-BF532
VERS_DATE = $(shell date  +%a%b%e%Y%T")

CORE_SRCS	= vdk_hdr_bf53x.asm panic.asm ExceptionPanic.asm InterruptPanic.asm adi_stack_overflow.asm
OBJS		= ${UP_OBJS}
ifeq (${NESTED_MAKE}, YES)
DEFINES		= ${UP_DEFINES}
else
DEFINES		= ${UP_DEFINES} -DVERSION_PRODUCT_="VDK::kBF532"
endif
COMMA		= ,
ENDCOMMA	= ,,
ENDCODE		= \"+\",,
QUOTE		= \"
DOS_ENDL   = @cmd /c echo.
PLUS		= +
CORE_INAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA))
CORE_OBJS	= $(CORE_SRCS:.asm=.doj) ${SRC:.cpp=.doj}
VPATH		= ${CURDIR}:${UP_VPATH}
OUTPUT		= ${OUT_DIR}BF532.dlb
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/blackfin/2153X/21532)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
VDKGEN          = ../../../Utilities/VdkGen/Release/VdkGen.exe
VDK_FILES_LIST	= ${subst /,\\,${shell ls -1 Examples/*/*.vdk}}
EXAMPLES_LIST	= ${shell find Examples/* -type d \! -name CVS -prune -exec ls -1d {} \;}
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
                          ADI_MAKE="${ADI_MAKE}"     \
                          API_VERSION="${API_VERSION}"     \
                          LIB_VERSION="${LIB_VERSION}"     \
                          VDSP_VERSION="${VDSP_VERSION}"


ifeq ($(findstring debug,$(OUTPUT)),debug)
CORE_OUTPUT	= ./debug/VDK-CORE-BF532.dlb
INSTALL_TYPE= debug
else
CORE_OUTPUT	= ./release/VDK-CORE-BF532.dlb
INSTALL_TYPE= release
endif


#
# Projects
#
all:		default
default:	build_BF532
squeaky: squeaky_current_dir clean
build_libs:	$(OUTPUT) $(CORE_OUTPUT)

ifeq (${REVISION},all)
build_BF532: build_BF532_0_1 build_BF532_0_2 build_BF532_0_3 build_BF532_0_5 build_BF532_none build_BF532_any
clean: clean_current_dir clean_BF532_0_1 clean_BF532_0_2 clean_BF532_0_3 clean_BF532_0_5 clean_BF532_none clean_BF532_any
install: install_examples install_BF532_0_1 install_BF532_0_2 install_BF532_0_3 install_BF532_0_5 install_BF532_none install_BF532_any
install_debug: install_examples install_debug_BF532_0_1 install_debug_BF532_0_2 install_debug_BF532_0_3 install_debug_BF532_0_5 install_debug_BF532_none install_debug_BF532_any
else
ifeq (${REVISION},0.1)
build_BF532: build_BF532_0_1
clean: clean_current_dir clean_BF532_0_1
install: install_examples install_BF532_0_1
install_debug: install_examples install_debug_BF532_0_1
else
ifeq (${REVISION},0.2)
build_BF532: build_BF532_0_2
clean: clean_current_dir clean_BF532_0_2
install: install_examples install_BF532_0_2
install_debug: install_examples install_debug_BF532_0_2
else
ifeq (${REVISION},0.3)
build_BF532: build_BF532_0_3
clean: clean_current_dir clean_BF532_0_3
install: install_examples install_BF532_0_3
install_debug: install_examples install_debug_BF532_0_3
else
ifeq (${REVISION},0.5)
build_BF532: build_BF532_0_5
clean: clean_current_dir clean_BF532_0_5
install: install_examples install_BF532_0_5
install_debug: install_examples install_debug_BF532_0_5
else
ifeq (${REVISION},none)
build_BF532: build_BF532_none
clean: clean_current_dir clean_BF532_none
install: install_examples install_BF532_none
install_debug: install_examples install_debug_BF532_none
else
ifeq (${REVISION},any)
build_BF532: build_BF532_any
clean: clean_current_dir clean_BF532_any
install: install_examples install_BF532_any
install_debug: install_examples install_debug_BF532_any
else
build_BF532: build_BF532_0_1 build_BF532_0_2 build_BF532_0_3 build_BF532_0_5 build_BF532_none build_BF532_any
clean: clean_current_dir clean_BF532_0_1 clean_BF532_0_2 clean_BF532_0_3 clean_BF532_0_5 clean_BF532_none clean_BF532_any
install: install_examples install_BF532_0_1 install_BF532_0_2 install_BF532_0_3 install_BF532_0_5 install_BF532_none install_BF532_any
install_debug: install_examples install_debug_BF532_0_1 install_debug_BF532_0_2 install_debug_BF532_0_3 install_debug_BF532_0_5 install_debug_BF532_none install_debug_BF532_any
endif
endif
endif
endif
endif
endif
endif

# Si revision 0.1
build_BF532_0_1:
	${MKDIR} bf532_rev_0.1
	"${MAKE}" -f../makefile.mk  -C bf532_rev_0.1 build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision 0.1" UP_CXXFLAGS+="-si-revision 0.1 " UP_ASMFLAGS+="-si-revision 0.1 -D__WORKAROUNDS_ENABLED" NESTED_MAKE="YES"

clean_BF532_0_1:
	${MKDIR} bf532_rev_0.1
	"${MAKE}" -f../makefile.mk  -C bf532_rev_0.1 clean_libs  NESTED_MAKE="YES"

install_BF532_0_1:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf532_rev_0.1" INST_DIR="bf532_rev_0.1"

install_debug_BF532_0_1:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf532_rev_0.1" INST_DIR="bf532_rev_0.1"

# Si revision 0.2
build_BF532_0_2:
	${MKDIR} bf532_rev_0.2
	"${MAKE}" -f../makefile.mk  -C bf532_rev_0.2 build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision 0.2" UP_CXXFLAGS+="-si-revision 0.2 " UP_ASMFLAGS+="-si-revision 0.2 -D__WORKAROUNDS_ENABLED" NESTED_MAKE="YES"

clean_BF532_0_2:
	${MKDIR} bf532_rev_0.2
	"${MAKE}" -f../makefile.mk  -C bf532_rev_0.2 clean_libs  NESTED_MAKE="YES"

install_BF532_0_2:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf532_rev_0.2" INST_DIR="bf532_rev_0.2"

install_debug_BF532_0_2:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf532_rev_0.2" INST_DIR="bf532_rev_0.2"

# Si revision 0.3
build_BF532_0_3:
	${MKDIR} bf532_rev_0.3
	"${MAKE}" -f../makefile.mk  -C bf532_rev_0.3 build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision 0.3" UP_CXXFLAGS+="-si-revision 0.3 " UP_ASMFLAGS+="-si-revision 0.3 -D__WORKAROUNDS_ENABLED" NESTED_MAKE="YES"

clean_BF532_0_3:
	${MKDIR} bf532_rev_0.3
	"${MAKE}" -f../makefile.mk  -C bf532_rev_0.3 clean_libs  NESTED_MAKE="YES"

install_BF532_0_3:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf532_rev_0.3" INST_DIR="bf532_rev_0.3"

install_debug_BF532_0_3:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf532_rev_0.3" INST_DIR="bf532_rev_0.3"

# Si revision 0.5
build_BF532_0_5:
	${MKDIR} bf532_rev_0.5
	"${MAKE}" -f../makefile.mk  -C bf532_rev_0.5 build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision 0.5" UP_CXXFLAGS+="-si-revision 0.5 " UP_ASMFLAGS+="-si-revision 0.5 -D__WORKAROUNDS_ENABLED" NESTED_MAKE="YES"

clean_BF532_0_5:
	${MKDIR} bf532_rev_0.5
	"${MAKE}" -f../makefile.mk  -C bf532_rev_0.5 clean_libs  NESTED_MAKE="YES"

install_BF532_0_5:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf532_rev_0.5" INST_DIR="bf532_rev_0.5"

install_debug_BF532_0_5:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf532_rev_0.5" INST_DIR="bf532_rev_0.5"

# No workaround libraries
build_BF532_none:
	${MKDIR} bf532_rev_none
	"${MAKE}" -f../makefile.mk  -C bf532_rev_none build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision none" UP_CXXFLAGS+="-si-revision none " UP_ASMFLAGS+="-si-revision none " NESTED_MAKE="YES"

clean_BF532_none:
	${MKDIR} bf532_rev_none
	"${MAKE}" -f../makefile.mk  -C bf532_rev_none clean_libs  NESTED_MAKE="YES"
install_BF532_none:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf532_rev_none" INST_DIR=""

install_debug_BF532_none:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf532_rev_none" INST_DIR=""


# All workaround libraries
build_BF532_any:
	${MKDIR} bf532_rev_any
	"${MAKE}" -f../makefile.mk  -C bf532_rev_any build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision any" UP_CXXFLAGS+="-si-revision any " UP_ASMFLAGS+="-si-revision any " NESTED_MAKE="YES"

clean_BF532_any:
	${MKDIR} bf532_rev_any
	"${MAKE}" -f../makefile.mk  -C bf532_rev_any clean_libs  NESTED_MAKE="YES"
install_BF532_any:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf532_rev_any" INST_DIR="bf532_rev_any"

install_debug_BF532_any:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf532_rev_any" INST_DIR="bf532_rev_any"


######
# 2153X dlb build
#
${OUTPUT}:	${OBJS} ver.txt
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
	@date +%c >>  $@
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

ADSP-BF532.ini: FORCE
	${ECHO}   ""                   				>  $@
	${DOS_ENDL}						>> $@
	${ECHO} "[ADSP-BF532]"					>> $@
	${DOS_ENDL}						>> $@
	${ECHO} ""                     				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Family = Blackfin"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "NaturalWordSize = 32"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "MaxUnsignedInt = 4294967295"			>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "MaxLong = 2147483647"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "MinLong = -2147483648"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Templates = $(QUOTE)blackfin\vdk\\$(QUOTE)"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "HeapSupport = true"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "LinkerFile = $(QUOTE)blackfin\ldf\VDK-BF532.ldf$(QUOTE)"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Interrupts = $(QUOTE)$(CORE_INAMES)$(QUOTE)" 	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "ExceptionHandler = ExceptionHandler-BF532.asm" >> $@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Template = VDK_NMI_Asm_Source.asm" 	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_C_Template = VDK_NMI_C_Source.c" 	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Cpp_Template = VDK_NMI_Cpp_Source.cpp"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "AsmIsrCodeTemplate = BF532_ISR_Asm_Source.asm"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "CIsrCodeTemplate = VDK_ISR_C_Source.c"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "CppIsrCodeTemplate = VDK_ISR_CPP_Source.cpp"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "ClockFrequency = 270" 				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "TickPeriod = 0.1" 				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "VdkVersion = 5.0" 				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "IdleThreadStack = 256"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "DefaultTimerInterrupt = EVT_IVTMR"		>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "DefaultHistoryBufferSize = 256"		>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "NumContextAreas = 0"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "NumThreadStacks = 1" 				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "NumStackSizes = 1" 				>> $@
	${DOS_ENDL}						>> $@
	${ECHO}  ""                       			>> $@
	${DOS_ENDL}						>> $@


$(CORE_OBJS):	VDK_IntVector.h BuildNumber.h $(CORE_SRCS)

$(CORE_OUTPUT):	 ${CORE_OBJS} VDK_ISR_API.doj ver_core.txt
	${MKDIR} release debug
	elfar -tx ver_core.txt -c $(CORE_OUTPUT) ${CORE_OBJS}
	@echo "****************************************"
	@echo "$(CORE_OUTPUT) build completed at:"
	@date +%c
	@echo "****************************************"

clean_current_dir:
	${RM} ADSP-BF532.ini

clean_libs:
	${RM} ADSP-BF532.ini
	${RM} *.ii *.ti *.s
	${RM} ${CORE_OBJS} ${OBJS}
	${RM} ver.txt
	${RM} ver_core.txt

squeaky_current_dir: clean_current_dir

${VDKGEN}:
	"${MAKE}" -fmakefile.mk -C../../../Utilities/VdkGen
	@echo "**************************************"
	@echo "VdkGen Built"
	@echo "**************************************"

# there are not BF532 examples but we install the common things
install_examples: ADSP-BF532.ini ${VDKGEN}
	${MKDIR} ${INSTALL_DIR}/vdk ${INSTALL_DIR}/ldf
	${CP} VDK-BF532.ldf ${INSTALL_DIR}/ldf
	${CP} ADSP-BF532.ini ${INSTALL_DIR}/vdk

install_debug_libs: ADSP-BF532.ini ${VDKGEN}
	${MKDIR} ${INSTALL_DIR}/lib/${INST_DIR}
	${CP} ${SI_REV}/debug/VDK-*-BF532*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

install_libs:install_debug_libs
	${CP} ${SI_REV}/release/VDK-*-BF532*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

dist_clean:
	${RM} ${INSTALL_DIR}/ldf/VDK-BF532.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-BF532.ini
	${RM} ${INSTALL_DIR}/lib/VDK-*-BF532*.dlb
	"${MAKE}" -i -f../../../../common/Examples/makefile.mk -CExamples UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/VDK/BF532 dist_clean
	${RMDIR} debug release
	${MKDIR} debug release
