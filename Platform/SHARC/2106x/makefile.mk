#
#   $RCSfile$
#
#   Description: Makefile for compiling the 2106X
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
	${CXX} -proc ADSP-21060 -si-revision any -c ${CXXFLAGS} ${DEFINES} `cygpath -w $<` -o $@


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
PLATFORM=ADSP-21060
CXX			= ${UP_CXX}
DEFINES		= ${UP_DEFINES} -D VERSION_FAMILY_="VDK::kSHARC"
CXXFLAGS	= ${UP_CXXFLAGS} ${INCLUDE_PATH} -double-size-any 
ASM_SRC		= VDK_ISR_API.asm VDK_ISR_EVENTS_API.asm  VDK_ISR_SEMAPHORES_API.asm VDK_ISR_DEVICES_API.asm SlowIdle.asm VDK_ISR_API_common.asm VDK_CISR_EVENTS_API.asm  VDK_CISR_SEMAPHORES_API.asm VDK_CISR_DEVICES_API.asm DebugLogging.asm UserHistoryLog.asm 
INSTALL_HDRS= ${CURDIR}/VDK*.h
SRC			= ${UP_SRC}	${ASM_SRC}
OBJS		= ${UP_OBJS} ${ASM_SRC:.asm=.doj}
VPATH		= ${CURDIR}:${UP_VPATH}
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2106x)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
ASM                     = EASM21K
ifeq (${VDSP_VERSION},4.0)
ASMFLAGS        = -21060
else
ASMFLAGS        = -21060 -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30 -double-size-any -si-revision any -Wwarn-error
endif
NEW_FLAGS	= UP_CXX="${CXX}"					\
			  UP_CXXFLAGS="${CXXFLAGS}"			\
			  TOP_DIR="${TOP_DIR}"			\
			  UP_OBJS="${OBJS}"				\
			  UP_VPATH="${VPATH}"				\
			  UP_DEFINES="${DEFINES}"			\
			  OUT_DIR="${OUT_DIR}"				\
			  FAMILY="${FAMILY}"				\
			  CORE="${CORE}"				\
			  DEVICE="${DEVICE}"				\
			  UP_INCLUDE_PATH="${INCLUDE_PATH}"	\
			  VDSP_VERSION="${VDSP_VERSION}"
INSTALL_FLAGS	= INSTALL_DIR="${INSTALL_DIR}"
INSTALL_DIR = ${UP_INSTALL_DIR}/21k
OUTPUT		= ${OUT_DIR}2106X.dlb

#
# Projects
#
all:		default
install:	install_base
install_debug:	install_base_debug
ifeq (${DEVICE},)
default:		build_21060			build_21065
clean:			clean_21060			clean_21065
squeaky:		squeaky_21060 		squeaky_21065 squeaky_21061 squeaky_21062
dist_clean:		dist_clean_21060 dist_clean_21061 dist_clean_21062 dist_clean_21065
install:		install_21060 install_21061 install_21062 install_21065
install_debug:	install_debug_21060 install_debug_21061 install_debug_21062 install_debug_21065
OFFSET_DEVICE	= 21060
else
ifeq (${DEVICE},21060)
default:		build_21060
clean:			clean_21060
squeaky:		squeaky_21060
dist_clean:		dist_clean_21060
install:		install_21060
install_debug:	install_debug_21060
OFFSET_DEVICE	= 21060
else
ifeq (${DEVICE},21061)
default:		build_21061
clean:			clean_21061
squeaky:		squeaky_21061
dist_clean:		dist_clean_21061
install:		install_21061
install_debug:	install_debug_21061
OFFSET_DEVICE	= 21061
else
ifeq (${DEVICE},21062)
default:		build_21062
clean:			clean_21062
squeaky:		squeaky_21062
dist_clean:		dist_clean_21062
install:		install_21062
install_debug:	install_debug_21062
OFFSET_DEVICE	= 21062
else
ifeq (${DEVICE},21065)
default:		build_21065
clean:			clean_21065
squeaky:		squeaky_21065
dist_clean:		dist_clean_21065
install:		install_21065
install_debug:	install_debug_21065
OFFSET_DEVICE	= 21060
else
default:
clean:
endif
endif
endif
endif
endif

######
# 2106X dlb build
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
	${RM} ${OBJS} 
	${RM} *.ii
	${RM} ver.txt
