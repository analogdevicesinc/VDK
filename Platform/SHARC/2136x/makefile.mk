#
#   $RCSfile$
#
#   Description: Makefile for compiling the 2136X
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
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC/2136x)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
ASM                     = EASM21K
ifeq (${VDSP_VERSION},4.0)
ASMFLAGS        = ${UP_ASMFLAGS} -double-size-any -Wwarn-error
else
ASMFLAGS        = ${UP_ASMFLAGS} -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30 -double-size-any  -Wwarn-error
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

INSTALL_DIR		= ${UP_INSTALL_DIR}/213x

OUTPUT		= ${OUT_DIR}2136X.dlb


#
# Projects
#
all:		default
#install: install_base 
#install_debug: install_debug_base 
ifeq (${DEVICE},)
default:	build_2136x build_21364 build_21369
clean:		clean_21364 clean_21369
squeaky:	squeaky_21364 squeaky_21363 squeaky_21365 squeaky_21366 squeaky_21367 squeaky_21368 squeaky_21369 squeaky_21362
dist_clean:	dist_clean_21364  dist_clean_21363 dist_clean_21365 dist_clean_21366  dist_clean_21367 dist_clean_21368 dist_clean_21369 dist_clean_21362
install:	install_2136x install_21364  install_21363 install_21365 install_21366  install_21367 install_21368 install_21369 install_21362
install_debug:	install_debug_2136x install_debug_21364  install_debug_21363 install_debug_21365 install_debug_21366  install_debug_21367 install_debug_21368 install_debug_21369 install_debug_21362

OFFSET_DEVICE	= 21364
else
ifeq (${DEVICE},21363)
default:	build_2136x build_21364
clean:		clean_21364
squeaky:	squeaky_21363
dist_clean:	dist_clean_21363
install:	install_2136x install_21363
install_debug:	install_debug_2136x install_debug_21363
OFFSET_DEVICE	= 21363
else
ifeq (${DEVICE},21364)
default:	build_2136x build_21364
clean:		clean_21364
squeaky:	squeaky_21364
dist_clean:	dist_clean_21364
install:	install_2136x install_21364
install_debug:	install_debug_2136x install_debug_21364
OFFSET_DEVICE	= 21364
else
ifeq (${DEVICE},21365)
default:	build_2136x build_21364 
clean:		clean_21364 
squeaky:	squeaky_21365 
dist_clean:	dist_clean_21365
install:	install_2136x install_21365
install_debug:	install_debug_2136x install_debug_21365
OFFSET_DEVICE	= 21365
else
ifeq (${DEVICE},21366)
default:        build_2136x build_21364
clean:          clean_21364
squeaky:        squeaky_21366
dist_clean:     dist_clean_21366
install:        install_2136x install_21366
install_debug:  install_debug_2136x install_debug_21366
OFFSET_DEVICE   = 21366
else
ifeq (${DEVICE},21367)
default:        build_2136x build_21369
clean:          clean_21369
squeaky:        squeaky_21367
dist_clean:     dist_clean_21367
install:        install_2136x install_21367
install_debug:  install_debug_2136x install_debug_21367
OFFSET_DEVICE   = 21367
else
ifeq (${DEVICE},21368)
default:        build_2136x build_21369
clean:          clean_21369
squeaky:        squeaky_21368
dist_clean:     dist_clean_21368
install:        install_2136x install_21368
install_debug:  install_debug_2136x install_debug_21368
OFFSET_DEVICE   = 21368
else
ifeq (${DEVICE},21369)
default:        build_2136x build_21369
clean:          clean_21369
squeaky:        squeaky_21369
dist_clean:     dist_clean_21369
install:        install_2136x install_21369
install_debug:  install_debug_2136x install_debug_21369
OFFSET_DEVICE   = 21369
else
ifeq (${DEVICE},21362)
default:        build_2136x build_21364
clean:          clean_21364
squeaky:        squeaky_21362
dist_clean:     dist_clean_21362
install:        install_2136x install_21362
install_debug:  install_debug_2136x install_debug_21362
OFFSET_DEVICE   = 21362
else
default:
clean:
squeaky:	squeaky_21364
dist_clean:	dist_clean_21364
install:	install_2136x install_21364
install_debug:	install_debug_2136x install_debug_21364
OFFSET_DEVICE	= 21364
endif
endif
endif
endif
endif
endif
endif
endif
endif

