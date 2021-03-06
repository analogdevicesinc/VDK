#
#   $RCSfile$
#
#   Description: Makefile for compiling the BF592-A
#
#   Last modified $Date$
#	Last modified by $Author$
#   $Revision$
#   $Source$ 
#
# -----------------------------------------------------------------------------
#  Comments: 
# -----------------------------------------------------------------------------
#  Copyright (c) 2004 Analog Devices, Inc.,  All rights reserved
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
# Additional suffixes
###########################################################
.SUFFIXES: .asm .doj
.asm.doj:
	${ASM} ${DEFINES} ${ASMFLAGS} ${subst -I,-i ,${INCLUDE_PATH}} `cygpath -w $<` -o $@

.cpp.doj:
	${CXX} ${DEFINES} -c ${CXXFLAGS} `cygpath -w $<` -o $@

###########################################################
# Setup the variables that are passed to sub calls of make
# These lines should change as they progress up the calls,
# but should basically remain the same from level to level
###########################################################
VPATH         = ${CURDIR}:${UP_VPATH}
UNDO_DEFINES  = -DLCK_INLINE="" -DTMK_INLINE=""
ASM           = EASMBLKFN
ASMFLAGS      = -proc ADSP-BF592-A ${UP_ASMFLAGS}
CXX           = ${UP_CXX}
CXXFLAGS      = -proc ADSP-BF592-A ${UP_CXXFLAGS} ${INCLUDE_PATH} 
SRC           = TimerInit.cpp Version.cpp InitPlatform.cpp

ifeq (${NESTED_MAKE}, YES)
CORE_VECS     = $(shell ${TYPE} ../../21535/IntVectors.dat)
else
CORE_VECS     = $(shell ${TYPE} ../21535/IntVectors.dat)
endif
CORE_SRCS     = vdk_hdr_bf53x.asm panic.asm ExceptionPanic.asm InterruptPanic.asm adi_stack_overflow.asm
OBJS          = ${UP_OBJS}
ifneq (${NESTED_MAKE}, YES)
DEFINES               = ${UP_DEFINES} ${UNDO_DEFINES} -DDO_NOT_INLINE
else
DEFINES               = ${UP_DEFINES} -DVERSION_PRODUCT_="VDK::kBF592A" ${UNDO_DEFINES} -DDO_NOT_INLINE
endif

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

COMMA		= ,
ENDCOMMA	= ,,
ENDCODE		= \"+\",,
QUOTE		=\"
DOS_ENDL   = @cmd /c echo.
PLUS		= +
CORE_INAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA)) 

CORE_OBJS     = $(CORE_SRCS:.asm=.doj) ${SRC:.cpp=.doj}
PLATFORM    = ADSP-BF592-A
VERS_DATE = $(shell date  +%a%b%e%Y%T")
OUTPUT                = ${OUT_DIR}BF532.dlb
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/blackfin/2153X/BF592-A)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
CORE_HINAMES     = $(filter-out $(ENDCODE), $(patsubst seg_%,% $(QUOTE)$(PLUS)$(QUOTE)$(COMMA), $(CORE_VECS))$(COMMA)) 

ifeq ($(findstring debug,$(OUTPUT)),debug)
CORE_OUTPUT   = ./debug/VDK-CORE-BF532.dlb
INSTALL_TYPE= debug
else
CORE_OUTPUT   = ./release/VDK-CORE-BF532.dlb
INSTALL_TYPE= release
endif


#
# Projects
#
all:          default
default:      build_BF592-A
squeaky: squeaky_current_dir clean
build_libs:   $(OUTPUT) $(CORE_OUTPUT)

build_BF592-A: build_BF592-A_any build_BF592-A_0.1
clean: clean_current_dir clean_BF592-A_any clean_BF592-A_0.1
install: install_examples install_BF592-A_any install_BF592-A_0.1
install_debug: install_examples install_debug_BF592-A_any install_debug_BF592-A_0.1

######
# 0.1 libs
build_BF592-A_0.1:
	${MKDIR} bf592-A_rev_0.1
	"${MAKE}" -f../makefile.mk  -C bf592-A_rev_0.1 build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision 0.1" UP_CXXFLAGS+="-si-revision 0.1 " UP_ASMFLAGS+="-si-revision 0.1 -D__WORKAROUNDS_ENABLED" NESTED_MAKE="YES"


