#
#   $RCSfile$
#
#   Description: Makefile for compiling the 2116X
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
ECHO=@echo -n
DOS_ENDL   = @cmd /c echo.

###########################################################
# Setup the variables that are passed to sub calls of make
# These lines should change as they progress up the calls,
# but should basically remain the same from level to level
###########################################################
PLATFORM=ADSP-21160
CXX			= ${UP_CXX}	
DEFINES		= ${UP_DEFINES} -D VERSION_FAMILY_="VDK::kSHARC"
DEVICE_CXXFLAGS	= ${UP_CXXFLAGS} ${INCLUDE_PATH} 
CXXFLAGS	= ${UP_CXXFLAGS} ${INCLUDE_PATH} -proc ADSP-21160 -workaround rframe -workaround 21161-anomaly-45  -double-size-any -si-revision any
ASM_SRC		= VDK_ISR_API.asm VDK_ISR_EVENTS_API.asm  VDK_ISR_SEMAPHORES_API.asm VDK_ISR_DEVICES_API.asm SlowIdle.asm VDK_ISR_API_common.asm VDK_CISR_EVENTS_API.asm  VDK_CISR_SEMAPHORES_API.asm VDK_CISR_DEVICES_API.asm DebugLogging.asm UserHistoryLog.asm 
INSTALL_HDRS= ${CURDIR}/VDK*.h
SRC			= ${UP_SRC}	${ASM_SRC}
OBJS		= ${UP_OBJS} ${ASM_SRC:.asm=.doj}
VPATH		= ${CURDIR}:${UP_VPATH}
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2116x)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
ASM                     = EASM21K
ifeq (${VDSP_VERSION},4.0)
ASMFLAGS        = -21160 
else
ASMFLAGS        = -21160 -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30 -double-size-any -si-revision any -Wwarn-error
endif
ifeq (${EXTRA_SUFFIX},_SWF)
VEROPTIONS = -workaround sfa 
CXXFLAGS        += -workaround swfa
DEVICE_CXXFLAGS += -workaround swfa
ASMFLAGS        += -swf_screening_fix -D_ADI_SWFA  -Wsuppress 2500
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
			  EXTRA_SUFFIX="${EXTRA_SUFFIX}"            \
			  UP_INCLUDE_PATH="${INCLUDE_PATH}"	\
			  VDSP_VERSION="${VDSP_VERSION}"

INSTALL_DIR		= ${UP_INSTALL_DIR}/211x

OUTPUT		= ${OUT_DIR}2116X${EXTRA_SUFFIX}.dlb


#
# Projects
#
all:		default
install: install_base
install_debug: install_debug_base  
ifeq (${DEVICE},)
default:	build_21160 build_21161
clean:		clean_21160 clean_21161
squeaky:	squeaky_21160 squeaky_21161
dist_clean:	dist_clean_21160 dist_clean_21161
install:	install_21160 install_21161
install_debug:	install_debug_21160 install_debug_21161
OFFSET_DEVICE	= 21160
else
ifeq (${DEVICE},21160)
default:	build_21160
clean:		clean_21160
squeaky:	squeaky_21160
dist_clean:	dist_clean_21160
install:	install_21160
install_debug:	install_debug_21160
OFFSET_DEVICE	= 21160
else
ifeq (${DEVICE},21161)
default:	build_21161
clean:		clean_21161
squeaky:	squeaky_21161
dist_clean:	dist_clean_21161
install:	install_21161
install_debug:	install_debug_21161
OFFSET_DEVICE	= 21161
else
default:
clean:
squeaky:	squeaky_21160
dist_clean:	dist_clean_21160
install:	install_21160
install_debug:	install_debug_21160
OFFSET_DEVICE	= 21160
endif
endif
endif

######
# 2116X dlb build
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
	${RM} ${OBJS}
	${RM} *.ii
	${RM} ver.txt
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
	${RM} ${INSTALL_DIR}/lib/VDK-*-2116X*.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

#
# Device Specific
#

#
# 21160
#
build_21160: ${OUTPUT}
	"${MAKE}" -fmakefile.mk -C21160 ${NEW_FLAGS}
clean_21160:
	"${MAKE}" -fmakefile.mk -C21160 clean ${NEW_FLAGS}
squeaky_21160:
	"${MAKE}" -fmakefile.mk -C21160 squeaky ${NEW_FLAGS}
dist_clean_21160:
	"${MAKE}" -fmakefile.mk -C21160 dist_clean ${NEW_FLAGS}
install_21160:
	"${MAKE}" -fmakefile.mk -C21160 install ${NEW_FLAGS}
install_debug_21160:
	"${MAKE}" -fmakefile.mk -C21160 install_debug ${NEW_FLAGS}
#
# 21161
#
build_21161: ${OUTPUT}
	"${MAKE}" -fmakefile.mk -C21161 ${NEW_FLAGS}
clean_21161:
	"${MAKE}" -fmakefile.mk -C21161 clean ${NEW_FLAGS}
squeaky_21161:
	"${MAKE}" -fmakefile.mk -C21161 squeaky ${NEW_FLAGS}
dist_clean_21161:
	"${MAKE}" -fmakefile.mk -C21161 dist_clean ${NEW_FLAGS}
install_21161:
	"${MAKE}" -fmakefile.mk -C21161 install ${NEW_FLAGS}
install_debug_21161:
	"${MAKE}" -fmakefile.mk -C21161 install_debug ${NEW_FLAGS}
#
# Setup some flags for assembly access
#
OFFSET_CXXFLAGS = ${subst " -c ",,${CXXFLAGS}}
offsets:                asm_offsets.h
asm_offsets.h:  GENERATE_OFFSETS.DXE
	../../common/generate_offsets.bat ADSP-${OFFSET_DEVICE} $< ${VDSP_VERSION} > asm_offsets.h
GENERATE_OFFSETS.DXE:   generate_offsets.cpp
	${CXX} ${OFFSET_CXXFLAGS} ${DEFINES} ${INCLUDE_PATH} -g `cygpath -w $<` -Wremark 265
squeaky_offsets:
	${RM} asm_offsets.h generate_offsets.dxe

#
# Defaults
#
install_debug_base:
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/lib
	${CP} ../../common/templates//*.tf ${INSTALL_DIR}/vdk
	${CP} ../templates/VDK_ISR_Cpp_Source.cpp.tf ${INSTALL_DIR}/vdk/VDK_ISR_CPP_Source.cpp.tf
	${CP} ../templates/*.tf ${INSTALL_DIR}/vdk
	${CP} debug/VDK-*-2116X*.dlb ${INSTALL_DIR}/lib

install_base:install_debug_base
	${CP} release/VDK-*-2116X*.dlb ${INSTALL_DIR}/lib

dist_clean_base:
	${RM} ${INSTALL_DIR}/include/${INSTALL_HDRS}
	${RM} ${INSTALL_DIR}/vdk//*.tf
	${RMDIR} ${INSTALL_DIR}/vdk
	${RM} ${OBJS}
	${RM} *.ii