build_2136x_libs: ${OUTPUT}

ifeq (${REVISION},all)
build_2136x: build_2136x_0_0 build_2136x_0_1 build_2136x_any build_2136x_none build_21369_any
install_2136x: install_2136x_0_0 install_2136x_0_1 install_2136x_none install_2136x_any install_2136x_none install_21369_any
install_debug_2136x: install_debug_2136x_0_0 install_debug_2136x_0_1 install_debug_2136x_any install_debug_2136x_none install_debug_21369_any
DIRECTORIES=2136x_rev_0.0 2136x_rev_0.1 2136x_any 2136x_rev_none 21369_rev_any
else
ifeq (${REVISION},0.0)
build_2136x: build_2136x_0_0
install_2136x: install_2136x_0_0
install_debug_2136x: install_debug_2136x_0_0
DIRECTORIES=2136x_rev_0.0
else
ifeq (${REVISION},0.1)
build_2136x: build_2136x_0_1
install_2136x: install_2136x_0_1
install_debug_2136x: install_debug_2136x_0_1
DIRECTORIES=2136x_rev_0.1
else
ifeq (${REVISION},none)
build_2136x: build_2136x_none
install_2136x: install_2136x_none
install_debug_2136x: install_debug_2136x_none
DIRECTORIES=2136x_rev_none
else
ifeq (${REVISION},any)
build_2136x: build_2136x_any build_21369_any
install_2136x: install_2136x_any install_21369_any
DIRECTORIES=2136x_any 21369_rev_any
install_debug_2136x: install_debug_2136x_any install_debug_21369_any
else
build_2136x: build_2136x_0_0 build_2136x_0_1 build_2136x_any build_2136x_none build_21369_any
install_2136x: install_2136x_0_0 install_2136x_0_1 install_2136x_any install_2136x_none install_21369_any
install_debug_2136x: install_debug_2136x_0_0 install_debug_2136x_0_1 install_debug_2136x_any install_debug_2136x_none install_debug_21369_any
DIRECTORIES=2136x_rev_0.0 2136x_rev_0.1 2136x_any 2136x_rev_none 21369_rev_any
endif
endif
endif
endif
endif

##############################
# Silicon revisions 0.0 
build_2136x_0_0:
	${MKDIR} 2136x_rev_0.0
	"${MAKE}" -f../makefile.mk  -C 2136x_rev_0.0 build_2136x_libs ${NEW_FLAGS}  PLATFORM=ADSP-21364 VEROPTIONS+="-si-revision 0.0" UP_ASMFLAGS+="-proc ADSP-21364 -si-revision 0.0 " UP_CXXFLAGS+=" -proc ADSP-21364 -si-revision 0.0 " REVISION=0.0 NESTED_MAKE=YES

install_2136x_0_0: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2136x_rev_0.0" INST_DIR="2136x_rev_0.0"

install_debug_2136x_0_0: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2136x_rev_0.0" INST_DIR="2136x_rev_0.0"

##############################
# Silicon revisions 0.1 
build_2136x_0_1:
	${MKDIR} 2136x_rev_0.1
	"${MAKE}" -f../makefile.mk  -C 2136x_rev_0.1 build_2136x_libs ${NEW_FLAGS}  PLATFORM=ADSP-21364 VEROPTIONS+="-si-revision 0.1" UP_ASMFLAGS+="-proc ADSP-21369 -si-revision 0.1 " UP_CXXFLAGS+=" -proc ADSP-21369 -si-revision 0.1 " REVISION=0.1 NESTED_MAKE=YES

install_2136x_0_1: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2136x_rev_0.1" INST_DIR="2136x_LX3_rev_0.1"

