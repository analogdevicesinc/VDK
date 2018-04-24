#
#   $RCSfile$
#
#   Description: Makefile for compiling the 2126X
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
PLATFORM=ADSP-21261
CXX			= ${UP_CXX}	
DEFINES		= ${UP_DEFINES} -D VERSION_FAMILY_="VDK::kSHARC" -DDO_40_BIT
DEVICE_CXXFLAGS	= ${UP_CXXFLAGS} ${INCLUDE_PATH} 
CXXFLAGS	= ${UP_CXXFLAGS} ${INCLUDE_PATH} -proc ADSP-21261 -double-size-any
ASM_SRC		= VDK_ISR_API.asm VDK_ISR_EVENTS_API.asm  VDK_ISR_SEMAPHORES_API.asm VDK_ISR_DEVICES_API.asm SlowIdle.asm VDK_ISR_API_common.asm VDK_CISR_EVENTS_API.asm  VDK_CISR_SEMAPHORES_API.asm VDK_CISR_DEVICES_API.asm DebugLogging.asm UserHistoryLog.asm 
INSTALL_HDRS= ${CURDIR}/VDK*.h
SRC			= ${UP_SRC}	${ASM_SRC}
ifeq (${NESTED_MAKE},YES)
OBJS		= ${UP_OBJS} 
else
OBJS		= ${UP_OBJS} ${ASM_SRC:.asm=.doj}
endif
VPATH		= ${CURDIR}:${UP_VPATH}
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2126x)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
ASM                     = EASM21K
ifeq (${VDSP_VERSION},4.0)
ASMFLAGS        = -proc ADSP-21261 ${UP_ASMFLAGS}
else
ASMFLAGS        = -proc ADSP-21261 ${UP_ASMFLAGS} -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30 -double-size-any -Wwarn-error
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
			  VDSP_VERSION="${VDSP_VERSION}"

INSTALL_DIR		= ${UP_INSTALL_DIR}/212x

OUTPUT		= ${OUT_DIR}2126X.dlb


#
# Projects
#
all:		default
#install: install_base 
#install_debug: install_debug_base 
ifeq (${DEVICE},)
default:	build_2126x build_21262 build_21261
clean:		clean_21262 clean_21261
squeaky:	squeaky_21262 squeaky_21261 squeaky_21266 squeaky_21267
dist_clean:	dist_clean_21262  dist_clean_21266 dist_clean_21261 dist_clean_21267
install:	install_2126x install_21262  install_21266 install_21261 install_21267
install_debug:	install_debug_2126x install_debug_21262  install_debug_21266 install_debug_21261 install_debug_21267

OFFSET_DEVICE	= 21261
else
ifeq (${DEVICE},21262)
default:	build_2126x build_21262
clean:		clean_21262
squeaky:	squeaky_21262
dist_clean:	dist_clean_21262
install:	install_2126x install_21262
install_debug:	install_debug_2126x install_debug_21262
OFFSET_DEVICE	= 21262
else
ifeq (${DEVICE},21266)
default:	build_2126x build_21262
clean:		clean_21262
squeaky:	squeaky_21266
dist_clean:	dist_clean_21266
install:	install_2126x install_21266
install_debug:	install_debug_2126x install_debug_21266
OFFSET_DEVICE	= 21266
else
ifeq (${DEVICE},21261)
default:	build_2126x build_21262 build_21261
clean:		clean_21262 clean_21261
squeaky:	squeaky_21262 squeaky_21261
dist_clean:	dist_clean_21261
install:	install_2126x install_21261
install_debug:	install_debug_2126x install_debug_21261
OFFSET_DEVICE	= 21261
else
ifeq (${DEVICE},21267)
default:	build_2126x build_21262 build_21261
clean:		clean_21262 clean_21261
squeaky:	squeaky_21262 squeaky_21261 squeaky_21267
dist_clean:	dist_clean_21267
install:	install_2126x install_21267
install_debug:	install_debug_2126x install_debug_21267
OFFSET_DEVICE	= 21267
else
default:
clean:
squeaky:	squeaky_21262
dist_clean:	dist_clean_21262
install:	install_2126x install_21262
install_debug:	install_debug_2126x install_debug_21262
OFFSET_DEVICE	= 21262
endif
endif
endif
endif
endif

build_2126x_libs: ${OUTPUT}

ifeq (${REVISION},all)
build_2126x: build_2126x_0_0 build_2126x_any build_2126x_none
install_2126x: install_2126x_0_0 install_2126x_none install_2126x_any install_2126x_none
install_debug_2126x: install_debug_2126x_0_0 install_debug_2126x_any install_debug_2126x_none
DIRECTORIES=2126x_rev_0.0 2126x_rev_any 2126x_rev_none
else
ifeq (${REVISION},0.0)
build_2126x: build_2126x_0_0
install_2126x: install_2126x_0_0
install_debug_2126x: install_debug_2126x_0_0
DIRECTORIES=2126x_rev_0.0
else
ifeq (${REVISION},none)
build_2126x: build_2126x_none
install_2126x: install_2126x_none
install_debug_2126x: install_debug_2126x_none
DIRECTORIES=2126x_rev_none
else
ifeq (${REVISION},any)
build_2126x: build_2126x_any
install_2126x: install_2126x_any
DIRECTORIES=2126x_rev_any
install_debug_2126x: install_debug_2126x_any
else
build_2126x: build_2126x_0_0 build_2126x_any build_2126x_none
install_2126x: install_2126x_0_0 install_2126x_any install_2126x_none
install_debug_2126x: install_debug_2126x_0_0 install_debug_2126x_any install_debug_2126x_none
DIRECTORIES=2126x_rev_0.0 2126x_rev_any 2126x_rev_none
endif
endif
endif
endif