squeaky:
	${RM} GENERATE_OFFSETS.DXE
	${RM} ${OBJS} 
	${RM} *.ii
	${RM} *.ti
	${RM} ver.txt
	${RM} debug/*
	${RM} release/*
dist_clean:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/include/${notdir ${file}}; } 
	${RM} ${OBJS}
	${RM} *.ii
	${RM} ${INSTALL_DIR}/lib/VDK-*-2106X.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

#
# Device Specific
#

#
# 21060
#
build_21060: ${OUTPUT}
	"${MAKE}" -fmakefile.mk -C21060 ${NEW_FLAGS}
clean_21060:
	"${MAKE}" -fmakefile.mk -C21060 clean ${NEW_FLAGS}
squeaky_21060:
	"${MAKE}" -fmakefile.mk -C21060 squeaky ${NEW_FLAGS}
dist_clean_21060:
	"${MAKE}" -fmakefile.mk -C21060 dist_clean ${NEW_FLAGS} --ignore-errors
install_21060:
	"${MAKE}" -fmakefile.mk -C21060 install ${INSTALL_FLAGS} --ignore-errors
install_debug_21060:
	"${MAKE}" -fmakefile.mk -C21060 install_debug ${INSTALL_FLAGS} --ignore-errors

# 21061 and 21062 have the same build, clean and squeaky as 21060 because
# they use the same libraries. The only difference will be in the .ldf files
# so we need a different install and dist_clean

#
# 21061
#
build_21061: ${OUTPUT} build_21060
clean_21061: clean_21060
squeaky_21061: squeaky_21060
	"${MAKE}" -fmakefile.mk -C21061 squeaky ${NEW_FLAGS}
dist_clean_21061: 
	"${MAKE}" -fmakefile.mk -C21061 dist_clean ${NEW_FLAGS} --ignore-errors
install_21061: install_21060
	"${MAKE}" -fmakefile.mk -C21061 install ${INSTALL_FLAGS} --ignore-errors
install_debug_21061: install_debug_21060
	"${MAKE}" -fmakefile.mk -C21061 install_debug ${INSTALL_FLAGS} --ignore-errors

#
# 21062
#
build_21062: ${OUTPUT} build_21060
clean_21062: clean_21060
squeaky_21062: squeaky_21060
	"${MAKE}" -fmakefile.mk -C21062 squeaky ${NEW_FLAGS}
dist_clean_21062: 
	"${MAKE}" -fmakefile.mk -C21062 dist_clean ${NEW_FLAGS} --ignore-errors
install_21062: install_21060
	"${MAKE}" -fmakefile.mk -C21062 install ${INSTALL_FLAGS} --ignore-errors
install_debug_21062: install_debug_21060
	"${MAKE}" -fmakefile.mk -C21062 install_debug ${INSTALL_FLAGS} --ignore-errors
#
# 21065
#
build_21065: ${OUTPUT}
	"${MAKE}" -fmakefile.mk -C21065 ${NEW_FLAGS}
clean_21065:
	"${MAKE}" -fmakefile.mk -C21065 clean ${NEW_FLAGS}
squeaky_21065:
	"${MAKE}" -fmakefile.mk -C21065 squeaky ${NEW_FLAGS}
dist_clean_21065:
	"${MAKE}" -fmakefile.mk -C21065 dist_clean ${NEW_FLAGS} --ignore-errors
install_21065:
	"${MAKE}" -fmakefile.mk -C21065 install ${INSTALL_FLAGS} --ignore-errors
install_debug_21065:
	"${MAKE}" -fmakefile.mk -C21065 install_debug ${INSTALL_FLAGS} --ignore-errors

#
# Setup some flags for assembly access
#
OFFSET_CXXFLAGS	= ${subst " -c ",,${CXXFLAGS}}
offsets:		asm_offsets.h
asm_offsets.h:	GENERATE_OFFSETS.DXE
	../../common/generate_offsets.bat ADSP-${OFFSET_DEVICE} $< ${VDSP_VERSION} > asm_offsets.h
GENERATE_OFFSETS.DXE:	generate_offsets.cpp
	${CXX} ${OFFSET_CXXFLAGS} ${DEFINES} ${INCLUDE_PATH} -g `cygpath -w $<` -Wremark 265
squeaky_offsets:
	${RM} asm_offsets.h generate_offsets.dxe

#
# Defaults
#
install_base_debug:
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/lib
	${CP} ../../common/templates//*.tf ${INSTALL_DIR}/vdk
	${CP} ../templates/VDK_ISR_Cpp_Source.cpp.tf ${INSTALL_DIR}/vdk/VDK_ISR_CPP_Source.cpp.tf
	${CP} ../templates/*.tf ${INSTALL_DIR}/vdk
	${CP} debug/VDK-*-2106X.dlb ${INSTALL_DIR}/lib

install_base: install_base_debug
	${CP} release/VDK-*-2106X.dlb ${INSTALL_DIR}/lib
	
dist_clean_base:
	${RM} ${INSTALL_DIR}/vdk//*.tf
	${RMDIR} ${INSTALL_DIR}/vdk
	${RM} ${INSTALL_DIR}/include/${INSTALL_HDRS}
	${RM} ${OBJS}
	${RM} *.ii


