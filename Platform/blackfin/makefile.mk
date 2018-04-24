#
#   $RCSfile$
#
#   Description: Makefile for compiling blackfin
#
#   Last modified $Date$
#	Last modified by $Author$
#   $Revision$
#   $Source$
#
# -----------------------------------------------------------------------------
#  Comments:
# -----------------------------------------------------------------------------
#  Copyright (c) 2006 Analog Devices, Inc.,  All rights reserved
#
#  This software is the property of Analog Devices, Inc.
#
#  ANALOG DEVICES (ADI) MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
#  REGARD TO THIS SOFTWARE.  IN NO EVENT SHALL ADI BE LIABLE FOR INCIDENTAL
#  OR CONSEQUENTIAL DAMAGES IN CONNECTION WITH OR ARISING FROM THE FURNISHING,
#  PERFORMANCE, OR USE OF THIS SOFTWARE.
#

###########################################################
# Setup the variables that are passed to sub calls of make
# These lines should change as they progress up the calls,
# but should basically remain the same from level to level
###########################################################
VPATH		= ${CURDIR}:${UP_VPATH}
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH		= -I '$(shell cygpath -w ${TOP_DIR}/blackfin)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH		= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif

DEFINES		= ${UP_DEFINES} -DVERSION_FAMILY_="VDK::kBLACKFIN" \
				-DVDK_INT_P_=EVT_IVG14_P
CXXFLAGS	= ${UP_CXXFLAGS} -double-size-any
INSTALL_HDRS= ${UP_INSTALL_HDRS}
ASMFLAGS	= -w -stallcheck=none ${DEFINES} -double-size-any -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30
ASM_OBJS	= ${ASM_SRC:.asm=.doj}
SRC			= ${UP_SRC}	${ASM_SRC}
OBJS		= ${UP_OBJS} ${ASM_OBJS}
NEW_FLAGS	= UP_CXX="${CXX}"					\
			  UP_CXXFLAGS="${CXXFLAGS}"		\
			  TOP_DIR="${TOP_DIR}"		\
			  UP_VPATH="${VPATH}"				\
			  OUT_DIR="${OUT_DIR}"		   		\
			  FAMILY="${FAMILY}"				\
			  UP_DEFINES="${DEFINES}"			\
			  CORE="${CORE}"					\
			  DEVICE="${DEVICE}"				\
			  UP_ASMFLAGS="${ASMFLAGS}"			\
			  UP_INCLUDE_PATH="${INCLUDE_PATH}" \
			  ADI_MAKE="${ADI_MAKE}" \
			  API_VERSION="${API_VERSION}" \
			  LIB_VERSION="${LIB_VERSION}" \
			  UP_OBJS="${OBJS}"			\
				VDSP_VERSION="${VDSP_VERSION}"

INSTALL_FLAGS	= INSTALL_DIR="${INSTALL_DIR}"
INSTALL_DIR		= ${UP_INSTALL_DIR}/BLACKFIN

#
# Projects
#
all:		default
ifeq (${CORE},)
default:		build_BF5xx
clean:			clean_BF5xx
squeaky:		squeaky_BF5xx
dist_clean:		dist_clean_BF5xx
install:		install_BF5xx
install_debug:	install_debug_BF5xx
squeaky_offsets: squeaky_offsets_BF5xx
offsets: offsets_BF5xx
else
ifeq (${CORE},10x)
default:		build_BF5xx
clean:			clean_BF5xx
squeaky:		squeaky_BF5xx
dist_clean:		dist_clean_BF5xx
install:		install_BF5xx
install_debug:	install_debug_BF5xx
squeaky_offsets: squeaky_offsets_BF5xx
offsets: offsets_BF5xx
endif
endif

#
# Core Specific
#

build_BF5xx:
	"${MAKE}" -fmakefile.mk -C2153X ${NEW_FLAGS} REVISION=${REVISION}
clean_BF5xx:
	"${MAKE}" -fmakefile.mk -C2153X clean ${NEW_FLAGS}
squeaky_BF5xx:
	"${MAKE}" -fmakefile.mk -C2153X squeaky ${NEW_FLAGS}
dist_clean_BF5xx:
	"${MAKE}" -fmakefile.mk -C2153X dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF5xx:
	"${MAKE}" -fmakefile.mk -C2153X install ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_debug_BF5xx:
	"${MAKE}" -fmakefile.mk -C2153X install_debug ${NEW_FLAGS} ${INSTALL_FLAGS} -i
squeaky_offsets_BF5xx:
	"${MAKE}" -fmakefile.mk -C2153X squeaky_offsets ${NEW_FLAGS} ${INSTALL_FLAGS} -i
offsets_BF5xx:
	"${MAKE}" -fmakefile.mk -C2153X offsets ${NEW_FLAGS} ${INSTALL_FLAGS} -i
