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
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2148x)' -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2146x)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' -I '$(shell cygpath -w ${CURDIR}../2146x)' ${UP_INCLUDE_PATH}
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
default:	
clean:		clean_21481 clean_21482 clean_21483 clean_21485  clean_21486  clean_21487 clean_21488 clean_21489
squeaky:	squeaky_21481 squeaky_21482  squeaky_21483 squeaky_21485  squeaky_21486  squeaky_21487 squeaky_21488 squeaky_21489
dist_clean:	dist_clean_21481 dist_clean_21482 dist_clean_21483 dist_clean_21485  dist_clean_21486  dist_clean_21487 dist_clean_21488 dist_clean_21489 
install:	install_21481 install_21482 install_21483 install_21485  install_21486  install_21487 install_21488 install_21489
install_debug:	install_debug_21481 install_debug_21482  install_debug_21483 install_debug_21485  install_debug_21486  install_debug_21487 install_debug_21488 install_debug_21489

OFFSET_DEVICE	= 21489
else
ifeq (${DEVICE},21481)
default:        build_21481
clean:          clean_21481
squeaky:        squeaky_21481
dist_clean:     dist_clean_21481
install:        install_21481
install_debug:  install_debug_21481
OFFSET_DEVICE   = 21481
else
ifeq (${DEVICE},21482)
default:        build_21482
clean:          clean_21482
squeaky:        squeaky_21482
dist_clean:     dist_clean_21482
install:        install_21482
install_debug:  install_debug_21482
OFFSET_DEVICE   = 21482
else
ifeq (${DEVICE},21483)
default:        build_21483
clean:          clean_21483
squeaky:        squeaky_21483
dist_clean:     dist_clean_21483
install:        install_21483
install_debug:  install_debug_21483
OFFSET_DEVICE   = 21483
else
ifeq (${DEVICE},21485)
default:        build_21485
clean:          clean_21485
squeaky:        squeaky_21485
dist_clean:     dist_clean_21485
install:        install_21485
install_debug:  install_debug_21485
OFFSET_DEVICE   = 21485
else
ifeq (${DEVICE},21486)
default:        build_21486
clean:          clean_21486
squeaky:        squeaky_21486
dist_clean:     dist_clean_21486
install:        install_21486
install_debug:  install_debug_21486
OFFSET_DEVICE   = 21486
else
ifeq (${DEVICE},21487)
default:        build_21487
clean:          clean_21487
squeaky:        squeaky_21487
dist_clean:     dist_clean_21487
install:        install_21487
install_debug:  install_debug_21487
OFFSET_DEVICE   = 21487
else
ifeq (${DEVICE},21488)
default:        build_21488
clean:          clean_21488
squeaky:        squeaky_21488
dist_clean:     dist_clean_21488
install:        install_21488
install_debug:  install_debug_21488
OFFSET_DEVICE   = 21488
else
ifeq (${DEVICE},21489)
default:        build_21489
clean:          clean_21489
squeaky:        squeaky_21489
dist_clean:     dist_clean_21489
install:        install_21489
install_debug:  install_debug_21489
OFFSET_DEVICE   = 21489
else
default:
clean:
squeaky:	squeaky_21489
dist_clean:	dist_clean_21489
install:	install_21489
install_debug:	install_debug_21489
OFFSET_DEVICE	= 21489
endif
endif
endif
endif
endif
endif
endif
endif
endif

#
# Device Specific
#

#
# 21481
#
build_21481: 
	"${MAKE}" -fmakefile.mk -C21481 ${NEW_FLAGS}
clean_21481:
	"${MAKE}" -fmakefile.mk -C21481 clean ${NEW_FLAGS}
squeaky_21481:
	"${MAKE}" -fmakefile.mk -C21481 squeaky ${NEW_FLAGS}
dist_clean_21481:
	"${MAKE}" -fmakefile.mk -C21481 dist_clean ${NEW_FLAGS}
install_21481:
	"${MAKE}" -fmakefile.mk -C21481 install ${NEW_FLAGS}
install_debug_21481:
	"${MAKE}" -fmakefile.mk -C21481 install_debug ${NEW_FLAGS}
#
# 21482
#
build_21482: 
	"${MAKE}" -fmakefile.mk -C21482 ${NEW_FLAGS}
clean_21482:
	"${MAKE}" -fmakefile.mk -C21482 clean ${NEW_FLAGS}
squeaky_21482:
	"${MAKE}" -fmakefile.mk -C21482 squeaky ${NEW_FLAGS}
dist_clean_21482:
	"${MAKE}" -fmakefile.mk -C21482 dist_clean ${NEW_FLAGS}
install_21482:
	"${MAKE}" -fmakefile.mk -C21482 install ${NEW_FLAGS}
install_debug_21482:
	"${MAKE}" -fmakefile.mk -C21482 install_debug ${NEW_FLAGS}
#
# 21483
#
build_21483: 
	"${MAKE}" -fmakefile.mk -C21483 ${NEW_FLAGS}
clean_21483:
	"${MAKE}" -fmakefile.mk -C21483 clean ${NEW_FLAGS}