install_debug_2136x_0_1: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2136x_rev_0.1" INST_DIR="2136x_LX3_rev_0.1"

##############################
# Any libraries
build_2136x_any:
	${MKDIR} 2136x_any
	"${MAKE}" -f../makefile.mk  -C 2136x_any build_2136x_libs ${NEW_FLAGS}  PLATFORM=ADSP-21364 VEROPTIONS+="-si-revision any" UP_ASMFLAGS+="-proc ADSP-21364 -si-revision any " UP_CXXFLAGS+="-proc ADSP-21364 -si-revision any " REVISION=any NESTED_MAKE=YES

install_2136x_any: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2136x_any" INST_DIR="2136x_any"

install_debug_2136x_any: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2136x_any" INST_DIR="2136x_any"

##############################
# Any libraries
build_21369_any:
	${MKDIR} 21369_rev_any
	"${MAKE}" -f../makefile.mk  -C 21369_rev_any build_2136x_libs ${NEW_FLAGS}  PLATFORM=ADSP-21369 VEROPTIONS+="-si-revision any" UP_ASMFLAGS+="-proc ADSP-21369 -si-revision any " UP_CXXFLAGS+="-proc ADSP-21369 -si-revision any " REVISION=any NESTED_MAKE=YES

install_21369_any: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="21369_rev_any" INST_DIR="21369_rev_any"

install_debug_21369_any: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="21369_rev_any" INST_DIR="21369_rev_any"

# None libraries
build_2136x_none:
	${MKDIR} 2136x_rev_none
	"${MAKE}" -f../makefile.mk  -C 2136x_rev_none build_2136x_libs ${NEW_FLAGS} PLATFORM=ADSP-21364 VEROPTIONS+="-si-revision none"  UP_ASMFLAGS+="-proc ADSP-21364 -si-revision none " UP_CXXFLAGS+="-proc ADSP-21364 -si-revision none " REVISION=none NESTED_MAKE=YES

install_2136x_none: 
	"${MAKE}" -fmakefile.mk  install_base ${NEW_FLAGS} SI_REV="2136x_rev_none" INST_DIR=""

install_debug_2136x_none: 
	"${MAKE}" -fmakefile.mk  install_debug_base ${NEW_FLAGS} SI_REV="2136x_rev_none" INST_DIR="2136x_rev_none"


######
# 2136X dlb build
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
	${RM} ${INSTALL_DIR}/lib/VDK-*-2136X*.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

#
# Device Specific
#

#
# 21364
#
build_21364: 
	"${MAKE}" -fmakefile.mk -C21364 ${NEW_FLAGS}
clean_21364:
	"${MAKE}" -fmakefile.mk -C21364 clean ${NEW_FLAGS}
squeaky_21364:
	"${MAKE}" -fmakefile.mk -C21364 squeaky ${NEW_FLAGS}
dist_clean_21364:
	"${MAKE}" -fmakefile.mk -C21364 dist_clean ${NEW_FLAGS}
install_21364:
	"${MAKE}" -fmakefile.mk -C21364 install ${NEW_FLAGS}
install_debug_21364:
	"${MAKE}" -fmakefile.mk -C21364 install_debug ${NEW_FLAGS}

#
# 21363
#
squeaky_21363:
	"${MAKE}" -fmakefile.mk -C21363 squeaky ${NEW_FLAGS}
dist_clean_21363:
	"${MAKE}" -fmakefile.mk -C21363 dist_clean ${NEW_FLAGS}
install_21363:
	"${MAKE}" -fmakefile.mk -C21363 install ${NEW_FLAGS}
install_debug_21363:
	"${MAKE}" -fmakefile.mk -C21363 install_debug ${NEW_FLAGS}

#
# 21365
#
squeaky_21365:
	"${MAKE}" -fmakefile.mk -C21365 squeaky ${NEW_FLAGS}
dist_clean_21365:
	"${MAKE}" -fmakefile.mk -C21365 dist_clean ${NEW_FLAGS}
install_21365:
	"${MAKE}" -fmakefile.mk -C21365 install ${NEW_FLAGS}
