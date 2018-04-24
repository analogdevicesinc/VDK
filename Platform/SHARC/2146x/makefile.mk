#
#   $RCSfile$
#
#   Description: Makefile for compiling the 2146X
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
ECHO=@echo -n
DOS_ENDL   = @cmd /c echo.

###########################################################
# Setup the variables that are passed to sub calls of make
# These lines should change as they progress up the calls,
# but should basically remain the same from level to level
###########################################################
CXX			= ${UP_CXX}	
DEFINES		= ${UP_DEFINES} -D VERSION_FAMILY_="VDK::kSHARC" -DDO_40_BIT 
DEVICE_CXXFLAGS	= ${UP_CXXFLAGS} ${INCLUDE_PATH} 
CXXFLAGS	= ${UP_CXXFLAGS} ${INCLUDE_PATH} -double-size-any
ASM_SRC		= VDK_ISR_API.asm VDK_ISR_EVENTS_API.asm  VDK_ISR_SEMAPHORES_API.asm VDK_ISR_DEVICES_API.asm SlowIdle.asm VDK_ISR_API_common.asm VDK_CISR_EVENTS_API.asm  VDK_CISR_SEMAPHORES_API.asm VDK_CISR_DEVICES_API.asm DebugLogging.asm UserHistoryLog.asm 
INSTALL_HDRS= ${CURDIR}/VDK*.h
SRC			= ${UP_SRC}	${ASM_SRC}
ifeq (${NESTED_MAKE},YES)
OBJS            = ${UP_OBJS}
else
OBJS            = ${UP_OBJS} ${ASM_SRC:.asm=.doj}
endif
VPATH		= ${CURDIR}:${UP_VPATH}
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2146x)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
ASM                     = EASM21K
ASMFLAGS        = ${UP_ASMFLAGS} -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30 -double-size-any  -Wwarn-error

ifeq (${EXTRA_SUFFIX},_NW)
VEROPTIONS = -nwc
CXXFLAGS        += -nwc
DEVICE_CXXFLAGS += -nwc
ASMFLAGS        += -nwc
SUFFIX = _nwc
else
ASMFLAGS        +=-D__SHORT_WORD_CODE__
endif

NEW_FLAGS	= UP_CXX="${CXX}"					\
			  UP_CXXFLAGS="${DEVICE_CXXFLAGS}"		\
			  TOP_DIR="${TOP_DIR}"			\
			  UP_OBJS="${OBJS}"					\
			  UP_VPATH="${VPATH}"				\
			  UP_DEFINES="${DEFINES}"			\
			  UP_OUT_DIR="${OUT_DIR}"			\
			  FAMILY="${FAMILY}"				\
			  CORE="${CORE}"					\
			  DEVICE="${DEVICE}"                            \
			  REVISION="${REVISION}"                            \
			  UP_INCLUDE_PATH="${INCLUDE_PATH}"	\
			  EXTRA_SUFFIX="${EXTRA_SUFFIX}"	\
			  VDSP_VERSION="${VDSP_VERSION}"

INSTALL_DIR		= ${UP_INSTALL_DIR}/214xx

OUTPUT		= ${OUT_DIR}2146X${SUFFIX}.dlb


#
# Projects
#
all:		default
#install: install_base 
#install_debug: install_debug_base 
ifeq (${DEVICE},)
default:	build_2146x build_21469
clean:		clean_21469
squeaky:	squeaky_21462 squeaky_21465 squeaky_21467 squeaky_21469
dist_clean:	dist_clean_21462 dist_clean_21465 dist_clean_21467 dist_clean_21469 
install:	install_2146x install_21462 install_21465 install_21467 install_21469
install_debug:	install_debug_2146x install_debug_21462 install_debug_21465 install_debug_21467 install_debug_21469

