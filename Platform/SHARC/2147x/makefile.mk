#
#   $RCSfile$
#
#   Description: Makefile for compiling the 2147X
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
VPATH		= ${CURDIR}:${CURDIR}/../2146x:${UP_VPATH}
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2147x)' -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2146x)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' -I '$(shell cygpath -w ${CURDIR}/../2146x)' ${UP_INCLUDE_PATH}
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
default:	build_2147x build_21479
clean:		clean_2147x clean_21471 clean_21472 clean_21475 clean_21477 clean_21478 clean_21479
squeaky:	squeaky_2147x squeaky_21471 squeaky_21472 squeaky_21475  squeaky_21477 squeaky_21478 squeaky_21479
dist_clean:	dist_clean_21471 dist_clean_21472 dist_clean_21475 dist_clean_21477 dist_clean_21478 dist_clean_21479
install:	install_2147x install_21471 install_21472 install_21475 install_21477 install_21478 install_21479
install_debug:	install_debug_2147x install_debug_21471 install_debug_21472 install_debug_21475 install_debug_21477 install_debug_21478 install_debug_21479

OFFSET_DEVICE	= 21479
else
ifeq (${DEVICE},21471)
default:        build_2147x build_21471
clean:          clean_21471
squeaky:        squeaky_21471
dist_clean:     dist_clean_21471
install:        install_2147x install_21471
install_debug:  install_debug_2147x install_debug_21471
OFFSET_DEVICE   = 21471
else
ifeq (${DEVICE},21472)
default:        build_2147x build_21479
clean:          clean_21479
squeaky:        squeaky_21479
dist_clean:     dist_clean_21472
install:        install_2147x install_21472
install_debug:  install_debug_2147x install_debug_21472
OFFSET_DEVICE   = 21472
else
ifeq (${DEVICE},21475)
default:        build_2147x build_21479
clean:          clean_21479
squeaky:        squeaky_21479
dist_clean:     dist_clean_21475
install:        install_2147x install_21475
install_debug:  install_debug_2147x install_debug_21475
OFFSET_DEVICE   = 21475
else
ifeq (${DEVICE},21477)
default:        build_2147x build_21477
clean:          clean_21477
squeaky:        squeaky_21477
dist_clean:     dist_clean_21477
install:        install_2147x install_21477
install_debug:  install_debug_2147x install_debug_21477
OFFSET_DEVICE   = 21477
else
ifeq (${DEVICE},21478)
default:        build_2147x build_21478
clean:          clean_21478
squeaky:        squeaky_21478
dist_clean:     dist_clean_21478
install:        install_2147x install_21478
install_debug:  install_debug_2147x install_debug_21478
OFFSET_DEVICE   = 21478
else
ifeq (${DEVICE},21479)
default:        build_2147x build_21479
clean:          clean_21479
squeaky:        squeaky_21479
dist_clean:     dist_clean_21479
install:        install_2147x install_21479
install_debug:  install_debug_2147x install_debug_21479
OFFSET_DEVICE   = 21479
else
default: build_2147x build_21479
clean:   clean_21479
squeaky:	squeaky_21479
dist_clean:	dist_clean_21479
install:	install_2147x install_21479
install_debug:	install_debug_2147x install_debug_21479
OFFSET_DEVICE	= 21479
endif
endif
endif
endif
endif
endif
endif

build_2147x_libs: ${OUTPUT}

#####
# Clean
#
clean_2147x:
	${foreach dir, ${DIRECTORIES}, \
	${MKDIR} ${dir}; \
	cd ${dir}; \
	${RM} ${OBJS}; \
	${RM} *.ii; \
	${RM} *.ti; \
	${RM} ver.txt; \
	cd ..;}