clean_BF592-A_0.1:
	${MKDIR} bf592-A_rev_0.1
	"${MAKE}" -f../makefile.mk  -C bf592-A_rev_0.1 clean_libs  NESTED_MAKE="YES"

install_BF592-A_0.1:
	"${MAKE}" -i -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf592-A_rev_0.1" INST_DIR="bf592-A_rev_0.1"

install_debug_BF592-A_0.1:
	"${MAKE}" -i -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf592-A_rev_0.1" INST_DIR="bf592-A_rev_0.1"

######
# any libs
build_BF592-A_any:
	${MKDIR} bf592-A_rev_any
	"${MAKE}" -f../makefile.mk  -C bf592-A_rev_any build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision any" UP_CXXFLAGS+="-si-revision any " UP_ASMFLAGS+="-si-revision any -D__WORKAROUNDS_ENABLED" NESTED_MAKE="YES"


clean_BF592-A_any:
	${MKDIR} bf592-A_rev_any
	"${MAKE}" -f../makefile.mk  -C bf592-A_rev_any clean_libs  NESTED_MAKE="YES"

install_BF592-A_any:
	"${MAKE}" -i -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf592-A_rev_any" INST_DIR="bf592-A_rev_any"

install_debug_BF592-A_any:
	"${MAKE}" -i -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf592-A_rev_any" INST_DIR="bf592-A_rev_any"

######
# 2153X dlb build
#
${OUTPUT}:                    ${OBJS} ver.txt
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

ADSP-BF592-A.ini:FORCE
	${ECHO}   ""                   						>  $@
	${DOS_ENDL}						>> $@
	${ECHO} "[ADSP-BF592-A]"								>> $@
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
	${ECHO} "LinkerFile = $(QUOTE)blackfin\ldf\VDK-BF592-A.ldf$(QUOTE)"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Interrupts = $(QUOTE)$(CORE_INAMES)$(QUOTE)" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "ExceptionHandler = ExceptionHandler-BF592-A.asm" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Template = VDK_NMI_Asm_Source.asm" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_C_Template = VDK_NMI_C_Source.c" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Cpp_Template = VDK_NMI_CPP_Source.cpp" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "AsmIsrCodeTemplate = BF592-A_ISR_Asm_Source.asm"	>> $@
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

$(CORE_OBJS):    VDK_IntVector.h BuildNumber.h $(CORE_SRCS)

$(CORE_OUTPUT):  ${CORE_OBJS} VDK_ISR_API.doj ver_core.txt
	${MKDIR} release debug
	elfar -tx ver_core.txt -c $(CORE_OUTPUT) ${CORE_OBJS}
	@echo "****************************************"
	@echo "$(CORE_OUTPUT) build completed at:"
	@date +%c
	@echo "****************************************"

install_examples: ADSP-BF592-A.ini 
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/ldf
	${MKDIR} ${INSTALL_DIR}/lib
	${CP} VDK-BF592-A.ldf ${INSTALL_DIR}/ldf
	${CP} ADSP-BF592-A.ini ${INSTALL_DIR}/vdk
	
install_debug_libs:
	${MKDIR} ${INSTALL_DIR}/lib/${INST_DIR}
	${CP} ${SI_REV}/debug/VDK-*-BF532*.dlb ${INSTALL_DIR}/lib/${INST_DIR}
install_libs: install_debug_libs
	${CP} ${SI_REV}/release/VDK-*-BF532*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

dist_clean:
	${RM} ${INSTALL_DIR}/ldf/VDK-BF592-A.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-BF592-A.ini
	${RM} ${INT_VEC_SRCS}

squeaky_current_dir: clean_current_dir

clean_current_dir:
	${RM}  ADSP-BF592-A.ini
	${RM} *.s
	${RM} ${CORE_SRCS}
	${RM} ${INT_VEC_SRCS}
	${RM} ${INT_RVEC_SRCS}
	${RM} seg_int_hook.asm

clean_libs:
	${RM} *.ii
	${RM} *.ti
	${RM} ${CORE_OBJS}
	${RM} ${OBJS}
	${RM} ver.txt
	${RM} ver_core.txt