OFFSET_DEVICE	= 21469
else
ifeq (${DEVICE},21462)
default:	build_2146x build_21469
clean:		clean_21469
squeaky:	squeaky_21462
dist_clean:	dist_clean_21462
install:	install_2146x install_21462
install_debug:	install_debug_2146x install_debug_21462
OFFSET_DEVICE	= 21462
else
ifeq (${DEVICE},21465)
default:	build_2146x build_21469 
clean:		clean_21469 
squeaky:	squeaky_21465 
dist_clean:	dist_clean_21465
install:	install_2146x install_21465
install_debug:	install_debug_2146x install_debug_21465
OFFSET_DEVICE	= 21465
else
ifeq (${DEVICE},21467)
default:        build_2146x build_21469
clean:          clean_21469
squeaky:        squeaky_21467
dist_clean:     dist_clean_21467
install:        install_2146x install_21467
install_debug:  install_debug_2146x install_debug_21467
OFFSET_DEVICE   = 21467
else
ifeq (${DEVICE},21469)
default:        build_2146x build_21469
clean:          clean_21469
squeaky:        squeaky_21469
dist_clean:     dist_clean_21469
install:        install_2146x install_21469
install_debug:  install_debug_2146x install_debug_21469
OFFSET_DEVICE   = 21469
else
default:
clean:
squeaky:	squeaky_21469
dist_clean:	dist_clean_21469
install:	install_2146x install_21469
install_debug:	install_debug_2146x install_debug_21469
OFFSET_DEVICE	= 21469
endif
endif
endif
endif
endif

build_2146x_libs: ${OUTPUT}

ifeq (${REVISION},all)
build_2146x: build_2146x_any build_2146x_none
install_2146x: install_2146x_any install_2146x_none
install_debug_2146x: install_debug_2146x_any install_debug_2146x_none
DIRECTORIES=2146x_rev_any 2146x_rev_none
else
ifeq (${REVISION},none)
build_2146x: build_2146x_none
install_2146x: install_2146x_none
install_debug_2146x: install_debug_2146x_none
DIRECTORIES=2146x_rev_none
else
ifeq (${REVISION},any)
build_2146x: build_2146x_any
install_2146x: install_2146x_any
DIRECTORIES=2146x_rev_any
install_debug_2146x: install_debug_2146x_any
else
build_2146x: build_2146x_any build_2146x_none
install_2146x: install_2146x_any install_2146x_none
install_debug_2146x: install_debug_2146x_any install_debug_2146x_none
DIRECTORIES=2146x_rev_any 2146x_rev_none
endif
endif
endif

# Any libraries
build_2146x_any:
	${MKDIR} 2146x_rev_any
	"${MAKE}" -f../makefile.mk  -C 2146x_rev_any build_2146x_libs ${NEW_FLAGS}  PLATFORM=ADSP-21469 VEROPTIONS+="-si-revision any" UP_ASMFLAGS+="-proc ADSP-21469 -si-revision any " UP_CXXFLAGS+="-proc ADSP-21469 -si-revision any " REVISION=any NESTED_MAKE=YES

install_2146x_any: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2146x_rev_any" INST_DIR="21469_rev_any"

install_debug_2146x_any: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2146x_rev_any" INST_DIR="21469_rev_any"

# None libraries
build_2146x_none:
	${MKDIR} 2146x_rev_none
	"${MAKE}" -f../makefile.mk  -C 2146x_rev_none build_2146x_libs ${NEW_FLAGS} PLATFORM=ADSP-21469 VEROPTIONS+="-si-revision none"  UP_ASMFLAGS+="-proc ADSP-21469 -si-revision none " UP_CXXFLAGS+="-proc ADSP-21469 -si-revision none " REVISION=none NESTED_MAKE=YES

install_2146x_none: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2146x_rev_none" INST_DIR=""

install_debug_2146x_none: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2146x_rev_none" INST_DIR=""


######
# 2146X dlb build
#
${OUTPUT}:			${OBJS} ver.txt
	@${MKDIR} debug
	@${MKDIR} release
	elfar -tx ver.txt -c "${OUTPUT}" ${OBJS}
	@echo "****************************************"
	@echo "${OUTPUT} build completed at:"
	@date +%c
	@echo "****************************************"

######
# Generate the version information. It will be different in all libs
#
 
ver.txt: 
	${ECHO} "" > $@
	${ECHO} "Library_Built: " >> $@
	@date +%c >>$@ 
	${DOS_ENDL} >> $@
	${ECHO}   "Library_Name: VDSP++ Kernel Library" >>$@ 
	${DOS_ENDL} >> $@
	${ECHO} "Library_Version: ${LIB_VERSION}"  >>$@ 
	${DOS_ENDL} >> $@
	${ECHO}  "Platform: ${PLATFORM}"   >> $@ 
	${DOS_ENDL} >> $@
	${ECHO}   "SpecialOptions: ${VEROPTIONS}"  >> $@ 
	${DOS_ENDL} >> $@

#####
# Clean
#
clean:
	${foreach dir, ${DIRECTORIES}, \
		${MKDIR} ${dir}; \
		cd ${dir}; \
		${RM} ${OBJS}; \
		${RM} *.ii; \
		${RM} *.ti; \
		${RM} ver.txt; \
		cd ..;}