squeaky_2147x:
	${RM} GENERATE_OFFSETS.DXE
	${RM} ${OBJS}
	${RM} *.ii
	${RM} *.ti
	${RM} debug/*
	${RM} release/*
	${RM} ver.txt

dist_clean_2147x:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/include/${notdir ${file}}; }
	${RM} ${OBJS}
	${RM} *.ii
	${RM} *.ti
	${RM} ${INSTALL_DIR}/lib/VDK-*-2146X*.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

ifeq (${REVISION},all)
build_2147x: build_2147x_any 
install_2147x: install_2147x_any 
install_debug_2147x: install_debug_2147x_any 
DIRECTORIES=2147x_rev_any 
else
ifeq (${REVISION},any)
build_2147x: build_2147x_any
install_2147x: install_2147x_any
DIRECTORIES=2147x_rev_any
install_debug_2147x: install_debug_2147x_any
else
build_2147x: build_2147x_any
install_2147x: install_2147x_any
install_debug_2147x: install_debug_2147x_any
DIRECTORIES=2147x_rev_any
endif
endif

# any libraries
build_2147x_any:
	${MKDIR} 2147x_rev_any
	"${MAKE}" -f../makefile.mk  -C 2147x_rev_any build_2147x_libs ${NEW_FLAGS}  PLATFORM=ADSP-21479 VEROPTIONS+="-si-revision any" UP_ASMFLAGS+="-proc ADSP-21479 -si-revision any " UP_CXXFLAGS+="-proc ADSP-21479 -si-revision any " REVISION=any NESTED_MAKE=YES

install_2147x_any: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2147x_rev_any" INST_DIR="21479_rev_any"

install_debug_2147x_any: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2147x_rev_any" INST_DIR="21479_rev_any"

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

#
# Device Specific
#

#
# 21479
#
build_21479: 
	"${MAKE}" -fmakefile.mk -C21479 ${NEW_FLAGS}
clean_21479:
	"${MAKE}" -fmakefile.mk -C21479 clean ${NEW_FLAGS}
squeaky_21479:
	"${MAKE}" -fmakefile.mk -C21479 squeaky ${NEW_FLAGS}
dist_clean_21479:
	"${MAKE}" -fmakefile.mk -C21479 dist_clean ${NEW_FLAGS}
install_21479:
	"${MAKE}" -fmakefile.mk -C21479 install ${NEW_FLAGS}
install_debug_21479:
	"${MAKE}" -fmakefile.mk -C21479 install_debug ${NEW_FLAGS}

#
# 21471
#
build_21471: build_21479
clean_21471: clean_21479
	"${MAKE}" -fmakefile.mk -C21471 clean ${NEW_FLAGS}
squeaky_21471: squeaky_21479
	"${MAKE}" -fmakefile.mk -C21471 clean ${NEW_FLAGS}
dist_clean_21471:
	"${MAKE}" -fmakefile.mk -C21471 dist_clean ${NEW_FLAGS}
install_21471:
	"${MAKE}" -fmakefile.mk -C21471 install ${NEW_FLAGS}
install_debug_21471:
	"${MAKE}" -fmakefile.mk -C21471 install_debug ${NEW_FLAGS}

#
# 21472
#
build_21472: build_21479
clean_21472: clean_21479
	"${MAKE}" -fmakefile.mk -C21472 clean ${NEW_FLAGS}
squeaky_21472: squeaky_21479
	"${MAKE}" -fmakefile.mk -C21472 clean ${NEW_FLAGS}
dist_clean_21472:
	"${MAKE}" -fmakefile.mk -C21472 dist_clean ${NEW_FLAGS}
install_21472:
	"${MAKE}" -fmakefile.mk -C21472 install ${NEW_FLAGS}
install_debug_21472:
	"${MAKE}" -fmakefile.mk -C21472 install_debug ${NEW_FLAGS}

#
# 21475
#
build_21475: build_21479
clean_21475: clean_21479
	"${MAKE}" -fmakefile.mk -C21475 clean ${NEW_FLAGS}
squeaky_21475: squeaky_21479
	"${MAKE}" -fmakefile.mk -C21475 clean ${NEW_FLAGS}
dist_clean_21475:
	"${MAKE}" -fmakefile.mk -C21475 dist_clean ${NEW_FLAGS}
install_21475:
	"${MAKE}" -fmakefile.mk -C21475 install ${NEW_FLAGS}
install_debug_21475:
	"${MAKE}" -fmakefile.mk -C21475 install_debug ${NEW_FLAGS}

#
# 21477
#
build_21477: build_21479
clean_21477: clean_21479
	"${MAKE}" -fmakefile.mk -C21477 clean ${NEW_FLAGS}
squeaky_21477: squeaky_21479
	"${MAKE}" -fmakefile.mk -C21477 clean ${NEW_FLAGS}
dist_clean_21477:
	"${MAKE}" -fmakefile.mk -C21477 dist_clean ${NEW_FLAGS}
install_21477:
	"${MAKE}" -fmakefile.mk -C21477 install ${NEW_FLAGS}
install_debug_21477:
	"${MAKE}" -fmakefile.mk -C21477 install_debug ${NEW_FLAGS}

#
# 21478
#
build_21478: build_21479
clean_21478: clean_21479
	"${MAKE}" -fmakefile.mk -C21478 clean ${NEW_FLAGS}
squeaky_21478: squeaky_21479
	"${MAKE}" -fmakefile.mk -C21478 clean ${NEW_FLAGS}
dist_clean_21478:
	"${MAKE}" -fmakefile.mk -C21478 dist_clean ${NEW_FLAGS}
install_21478:
	"${MAKE}" -fmakefile.mk -C21478 install ${NEW_FLAGS}
install_debug_21478:
	"${MAKE}" -fmakefile.mk -C21478 install_debug ${NEW_FLAGS}


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
	${CP} ${SI_REV}/debug/VDK-*-2146X*.dlb ${INSTALL_DIR}/lib/${INST_DIR}


install_base: install_debug_base
	${CP} ${SI_REV}/release/VDK-*-2146X*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

dist_clean_base:
	${RM} ${INSTALL_DIR}/include/${INSTALL_HDRS}
	${RM} ${INSTALL_DIR}/vdk//*.tf
	${RMDIR} ${INSTALL_DIR}/vdk
	${RM} ${OBJS}
	${RM} *.ii
