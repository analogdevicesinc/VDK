#
#   $RCSfile$
#
#   Description: Makefile for compiling the 21535
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
	${ASM} ${ASMFLAGS} ${subst -I,-i ,${INCLUDE_PATH}} `cygpath -w $<` -o $@

.cpp.doj:
	${CXX} -c ${CXXFLAGS} `cygpath -w $<` -o $@


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
ASMFLAGS	= -proc ADSP-BF535 ${UP_ASMFLAGS} ${DEFINES}
CXX			= ${UP_CXX}
CXXFLAGS	= -proc ADSP-BF535 ${UP_CXXFLAGS} ${INCLUDE_PATH} ${DEFINES}
SRC			= TimerInit.cpp Version.cpp InitPlatform.cpp
ifeq (${NESTED_MAKE}, YES)
CORE_VECS	= $(shell ${TYPE} ../IntVectors.dat) 
else
CORE_VECS	= $(shell ${TYPE} IntVectors.dat)
endif
CORE_SRCS	= vdk_hdr_bf53x.asm panic.asm ExceptionPanic.asm InterruptPanic.asm adi_stack_overflow.asm
OBJS		= ${UP_OBJS}
DEFINES		= ${UP_DEFINES} -DVERSION_PRODUCT_="VDK::kBF535"
COMMA		= ,
ENDCOMMA	= ,,
ENDCODE		= \"+\",,
QUOTE		= \"
DOS_ENDL   = @cmd /c echo. 
PLUS		= +
CORE_INAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA)) 
CORE_OBJS	= $(CORE_SRCS:.asm=.doj) ${SRC:.cpp=.doj} 
PLATFORM    = ADSP-BF535
VERS_DATE = $(shell date  +%a%b%e%Y%T")
VPATH		= ${CURDIR}:${UP_VPATH}
OUTPUT		= ${OUT_DIR}BF535.dlb
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/blackfin/2153X/21535)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
VDKGEN          = ../../../Utilities/VdkGen/Release/VdkGen.exe
CURR_DIR=${subst \,\\,${shell cygpath -w `pwd`}}
EXAMPLES_DIR = Examples_35

ifeq (${VDSP_VERSION},5.0)
VDKGEN_FLAGS = -proc ADSP-BF535 ++vdkgen_for_rt ++VDSP_5.0 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
else
ifeq (${VDSP_VERSION},4.5)
VDKGEN_FLAGS = -proc ADSP-BF535 ++vdkgen_for_rt ++VDSP_4.5 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
else
VDKGEN_FLAGS = -proc ADSP-BF535 ++vdkgen_for_rt ++VDSP_4.0 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
endif
endif