squeaky_21483:
	"${MAKE}" -fmakefile.mk -C21483 squeaky ${NEW_FLAGS}
dist_clean_21483:
	"${MAKE}" -fmakefile.mk -C21483 dist_clean ${NEW_FLAGS}
install_21483:
	"${MAKE}" -fmakefile.mk -C21483 install ${NEW_FLAGS}
install_debug_21483:
	"${MAKE}" -fmakefile.mk -C21483 install_debug ${NEW_FLAGS}
#
# 21485
#
build_21485: 
	"${MAKE}" -fmakefile.mk -C21485 ${NEW_FLAGS}
clean_21485:
	"${MAKE}" -fmakefile.mk -C21485 clean ${NEW_FLAGS}
squeaky_21485:
	"${MAKE}" -fmakefile.mk -C21485 squeaky ${NEW_FLAGS}
dist_clean_21485:
	"${MAKE}" -fmakefile.mk -C21485 dist_clean ${NEW_FLAGS}
install_21485:
	"${MAKE}" -fmakefile.mk -C21485 install ${NEW_FLAGS}
install_debug_21485:
	"${MAKE}" -fmakefile.mk -C21485 install_debug ${NEW_FLAGS}
#
# 21486
#
build_21486: 
	"${MAKE}" -fmakefile.mk -C21486 ${NEW_FLAGS}
clean_21486:
	"${MAKE}" -fmakefile.mk -C21486 clean ${NEW_FLAGS}
squeaky_21486:
	"${MAKE}" -fmakefile.mk -C21486 squeaky ${NEW_FLAGS}
dist_clean_21486:
	"${MAKE}" -fmakefile.mk -C21486 dist_clean ${NEW_FLAGS}
install_21486:
	"${MAKE}" -fmakefile.mk -C21486 install ${NEW_FLAGS}
install_debug_21486:
	"${MAKE}" -fmakefile.mk -C21486 install_debug ${NEW_FLAGS}
#
# 21487
#
build_21487: 
	"${MAKE}" -fmakefile.mk -C21487 ${NEW_FLAGS}
clean_21487:
	"${MAKE}" -fmakefile.mk -C21487 clean ${NEW_FLAGS}
squeaky_21487:
	"${MAKE}" -fmakefile.mk -C21487 squeaky ${NEW_FLAGS}
dist_clean_21487:
	"${MAKE}" -fmakefile.mk -C21487 dist_clean ${NEW_FLAGS}
install_21487:
	"${MAKE}" -fmakefile.mk -C21487 install ${NEW_FLAGS}
install_debug_21487:
	"${MAKE}" -fmakefile.mk -C21487 install_debug ${NEW_FLAGS}
#
# 21488
#
build_21488: 
	"${MAKE}" -fmakefile.mk -C21488 ${NEW_FLAGS}
clean_21488:
	"${MAKE}" -fmakefile.mk -C21488 clean ${NEW_FLAGS}
squeaky_21488:
	"${MAKE}" -fmakefile.mk -C21488 squeaky ${NEW_FLAGS}
dist_clean_21488:
	"${MAKE}" -fmakefile.mk -C21488 dist_clean ${NEW_FLAGS}
install_21488:
	"${MAKE}" -fmakefile.mk -C21488 install ${NEW_FLAGS}
install_debug_21488:
	"${MAKE}" -fmakefile.mk -C21488 install_debug ${NEW_FLAGS}
#
# 21489
#
build_21489: 
	"${MAKE}" -fmakefile.mk -C21489 ${NEW_FLAGS}
clean_21489:
	"${MAKE}" -fmakefile.mk -C21489 clean ${NEW_FLAGS}
squeaky_21489:
	"${MAKE}" -fmakefile.mk -C21489 squeaky ${NEW_FLAGS}
dist_clean_21489:
	"${MAKE}" -fmakefile.mk -C21489 dist_clean ${NEW_FLAGS}
install_21489:
	"${MAKE}" -fmakefile.mk -C21489 install ${NEW_FLAGS}
install_debug_21489:
	"${MAKE}" -fmakefile.mk -C21489 install_debug ${NEW_FLAGS}


#
# Defaults
#
install_debug_base:
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/lib/${INST_DIR}
	${CP} ../../common/templates//*.tf ${INSTALL_DIR}/vdk
	${CP} ../templates/VDK_ISR_Cpp_Source.cpp.tf ${INSTALL_DIR}/vdk/VDK_ISR_CPP_Source.cpp.tf
	${CP} ../templates/*.tf ${INSTALL_DIR}/vdk
	${CP} ../2146x/templates/VDK_ISR_Cpp_Source.cpp.tf ${INSTALL_DIR}/vdk/VDK_ISR_CPP_Source.cpp.tf
	${CP} ../2146x/templates/*.tf ${INSTALL_DIR}/vdk

install_base: install_debug_base

dist_clean_base:
	${RM} ${INSTALL_DIR}/include/${INSTALL_HDRS}
	${RM} ${INSTALL_DIR}/vdk//*.tf
	${RMDIR} ${INSTALL_DIR}/vdk
	${RM} ${OBJS}
	${RM} *.ii