##############################
# Silicon revisions 0.0 and 0.1 should be included here
build_2126x_0_0:
	${MKDIR} 2126x_rev_0.0
	"${MAKE}" -f../makefile.mk  -C 2126x_rev_0.0 build_2126x_libs ${NEW_FLAGS}  UP_ASMFLAGS+="-si-revision 0.0 " UP_CXXFLAGS+="-si-revision 0.0 " REVISION=0.0 NESTED_MAKE=YES

install_2126x_0_0: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2126x_rev_0.0" INST_DIR="2126x_rev_0.0"

install_debug_2126x_0_0: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2126x_rev_0.0" INST_DIR="2126x_rev_0.0"

# Any libraries
build_2126x_any:
	${MKDIR} 2126x_rev_any
	"${MAKE}" -f../makefile.mk  -C 2126x_rev_any build_2126x_libs ${NEW_FLAGS}  UP_ASMFLAGS+="-si-revision any " UP_CXXFLAGS+="-si-revision any " REVISION=any NESTED_MAKE=YES

install_2126x_any: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2126x_rev_any" INST_DIR="2126x_any"

install_debug_2126x_any: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2126x_rev_any" INST_DIR="2126x_any"

# None libraries
build_2126x_none:
	${MKDIR} 2126x_rev_none
	"${MAKE}" -f../makefile.mk  -C 2126x_rev_none build_2126x_libs ${NEW_FLAGS}  UP_ASMFLAGS+="-si-revision none " UP_CXXFLAGS+="-si-revision none " REVISION=none NESTED_MAKE=YES

install_2126x_none: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2126x_rev_none" INST_DIR=""

install_debug_2126x_none: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2126x_rev_none" INST_DIR="2126x_rev_none"

######
# 2126X dlb build
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
	${RM} ${INSTALL_DIR}/lib/VDK-*-2126X*.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

#
# Device Specific
#

#
# 21262
#
build_21262: 
	"${MAKE}" -fmakefile.mk -C21262 ${NEW_FLAGS}
clean_21262:
	"${MAKE}" -fmakefile.mk -C21262 clean ${NEW_FLAGS}
squeaky_21262:
	"${MAKE}" -fmakefile.mk -C21262 squeaky ${NEW_FLAGS}
dist_clean_21262:
	"${MAKE}" -fmakefile.mk -C21262 dist_clean ${NEW_FLAGS}
install_21262:
	"${MAKE}" -fmakefile.mk -C21262 install ${NEW_FLAGS}
install_debug_21262:
	"${MAKE}" -fmakefile.mk -C21262 install_debug ${NEW_FLAGS}

#
# 21266
#
clean_21266:
	"${MAKE}" -fmakefile.mk -C21266 clean ${NEW_FLAGS}
squeaky_21266:
	"${MAKE}" -fmakefile.mk -C21266 squeaky ${NEW_FLAGS}
dist_clean_21266:
	"${MAKE}" -fmakefile.mk -C21266 dist_clean ${NEW_FLAGS}
install_21266:
	"${MAKE}" -fmakefile.mk -C21266 install ${NEW_FLAGS}
install_debug_21266:
	"${MAKE}" -fmakefile.mk -C21266 install_debug ${NEW_FLAGS}
#
# 21261
#
build_21261: 
	"${MAKE}" -fmakefile.mk -C21261 ${NEW_FLAGS}
clean_21261:
	"${MAKE}" -fmakefile.mk -C21261 clean ${NEW_FLAGS}
squeaky_21261:
	"${MAKE}" -fmakefile.mk -C21261 squeaky ${NEW_FLAGS}
dist_clean_21261:
	"${MAKE}" -fmakefile.mk -C21261 dist_clean ${NEW_FLAGS}
install_21261:
	"${MAKE}" -fmakefile.mk -C21261 install ${NEW_FLAGS}
install_debug_21261:
	"${MAKE}" -fmakefile.mk -C21261 install_debug ${NEW_FLAGS}
#
# 21267
#
clean_21267:
	"${MAKE}" -fmakefile.mk -C21267 clean ${NEW_FLAGS}
squeaky_21267:
	"${MAKE}" -fmakefile.mk -C21267 squeaky ${NEW_FLAGS}
dist_clean_21267:
	"${MAKE}" -fmakefile.mk -C21267 dist_clean ${NEW_FLAGS}
install_21267:
	"${MAKE}" -fmakefile.mk -C21267 install ${NEW_FLAGS}
install_debug_21267:
	"${MAKE}" -fmakefile.mk -C21267 install_debug ${NEW_FLAGS}

#
# Setup some flags for assembly access
#
OFFSET_CXXFLAGS = ${subst " -c ",,${CXXFLAGS}}
offsets:                asm_offsets.h
asm_offsets.h:  GENERATE_OFFSETS.DXE
	../../common/generate_offsets.bat ADSP-${OFFSET_DEVICE} $< ${VDSP_VERSION} | sed "/Could not locate DM range/d" > asm_offsets.h
GENERATE_OFFSETS.DXE:   generate_offsets.cpp
	${CXX} -proc ADSP-21261 ${OFFSET_CXXFLAGS} ${DEFINES} ${INCLUDE_PATH} -g `cygpath -w $<` -Wremark 265
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
	${CP} ${SI_REV}/debug/VDK-*-2126X*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

install_base: install_debug_base
	${CP} ${SI_REV}/release/VDK-*-2126X*.dlb ${INSTALL_DIR}/lib/${INST_DIR}


dist_clean_base:
	${RM} ${INSTALL_DIR}/include/${INSTALL_HDRS}
	${RM} ${INSTALL_DIR}/vdk//*.tf
	${RMDIR} ${INSTALL_DIR}/vdk
	${RM} ${OBJS}
	${RM} *.ii
