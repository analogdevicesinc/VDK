#
#   $RCSfile$
#
#   Description: Makefile for compiling the 2137X
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
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2137x)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
ASM                     = EASM21K
ifeq (${VDSP_VERSION},4.0)
ASMFLAGS        = ${UP_ASMFLAGS} -double-size-any  -Wwarn-error
else
ASMFLAGS        = ${UP_ASMFLAGS} -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30 -double-size-any -Wwarn-error
endif
NEW_FLAGS	= UP_CXX="${CXX}"					\
			  UP_CXXFLAGS="${DEVICE_CXXFLAGS}"		\
			  TOP_DIR="${TOP_DIR}"			\
			  UP_OBJS="${OBJS}"				\
			  UP_VPATH="${VPATH}"				\
			  UP_DEFINES="${DEFINES}"			\
			  UP_OUT_DIR="${OUT_DIR}"			\
			  FAMILY="${FAMILY}"				\
			  CORE="${CORE}"				\
			  DEVICE="${DEVICE}"                            \
			  REVISION="${REVISION}"                        \
			  UP_INCLUDE_PATH="${INCLUDE_PATH}"	\
			  VDSP_VERSION="${VDSP_VERSION}"

INSTALL_DIR		= ${UP_INSTALL_DIR}/213xx

OUTPUT		= ${OUT_DIR}2137X.dlb


#
# Projects
#
all:		default
#install: install_base 
#install_debug: install_debug_base 
ifeq (${DEVICE},)
default:	build_2137x build_21371
clean:		clean_21371 
squeaky:	squeaky_21371 squeaky_21375
dist_clean:	dist_clean_21371 dist_clean_21375
install:	install_2137x install_21371 install_21375
install_debug:	install_debug_2176x install_debug_21371 install_debug_21375

OFFSET_DEVICE	= 21371
else
ifeq (${DEVICE},21371)
default:        build_2137x build_21371
clean:          clean_21371
squeaky:        squeaky_21371
dist_clean:     dist_clean_21371
install:        install_2137x install_21371
install_debug:  install_debug_2137x install_debug_21371
OFFSET_DEVICE   = 21371
else
ifeq (${DEVICE},21375)
default:        build_2137x build_21371
clean:          clean_21371
squeaky:        squeaky_21375
dist_clean:     dist_clean_21375
install:        install_2137x install_21375
install_debug:  install_debug_2137x install_debug_21375
OFFSET_DEVICE   = 21375
else
default:
clean:
squeaky:	squeaky_21371
dist_clean:	dist_clean_21371
install:	install_2137x install_21371
install_debug:	install_debug_2137x install_debug_21371
OFFSET_DEVICE	= 21371
endif
endif
endif

build_2137x_libs: ${OUTPUT}

ifeq (${REVISION},all)
build_2137x: build_2137x_0_0 build_2137x_any build_2137x_none
install_2137x: install_2137x_0_0 install_2137x_none install_2137x_any 
install_debug_2137x: install_debug_2137x_0_0 install_debug_2137x_any install_debug_2137x_none
DIRECTORIES=2137x_rev_0.0 2137x_rev_any 2137x_rev_none
else
ifeq (${REVISION},0.0)
build_2137x: build_2137x_0_0
install_2137x: install_2137x_0_0
install_debug_2137x: install_debug_2137x_0_0
DIRECTORIES=2137x_rev_0.0
else
ifeq (${REVISION},none)
build_2137x: build_2137x_none
install_2137x: install_2137x_none
install_debug_2137x: install_debug_2137x_none
DIRECTORIES=2137x_rev_none
else
ifeq (${REVISION},any)
build_2137x: build_2137x_any
install_2137x: install_2137x_any
DIRECTORIES=2137x_rev_any
install_debug_2137x: install_debug_2137x_any
else
build_2137x: build_2137x_0_0 build_2137x_any build_2137x_none
install_2137x: install_2137x_0_0 install_2137x_any install_2137x_none
install_debug_2137x: install_debug_2137x_0_0 install_debug_2137x_any install_debug_2137x_none
DIRECTORIES=2137x_rev_0.0 2137x_rev_any 2137x_rev_none
endif
endif
endif
endif

##############################
# Silicon revisions 0.0 and 0.1 should be included here
build_2137x_0_0:
	${MKDIR} 2137x_rev_0.0
	"${MAKE}" -f../makefile.mk  -C 2137x_rev_0.0 build_2137x_libs ${NEW_FLAGS}  UP_ASMFLAGS+="-proc ADSP-21371 -si-revision 0.0" PLATFORM=ADSP-21371 VEROPTIONS="-si-revision 0.0" UP_CXXFLAGS+="-proc ADSP-21371 -si-revision 0.0 " REVISION=0.0 NESTED_MAKE=YES