squeaky:
	${RM} GENERATE_OFFSETS.DXE
	${RM} ${OBJS}
	${RM} *.ii
	${RM} *.ti
	${RM} debug/*
	${RM} release/*
	${RM} ver.txt
dist_clean:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/include/${notdir ${file}}; } 
	${RM} ${OBJS}
	${RM} *.ii
	${RM} *.ti
	${RM} ${INSTALL_DIR}/lib/VDK-*-2146X*.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

#
# Device Specific
#

#
# 21462
#
squeaky_21462:
	"${MAKE}" -fmakefile.mk -C21462 squeaky ${NEW_FLAGS}
dist_clean_21462:
	"${MAKE}" -fmakefile.mk -C21462 dist_clean ${NEW_FLAGS}
install_21462:
	"${MAKE}" -fmakefile.mk -C21462 install ${NEW_FLAGS}
install_debug_21462:
	"${MAKE}" -fmakefile.mk -C21462 install_debug ${NEW_FLAGS}

#
# 21465
#
squeaky_21465:
	"${MAKE}" -fmakefile.mk -C21465 squeaky ${NEW_FLAGS}
dist_clean_21465:
	"${MAKE}" -fmakefile.mk -C21465 dist_clean ${NEW_FLAGS}
install_21465:
	"${MAKE}" -fmakefile.mk -C21465 install ${NEW_FLAGS}
install_debug_21465:
	"${MAKE}" -fmakefile.mk -C21465 install_debug ${NEW_FLAGS}

#
# 21467
#
squeaky_21467:
	"${MAKE}" -fmakefile.mk -C21467 squeaky ${NEW_FLAGS}
dist_clean_21467:
	"${MAKE}" -fmakefile.mk -C21467 dist_clean ${NEW_FLAGS}
install_21467:
	"${MAKE}" -fmakefile.mk -C21467 install ${NEW_FLAGS}
install_debug_21467:
	"${MAKE}" -fmakefile.mk -C21467 install_debug ${NEW_FLAGS}

#
# 21469
#
build_21469: 
	"${MAKE}" -fmakefile.mk -C21469 ${NEW_FLAGS}
clean_21469:
	"${MAKE}" -fmakefile.mk -C21469 clean ${NEW_FLAGS}
squeaky_21469:
	"${MAKE}" -fmakefile.mk -C21469 squeaky ${NEW_FLAGS}
dist_clean_21469:
	"${MAKE}" -fmakefile.mk -C21469 dist_clean ${NEW_FLAGS}
install_21469:
	"${MAKE}" -fmakefile.mk -C21469 install ${NEW_FLAGS}
install_debug_21469:
	"${MAKE}" -fmakefile.mk -C21469 install_debug ${NEW_FLAGS}

#
# Setup some flags for assembly access
#
OFFSET_CXXFLAGS = ${subst " -c ",,${CXXFLAGS}}
offsets:                asm_offsets.h
asm_offsets.h:  GENERATE_OFFSETS.DXE
	../../common/generate_offsets.bat ADSP-${OFFSET_DEVICE} $< ${VDSP_VERSION} | sed "/Could not locate DM range/d" > asm_offsets.h
GENERATE_OFFSETS.DXE:   generate_offsets.cpp
	${CXX} -proc ADSP-21469 ${OFFSET_CXXFLAGS} ${DEFINES} ${INCLUDE_PATH} -g `cygpath -w $<` -Wremark 265
squeaky_offsets:
	${RM} asm_offsets.h generate_offsets.dxe

#
# Defaults
#
install_debug_base:
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/lib/${INST_DIR}
	${CP} ../../common/templates//*.tf ${INSTALL_DIR}/vdk
	${CP} ../templates/VDK_ISR_Cpp_Source.cpp.tf ${INSTALL_DIR}/vdk/VDK_ISR_CPP_Source.cpp.tf
	${CP} ../templates/*.tf ${INSTALL_DIR}/vdk
	${CP} ${SI_REV}/debug/VDK-*-2146X*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

install_base: install_debug_base
	${CP} ${SI_REV}/release/VDK-*-2146X*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

dist_clean_base:
	${RM} ${INSTALL_DIR}/include/${INSTALL_HDRS}
	${RM} ${INSTALL_DIR}/vdk//*.tf
	${RMDIR} ${INSTALL_DIR}/vdk
	${RM} ${OBJS}
	${RM} *.ii