VDK_FILES_LIST= ${subst /,\\,${shell ls -1 ${EXAMPLES_DIR}/*/*.vdk}}
EXAMPLES_LIST	= ${shell find ${EXAMPLES_DIR}/* -type d \! -name CVS -prune -exec ls -1d {} \;}

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
CORE_OUTPUT	= ./debug/VDK-CORE-BF535.dlb
INSTALL_TYPE= debug
else
CORE_OUTPUT	= ./release/VDK-CORE-BF535.dlb
INSTALL_TYPE= release
endif


#
# Projects
#
all:		default
default:	build_BF535
squeaky: squeaky_current_dir clean
build_libs:	$(OUTPUT) $(CORE_OUTPUT) 

ifeq (${REVISION},all)
build_BF535: build_BF535_0_2 build_BF535_none build_BF535_any
clean: clean_current_dir clean_BF535_0_2 clean_BF535_none clean_BF535_any
install: install_examples install_BF535_0_2 install_BF535_none install_BF535_any
install_debug: install_examples install_debug_BF535_0_2 install_debug_BF535_none install_debug_BF535_any
else
ifeq (${REVISION},0.2)
build_BF535: build_BF535_0_2
clean: clean_current_dir clean_BF535_0_2
install: install_examples install_BF535_0_2
install_debug: install_examples install_debug_BF535_0_2
else
ifeq (${REVISION},none)
build_BF535: build_BF535_none
clean: clean_current_dir clean_BF535_none
install: install_examples install_BF535_none
install_debug: install_examples install_debug_BF535_none
else
ifeq (${REVISION},any)
build_BF535: build_BF535_any
clean: clean_current_dir clean_BF535_any
install: install_examples install_BF535_any
install_debug: install_examples install_debug_BF535_any
else
build_BF535: build_BF535_0_2 build_BF535_none build_BF535_any
clean: clean_current_dir clean_BF535_0_2 clean_BF535_none clean_BF535_any
install: install_examples install_BF535_0_2 install_BF535_none install_BF535_any
install_debug: install_examples install_debug_BF535_0_2 install_debug_BF535_none install_debug_BF535_any
endif
endif
endif
endif

# All silicon so far uses 0.2 libs
build_BF535_0_2:
	${MKDIR} bf535_rev_0.2
	"${MAKE}" -f../makefile.mk  -C bf535_rev_0.2 build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision 0.2" UP_CXXFLAGS+="-si-revision 0.2 " UP_ASMFLAGS+="-si-revision 0.2 -D__WORKAROUNDS_ENABLED" NESTED_MAKE="YES"

clean_BF535_0_2: 
	${MKDIR} bf535_rev_0.2
	"${MAKE}" -f../makefile.mk  -C bf535_rev_0.2 clean_libs  NESTED_MAKE="YES"

install_BF535_0_2:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf535_rev_0.2" INST_DIR="bf535_rev_0.2" 

install_debug_BF535_0_2:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf535_rev_0.2" INST_DIR="bf535_rev_0.2" 

# No workaround libraries
build_BF535_none:
	${MKDIR} bf535_rev_none
	"${MAKE}" -f../makefile.mk  -C bf535_rev_none build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision none"  UP_CXXFLAGS+="-si-revision none " UP_ASMFLAGS+="-si-revision none " NESTED_MAKE="YES"

clean_BF535_none: 
	${MKDIR} bf535_rev_none
	"${MAKE}" -f../makefile.mk  -C bf535_rev_none clean_libs  NESTED_MAKE="YES"
install_BF535_none:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf535_rev_none" INST_DIR="" 

install_debug_BF535_none:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf535_rev_none" INST_DIR="" 

# All workaround libraries
build_BF535_any:
	${MKDIR} bf535_rev_any
	"${MAKE}" -f../makefile.mk  -C bf535_rev_any build_libs ${NEW_FLAGS} VEROPTIONS="-si-revision any" UP_CXXFLAGS+="-si-revision any " UP_ASMFLAGS+="-si-revision any " NESTED_MAKE="YES"

clean_BF535_any: 
	${MKDIR} bf535_rev_any
	"${MAKE}" -f../makefile.mk  -C bf535_rev_any clean_libs  NESTED_MAKE="YES"
install_BF535_any:
	"${MAKE}" -fmakefile.mk  install_libs ${NEW_FLAGS} SI_REV="bf535_rev_any" INST_DIR="bf535_rev_any" 

install_debug_BF535_any:
	"${MAKE}" -fmakefile.mk  install_debug_libs ${NEW_FLAGS} SI_REV="bf535_rev_any" INST_DIR="bf535_rev_any" 

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

ADSP-BF535.ini:FORCE
	${ECHO} ""                     						>  $@
	${DOS_ENDL}						>> $@
	${ECHO} "[ADSP-BF535]"					>> $@
	${DOS_ENDL}						>> $@
	${ECHO}  ""                         				>> $@
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
	${ECHO} "Templates = $(QUOTE)blackfin\vdk\\$(QUOTE)"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "HeapSupport = true"						>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "LinkerFile = $(QUOTE)blackfin\ldf\VDK-BF535.ldf$(QUOTE)"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Interrupts = $(QUOTE)$(CORE_INAMES)$(QUOTE)" >> $@
	${DOS_ENDL}						>> $@
	${ECHO} "ExceptionHandler = ExceptionHandler-BF535.asm" >> $@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Template = VDK_NMI_Asm_Source.asm" >> $@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_C_Template = VDK_NMI_C_Source.c" 	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Cpp_Template = VDK_NMI_Cpp_Source.cpp"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "AsmIsrCodeTemplate = BF535_ISR_Asm_Source.asm"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "CIsrCodeTemplate = VDK_ISR_C_Source.c"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "CppIsrCodeTemplate = VDK_ISR_CPP_Source.cpp"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "ClockFrequency = 350" >> $@
	${DOS_ENDL}						>> $@
	${ECHO} "TickPeriod = 0.1" >> $@
	${DOS_ENDL}						>> $@
	${ECHO} "VdkVersion = 5.0" >> $@
	${DOS_ENDL}						>> $@
	${ECHO} "IdleThreadStack = 256"				>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "DefaultTimerInterrupt = EVT_IVTMR"		>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "DefaultHistoryBufferSize = 256"		>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "NumContextAreas = 0" 				>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "NumThreadStacks = 1" >> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "NumStackSizes = 1" 				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} ""                           				>> $@
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
	${RM} ADSP-BF535.ini
	${RM} *.s
	${RM} ${CORE_SRCS} 
	${RM} ${INT_VEC_SRCS}
	${RM} ${INT_RVEC_SRCS}
	${RM} seg_int_hook.asm

clean_libs:
	${RM} *.s
	${RM} *.ii
	${RM} *.ti
	${RM} ${CORE_OBJS}
	${RM} ${OBJS}
	${RM} ver.txt
	${RM} ver_core.txt
squeaky_current_dir:
	@echo "**** CLEANING EXAMPLES ****"
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK-BF535.ldf; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK.cpp; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK.h; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/ExceptionHandler-BF535.asm; }

${VDKGEN}:
	"${MAKE}" -fmakefile.mk -C../../../Utilities/VdkGen
	@echo "****VdkGen Built****"

install_examples: ADSP-BF535.ini ${VDKGEN} 
	"${MAKE}" -fmakefile.mk -C../../../Utilities/VdkGen
	@echo "**************************************"
	@echo "VdkGen Built"
	@echo "**************************************"
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/ldf
	${CP} VDK-BF535.ldf ${INSTALL_DIR}/ldf
	${CP} ${VDKGEN} ${INSTALL_DIR}/vdk
	${CP} ADSP-BF535.ini ${INSTALL_DIR}/vdk
	@echo "**************Creating LDF Vdk.cpp and Vdk.h files for examples ***********"
	@echo ""
	${foreach example_dir,${EXAMPLES_LIST}, cp VDK-BF535.ldf ${example_dir}; }
	${foreach example_dir,${EXAMPLES_LIST}, cp ../templates/ExceptionHandler-BF535.asm.tf ${example_dir}/ExceptionHandler-BF535.asm; }
	${foreach the_kernel_file,${VDK_FILES_LIST}, ${INSTALL_DIR}/vdk/Vdkgen.exe ${VDKGEN_FLAGS} '$(shell cygpath -w ${CURR_DIR}/${the_kernel_file})'; }
	${foreach example_dir,${EXAMPLES_LIST}, ${RM} ${example_dir}/*.rbld; }
	@echo "**************LDF Vdk.cpp and Vdk.h for examples created ***********"
	@echo ""
	"${MAKE}" -i -f../../../../common/Examples/makefile.mk -C${EXAMPLES_DIR} UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/"No\ Hardware\ Required"/VDK/BF535 install
	# Move the examples that require hardware
	${MKDIR} ${INSTALL_DIR}/Examples/ADSP-BF535\ EZ-KIT\ Lite/VDK
	${RMDIR} ${INSTALL_DIR}/Examples/ADSP-BF535\ EZ-KIT\ Lite/VDK/Pipelined*
	mv -f ${INSTALL_DIR}/Examples/No\ Hardware\ Required/VDK/BF535/Pipelined* ${INSTALL_DIR}/Examples/ADSP-BF535\ EZ-KIT\ Lite/VDK

install_debug_libs:
	${MKDIR} ${INSTALL_DIR}/lib/${INST_DIR}
	${CP} ${SI_REV}/debug/VDK-*-BF535*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

install_libs: install_debug_libs
	${CP} ${SI_REV}/release/VDK-*-BF535*.dlb ${INSTALL_DIR}/lib/${INST_DIR}
	
dist_clean:
	${RM} ${INSTALL_DIR}/ldf/VDK-BF535.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-BF535.ini
	${RM} ${INSTALL_DIR}/lib/VDK-CORE-BF535.dlb
	${RM} ${INSTALL_DIR}/lib/VDK-*-BF535*.dlb
	"${MAKE}" -i -f../../../../common/Examples/makefile.mk -C${EXAMPLES_DIR} UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/"No\ Hardware\ Required"/VDK/BF535 dist_clean
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