install_debug_21365:
	"${MAKE}" -fmakefile.mk -C21365 install_debug ${NEW_FLAGS}

#
# 21366
#
squeaky_21366:
	"${MAKE}" -fmakefile.mk -C21366 squeaky ${NEW_FLAGS}
dist_clean_21366:
	"${MAKE}" -fmakefile.mk -C21366 dist_clean ${NEW_FLAGS}
install_21366:
	"${MAKE}" -fmakefile.mk -C21366 install ${NEW_FLAGS}
install_debug_21366:
	"${MAKE}" -fmakefile.mk -C21366 install_debug ${NEW_FLAGS}

#
# 21367
#
squeaky_21367:
	"${MAKE}" -fmakefile.mk -C21367 squeaky ${NEW_FLAGS}
dist_clean_21367:
	"${MAKE}" -fmakefile.mk -C21367 dist_clean ${NEW_FLAGS}
install_21367:
	"${MAKE}" -fmakefile.mk -C21367 install ${NEW_FLAGS}
install_debug_21367:
	"${MAKE}" -fmakefile.mk -C21367 install_debug ${NEW_FLAGS}

#
# 21368
#
squeaky_21368:
	"${MAKE}" -fmakefile.mk -C21368 squeaky ${NEW_FLAGS}
dist_clean_21368:
	"${MAKE}" -fmakefile.mk -C21368 dist_clean ${NEW_FLAGS}
install_21368:
	"${MAKE}" -fmakefile.mk -C21368 install ${NEW_FLAGS}
install_debug_21368:
	"${MAKE}" -fmakefile.mk -C21368 install_debug ${NEW_FLAGS}

#
# 21369
#
build_21369: 
	"${MAKE}" -fmakefile.mk -C21369 ${NEW_FLAGS}
clean_21369:
	"${MAKE}" -fmakefile.mk -C21369 clean ${NEW_FLAGS}
squeaky_21369:
	"${MAKE}" -fmakefile.mk -C21369 squeaky ${NEW_FLAGS}
dist_clean_21369:
	"${MAKE}" -fmakefile.mk -C21369 dist_clean ${NEW_FLAGS}
install_21369:
	"${MAKE}" -fmakefile.mk -C21369 install ${NEW_FLAGS}
install_debug_21369:
	"${MAKE}" -fmakefile.mk -C21369 install_debug ${NEW_FLAGS}

#
# 21362
#
squeaky_21362:
	"${MAKE}" -fmakefile.mk -C21362 squeaky ${NEW_FLAGS}
dist_clean_21362:
	"${MAKE}" -fmakefile.mk -C21362 dist_clean ${NEW_FLAGS}
install_21362:
	"${MAKE}" -fmakefile.mk -C21362 install ${NEW_FLAGS}
install_debug_21362:
	"${MAKE}" -fmakefile.mk -C21362 install_debug ${NEW_FLAGS}

#
# Setup some flags for assembly access
#
OFFSET_CXXFLAGS = ${subst " -c ",,${CXXFLAGS}}
offsets:                asm_offsets.h
asm_offsets.h:  GENERATE_OFFSETS.DXE
	../../common/generate_offsets.bat ADSP-${OFFSET_DEVICE} $< ${VDSP_VERSION} | sed "/Could not locate DM range/d" > asm_offsets.h
GENERATE_OFFSETS.DXE:   generate_offsets.cpp
	${CXX} -proc ADSP-21364 ${OFFSET_CXXFLAGS} ${DEFINES} ${INCLUDE_PATH} -g `cygpath -w $<` -Wremark 265
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
	${CP} ${SI_REV}/debug/VDK-*-2136X*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

install_base: install_debug_base
	${CP} ${SI_REV}/release/VDK-*-2136X*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

dist_clean_base:
	${RM} ${INSTALL_DIR}/include/${INSTALL_HDRS}
	${RM} ${INSTALL_DIR}/vdk//*.tf
	${RMDIR} ${INSTALL_DIR}/vdk
	${RM} ${OBJS}
	${RM} *.ii