install_2137x_0_0: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2137x_rev_0.0" INST_DIR="2136x_rev_0.0"

install_debug_2137x_0_0: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2137x_rev_0.0" INST_DIR="2136x_rev_0.0"

# Any libraries
build_2137x_any:
	${MKDIR} 2137x_rev_any
	"${MAKE}" -f../makefile.mk  -C 2137x_rev_any build_2137x_libs ${NEW_FLAGS}  UP_ASMFLAGS+="-proc ADSP-21371 -si-revision any " PLATFORM=ADSP-21371 VEROPTIONS="-si-revision any" UP_CXXFLAGS+="-proc ADSP-21371 -si-revision any " REVISION=any NESTED_MAKE=YES

install_2137x_any: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2137x_rev_any" INST_DIR="2136x_any"

install_debug_2137x_any: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2137x_rev_any" INST_DIR="2136x_any"

# None libraries
build_2137x_none:
	${MKDIR} 2137x_rev_none
	"${MAKE}" -f../makefile.mk  -C 2137x_rev_none build_2137x_libs ${NEW_FLAGS}  UP_ASMFLAGS+="-proc ADSP-21371 -si-revision none " PLATFORM=ADSP-21371 VEROPTIONS="-si-revision none" UP_CXXFLAGS+="-proc ADSP-21371 -si-revision none " REVISION=none NESTED_MAKE=YES

install_2137x_none: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2137x_rev_none" INST_DIR=""

install_debug_2137x_none: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2137x_rev_none" INST_DIR=""


######
# 2137X dlb build
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
	${ECHO}  "SpecialOptions: ${VEROPTIONS}"  >> $@ 
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
		${RM} *.dlb; \
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
	${RM} ${INSTALL_DIR}/lib/VDK-*-2137X*.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

#
# Device Specific
#

#
# 21371
#
build_21371: 
	"${MAKE}" -fmakefile.mk -C21371 ${NEW_FLAGS}
clean_21371:
	"${MAKE}" -fmakefile.mk -C21371 clean ${NEW_FLAGS}
squeaky_21371:
	"${MAKE}" -fmakefile.mk -C21371 squeaky ${NEW_FLAGS}
dist_clean_21371:
	"${MAKE}" -fmakefile.mk -C21371 dist_clean ${NEW_FLAGS}
install_21371:
	"${MAKE}" -fmakefile.mk -C21371 install ${NEW_FLAGS}
install_debug_21371:
	"${MAKE}" -fmakefile.mk -C21371 install_debug ${NEW_FLAGS}

#
# 21375
#
squeaky_21375:
	"${MAKE}" -fmakefile.mk -C21375 squeaky ${NEW_FLAGS}
dist_clean_21375:
	"${MAKE}" -fmakefile.mk -C21375 dist_clean ${NEW_FLAGS}
install_21375:
	"${MAKE}" -fmakefile.mk -C21375 install ${NEW_FLAGS}
install_debug_21375:
	"${MAKE}" -fmakefile.mk -C21375 install_debug ${NEW_FLAGS}

#
# Setup some flags for assembly access
#
OFFSET_CXXFLAGS = ${subst " -c ",,${CXXFLAGS}}
offsets:                asm_offsets.h
asm_offsets.h:  GENERATE_OFFSETS.DXE
	../../common/generate_offsets.bat ADSP-${OFFSET_DEVICE} $< ${VDSP_VERSION} > asm_offsets.h
GENERATE_OFFSETS.DXE:   generate_offsets.cpp
	${CXX} -proc ADSP-21371 -flags-link -e ${OFFSET_CXXFLAGS} ${DEFINES} ${INCLUDE_PATH} -g `cygpath -w $<` -Wremark 265
squeaky_offsets:
	${RM} asm_offsets.h generate_offsets.dxe

#
# Defaults
#
install_debug_base:
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/lib/${INST_DIR}
	${CP} ../../common/templates//*.tf ${INSTALL_DIR}/vdk
	${CP} ../templates/*.tf ${INSTALL_DIR}/vdk
	${CP} templates/*.tf ${INSTALL_DIR}/vdk
	${CP} ${SI_REV}/debug/VDK-*-2137X*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

install_base: install_debug_base
	${CP} ${SI_REV}/release/VDK-*-2137X*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

dist_clean_base:
	${RM} ${INSTALL_DIR}/include/${INSTALL_HDRS}
	${RM} ${INSTALL_DIR}/vdk//*.tf
	${RMDIR} ${INSTALL_DIR}/vdk
	${RM} ${OBJS}
	${RM} *.ii
