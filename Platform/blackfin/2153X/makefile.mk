#
#   $RCSfile$
#
#   Description: Makefile for compiling the 2153X
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
	${ASM} ${ASMFLAGS} $(subst -I,-i ,${INCLUDE_PATH}) ${DEFINES} $(shell cygpath -w $<) -o $@

.cpp.doj:
	${CXX} -c ${CXXFLAGS} '$(shell cygpath -w $<)' ${DEFINES} -o $@


###########################################################
# Default commands
###########################################################
RM=rm -f
CP=cp -p
RMDIR=rm -rf
MKDIR=mkdir -p


###########################################################
# Setup the variables that are passed to sub calls of make
# These lines should change as they progress up the calls,
# but should basically remain the same from level to level
###########################################################
VPATH		= ${CURDIR}:${UP_VPATH}
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH		= -I '$(shell cygpath -w ${TOP_DIR}/blackfin/2153X)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH		= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif

CXX			= CCBLKFN
DEFINES		= ${UP_DEFINES}
CXXFLAGS	= ${UP_CXXFLAGS} -double-size-any
INSTALL_HDRS= ${UP_INSTALL_HDRS}
ASM			= EASMBLKFN
ifeq (${VDSP_VERSION},4.0)
ASMFLAGS	= -w -stallcheck=none -double-size-any
else
ASMFLAGS	= -w -stallcheck=none -double-size-any -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30
endif
ASM_SRC		= SlowIdle.asm VDK_ISR_API.asm VDK_ISR_EVENTS_API.asm VDK_ISR_SEMAPHORES_API.asm VDK_ISR_DEVICES_API.asm ADI_DCB_Support.asm DebugLogging.asm UserHistoryLog.asm RoundRobinBypass.asm
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
			  UP_ASM_SRC="${ASM_SRC}"			\
			  UP_OBJS="${OBJS}"			\
				VDSP_VERSION="${VDSP_VERSION}"
INSTALL_FLAGS	= INSTALL_DIR="${INSTALL_DIR}"
# TODO:
#	Change the install dir from BLACKFIN
INSTALL_DIR		= ${UP_INSTALL_DIR}/BLACKFIN

OUTPUT		= ${OUT_DIR}BF53X.dlb

#Version.o:	BuildNumber.h


#
# Projects
#
all:		default
ifeq (${DEVICE},)
default:		build_21532 build_21535 build_BF531 build_BF533 build_BF561 build_BF539 build_BF534 build_BF536 build_BF537 build_BF538  build_BF542 build_BF544 build_BF547 build_BF548 build_BF549 build_BF542M build_BF544M build_BF547M build_BF548M build_BF549M build_BF522 build_BF523 build_BF524 build_BF525 build_BF526 build_BF527 build_BF512 build_BF514 build_BF516 build_BF518 build_BF506F build_BF592-A
clean:		clean_21532 clean_21535 clean_BF531 clean_BF533 clean_BF561 clean_BF539 clean_BF534 clean_BF536 clean_BF537 clean_BF538 clean_BF542 clean_BF544 clean_BF547 clean_BF548 clean_BF549 clean_BF542M clean_BF544M clean_BF547M clean_BF548M clean_BF549M clean_BF522 clean_BF523 clean_BF524 clean_BF525 clean_BF526 clean_BF527 clean_BF512 clean_BF514 clean_BF516 clean_BF518 clean_BF504 clean_BF504F clean_BF506F clean_BF592-A
squeaky:		squeaky_21532 squeaky_21535 squeaky_BF531 squeaky_BF533 squeaky_BF561 squeaky_BF539 squeaky_BF534 squeaky_BF536 squeaky_BF537 squeaky_BF538 squeaky_BF542 squeaky_BF544 squeaky_BF547 squeaky_BF548 squeaky_BF549 squeaky_BF542M squeaky_BF544M squeaky_BF547M squeaky_BF548M squeaky_BF549M squeaky_BF522 squeaky_BF523 squeaky_BF524 squeaky_BF525 squeaky_BF526 squeaky_BF527 squeaky_BF512 squeaky_BF514 squeaky_BF516 squeaky_BF518 squeaky_BF504 squeaky_BF504F squeaky_BF506F squeaky_BF592-A
dist_clean:		dist_clean_21532 dist_clean_21535 dist_clean_BF531 dist_clean_BF533 dist_clean_BF561 dist_clean_BF539 dist_clean_BF534 dist_clean_BF536 dist_clean_BF537 dist_clean_BF538  dist_clean_BF542 dist_clean_BF544 dist_clean_BF547 dist_clean_BF548 dist_clean_BF549 dist_clean_BF542M dist_clean_BF544M dist_clean_BF547M dist_clean_BF548M dist_clean_BF549M dist_clean_BF522 dist_clean_BF523 dist_clean_BF524 dist_clean_BF525 dist_clean_BF526 dist_clean_BF527 dist_clean_BF512 dist_clean_BF514 dist_clean_BF516 dist_clean_BF518 dist_clean_BF504 dist_clean_BF504F dist_clean_BF506F dist_clean_BF592-A
install:		install_21532 install_21535 install_BF531 install_BF533 install_BF561 install_BF539 install_BF534 install_BF536 install_BF537 install_BF538 install_BF522 install_BF523 install_BF524 install_BF525 install_BF526 install_BF527 install_BF542 install_BF544 install_BF547 install_BF548 install_BF549 install_BF542M install_BF544M install_BF547M install_BF548M install_BF549M install_BF512 install_BF514 install_BF516 install_BF518 install_BF504 install_BF504F install_BF506F install_BF592-A
install_debug:	install_debug_21532 install_debug_21535 install_debug_BF531 install_debug_BF533 install_debug_BF561 install_debug_BF539 install_debug_BF534 install_debug_BF536 install_debug_BF537 install_debug_BF538 install_debug_BF522 install_debug_BF523 install_debug_BF524 install_debug_BF525 install_debug_BF526 install_debug_BF527 install_debug_BF542 install_debug_BF544 install_debug_BF547 install_debug_BF548 install_debug_BF549 install_debug_BF542M install_debug_BF544M install_debug_BF547M install_debug_BF548M install_debug_BF549M install_debug_BF512 install_debug_BF514 install_debug_BF516 install_debug_BF518 install_debug_BF504 install_debug_BF504F install_debug_BF506F install_debug_BF592-A
else
ifeq (${DEVICE},BF504)
default:                build_BF504
clean:                  clean_BF504
squeaky:                squeaky_BF504
dist_clean:             dist_clean_BF504
install:                install_BF504
install_debug:		install_debug_BF504
else
ifeq (${DEVICE},BF504F)
default:                build_BF504F
clean:                  clean_BF504F
squeaky:                squeaky_BF504F
dist_clean:             dist_clean_BF504F
install:                install_BF504F
install_debug:		install_debug_BF504F
else
ifeq (${DEVICE},BF506F)
default:                build_BF506F
clean:                  clean_BF506F
squeaky:                squeaky_BF506F
dist_clean:             dist_clean_BF506F
install:                install_BF506F
install_debug:		install_debug_BF506F
else
ifeq (${DEVICE},BF512)
default:                build_BF512
clean:                  clean_BF512
squeaky:                squeaky_BF512
dist_clean:             dist_clean_BF512
install:                install_BF512
install_debug:		install_debug_BF512
else
ifeq (${DEVICE},BF514)
default:                build_BF514
clean:                  clean_BF514
squeaky:                squeaky_BF514
dist_clean:             dist_clean_BF514
install:                install_BF514
install_debug:		install_debug_BF514
else
ifeq (${DEVICE},BF516)
default:                build_BF516
clean:                  clean_BF516
squeaky:                squeaky_BF516
dist_clean:             dist_clean_BF516
install:                install_BF516
install_debug:		install_debug_BF516
else
ifeq (${DEVICE},BF518)
default:                build_BF518
clean:                  clean_BF518
squeaky:                squeaky_BF518
dist_clean:             dist_clean_BF518
install:                install_BF518
install_debug:		install_debug_BF518
else
ifeq (${DEVICE},BF522)
default:                build_BF522
clean:                  clean_BF522
squeaky:                squeaky_BF522
dist_clean:             dist_clean_BF522
install:                install_BF522
install_debug:		install_debug_BF522
else
ifeq (${DEVICE},BF523)
default:                build_BF523
clean:                  clean_BF523
squeaky:                squeaky_BF523
dist_clean:             dist_clean_BF523
install:                install_BF523
install_debug:		install_debug_BF523
else
ifeq (${DEVICE},BF524)
default:                build_BF524
clean:                  clean_BF524
squeaky:                squeaky_BF524
dist_clean:             dist_clean_BF524
install:                install_BF524
install_debug:		install_debug_BF524
else
ifeq (${DEVICE},BF525)
default:                build_BF525
clean:                  clean_BF525
squeaky:                squeaky_BF525
dist_clean:             dist_clean_BF525
install:                install_BF525
install_debug:		install_debug_BF525
else
ifeq (${DEVICE},BF526)
default:                build_BF526
clean:                  clean_BF526
squeaky:                squeaky_BF526
dist_clean:             dist_clean_BF526
install:                install_BF526
install_debug:		install_debug_BF526
else
ifeq (${DEVICE},BF527)
default:                build_BF527
clean:                  clean_BF527
squeaky:                squeaky_BF527
dist_clean:             dist_clean_BF527
install:                install_BF527
install_debug:		install_debug_BF527
else
ifeq (${DEVICE},BF532)
default:		build_21532
clean:		clean_21532
squeaky:		squeaky_21532
dist_clean:		dist_clean_21532
install:		install_21532
install_debug:		install_debug_21532
else
ifeq (${DEVICE},BF535)
default:		build_21535
clean:		clean_21535
squeaky:		squeaky_21535
dist_clean:		dist_clean_21535
install:		install_21535
install_debug:		install_debug_21535
else
ifeq (${DEVICE},BF533)
default:                build_BF533
clean:                  clean_BF533
squeaky:                squeaky_BF533
dist_clean:             dist_clean_BF533
install:                install_BF533
install_debug:		install_debug_BF533
else
ifeq (${DEVICE},BF531)
default:                build_BF531
clean:                  clean_BF531
squeaky:                squeaky_BF531
dist_clean:             dist_clean_BF531
install:                install_BF531
install_debug:		install_debug_BF531
else
ifeq (${DEVICE},BF561)
default:              build_BF561
clean:                        clean_BF561
squeaky:              squeaky_BF561
dist_clean:           dist_clean_BF561
install:              install_BF561
install_debug:		install_debug_BF561
else
ifeq (${DEVICE},BF539)
default:                build_BF539
clean:                  clean_BF539
squeaky:                squeaky_BF539
dist_clean:             dist_clean_BF539
install:                install_BF539
install_debug:          install_debug_BF539
else
ifeq (${DEVICE},BF534)
default:                build_BF534
clean:                  clean_BF534
squeaky:                squeaky_BF534
dist_clean:             dist_clean_BF534
install:                install_BF534
install_debug:          install_debug_BF534
else
ifeq (${DEVICE},BF536)
default:                build_BF536
clean:                  clean_BF536
squeaky:                squeaky_BF536
dist_clean:             dist_clean_BF536
install:                install_BF536
install_debug:          install_debug_BF536
else
ifeq (${DEVICE},BF537)
default:                build_BF537
clean:                  clean_BF537
squeaky:                squeaky_BF537
dist_clean:             dist_clean_BF537
install:                install_BF537
install_debug:          install_debug_BF537
else
ifeq (${DEVICE},BF538)
default:                build_BF538
clean:                  clean_BF538
squeaky:                squeaky_BF538
dist_clean:             dist_clean_BF538
install:                install_BF538
install_debug:          install_debug_BF538
else
ifeq (${DEVICE},BF542)
default:                build_BF542
clean:                  clean_BF542
squeaky:                squeaky_BF542
dist_clean:             dist_clean_BF542
install:                install_BF542
install_debug:          install_debug_BF542
else
ifeq (${DEVICE},BF544)
default:                build_BF544
clean:                  clean_BF544
squeaky:                squeaky_BF544
dist_clean:             dist_clean_BF544
install:                install_BF544
install_debug:          install_debug_BF544
else
ifeq (${DEVICE},BF547)
default:                build_BF547
clean:                  clean_BF547
squeaky:                squeaky_BF547
dist_clean:             dist_clean_BF547
install:                install_BF547
install_debug:          install_debug_BF547
else
ifeq (${DEVICE},BF548)
default:                build_BF548
clean:                  clean_BF548
squeaky:                squeaky_BF548
dist_clean:             dist_clean_BF548
install:                install_BF548
install_debug:          install_debug_BF548
else
ifeq (${DEVICE},BF549)
default:                build_BF549
clean:                  clean_BF549
squeaky:                squeaky_BF549
dist_clean:             dist_clean_BF549
install:                install_BF549
install_debug:          install_debug_BF549
else
ifeq (${DEVICE},BF542M)
default:                build_BF542M
clean:                  clean_BF542M
squeaky:                squeaky_BF542M
dist_clean:             dist_clean_BF542M
install:                install_BF542M
install_debug:          install_debug_BF542M
else
ifeq (${DEVICE},BF544M)
default:                build_BF544M
clean:                  clean_BF544M
squeaky:                squeaky_BF544M
dist_clean:             dist_clean_BF544M
install:                install_BF544M
install_debug:          install_debug_BF544M
else
ifeq (${DEVICE},BF547M)
default:                build_BF547M
clean:                  clean_BF547M
squeaky:                squeaky_BF547M
dist_clean:             dist_clean_BF547M
install:                install_BF547M
install_debug:          install_debug_BF547M
else
ifeq (${DEVICE},BF548M)
default:                build_BF548M
clean:                  clean_BF548M
squeaky:                squeaky_BF548M
dist_clean:             dist_clean_BF548M
install:                install_BF548M
install_debug:          install_debug_BF548M
else
ifeq (${DEVICE},BF549M)
default:                build_BF549M
clean:                  clean_BF549M
squeaky:                squeaky_BF549M
dist_clean:             dist_clean_BF549M
install:                install_BF549M
install_debug:          install_debug_BF549M
else
ifeq (${DEVICE},BF592-A)
default:                build_BF592-A
clean:                  clean_BF592-A
squeaky:                squeaky_BF592-A
dist_clean:             dist_clean_BF592-A
install:                install_BF592-A
install_debug:          install_debug_BF592-A
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif



######
# BF53X dlb build
#
${OUTPUT}:			${OBJS}
	${MKDIR} release debug
	elfar -c "${OUTPUT}" ${OBJS}
	@echo "****************************************"
	@echo "${OUTPUT} build completed at:"
	@date +%c
	@echo "****************************************"



#####
# Install
#
install_base:
	${MKDIR} ${INSTALL_DIR}/include
	${MKDIR} ${INSTALL_DIR}/vdk
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} $(shell cygpath -w ${file}) -o $(shell cygpath -w ${INSTALL_DIR}/include/${notdir ${file}}) ${INCLUDE_PATH};}
	${CP} ../../common/templates//*.tf ${INSTALL_DIR}/vdk
	${CP} templates//VDK_NMI_CPP_Source.cpp.tf ${INSTALL_DIR}//vdk
	${CP} templates//*.tf ${INSTALL_DIR}//vdk
	# This template is not used but it is copied from common
	${RM} ${INSTALL_DIR}//vdk//VDK_ISR_Asm_Source.asm.tf


#####
# Clean
#
clean:
	${RM} ${OBJS}
squeaky:
	${RM} *.s
	${RM} *.ii
dist_clean:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/include/${notdir ${file}}; }
	${RM} ${INSTALL_DIR}/vdk//*.tf
	${RMDIR} ${INSTALL_DIR}/vdk
	${RM} GENERATE_OFFSETS.DXE
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release

#
# Device Specific
#

#####
# 21532
#
# The only change between 21532 and BF531 and BF533 at the moment is the ldf file
# Silicon 0.3 needs to build BF66 libs

ifeq (${REVISION},all)
build_21532: build_21532_dir
clean_21532: clean_21532_dir
squeaky_21532: squeaky_21532_dir
install_21532: install_21532_dir
install_debug_21532: install_debug_21532_dir
else
ifeq (${REVISION},0.3)
build_21532: build_21532_dir
clean_21532: clean_21532_dir
squeaky_21532: squeaky_21532_dir
install_21532: install_21532_dir
install_debug_21532: install_debug_21532_dir
else
ifeq (${REVISION},)
build_21532: build_21532_dir
clean_21532: clean_21532_dir
squeaky_21532: squeaky_21532_dir
install_21532: install_21532_dir
install_debug_21532: install_debug_21532_dir
else
build_21532: build_21532_dir
clean_21532: clean_21532_dir
squeaky_21532: squeaky_21532_dir
install_21532: install_21532_dir
install_debug_21532: install_debug_21532_dir
endif
endif
endif

#####
# BF504
#
build_BF504: build_BF506F
clean_BF504: clean_BF506F
	"${MAKE}" -fmakefile.mk -CBF504 clean ${NEW_FLAGS}
squeaky_BF504: squeaky_BF506F
	"${MAKE}" -fmakefile.mk -CBF504 squeaky ${NEW_FLAGS}
dist_clean_BF504:
	"${MAKE}" -fmakefile.mk -CBF504 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF504:install_BF506F
	"${MAKE}" -fmakefile.mk -CBF504 install ${INSTALL_FLAGS} -i
install_debug_BF504:  install_debug_BF506F
	"${MAKE}" -fmakefile.mk -CBF504 install_debug ${INSTALL_FLAGS} -i
#####
# BF504F
#
build_BF504F: build_BF506F
clean_BF504F: clean_BF506F
	"${MAKE}" -fmakefile.mk -CBF504F clean ${NEW_FLAGS}
squeaky_BF504F: squeaky_BF506F
	"${MAKE}" -fmakefile.mk -CBF504F squeaky ${NEW_FLAGS}
dist_clean_BF504F:
	"${MAKE}" -fmakefile.mk -CBF504F dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF504F:install_BF506F
	"${MAKE}" -fmakefile.mk -CBF504F install ${INSTALL_FLAGS} -i
install_debug_BF504F:  install_debug_BF506F
	"${MAKE}" -fmakefile.mk -CBF504F install_debug ${INSTALL_FLAGS} -i
#####
# BF506F
#
build_BF506F:
	"${MAKE}" -fmakefile.mk -CBF506F ${NEW_FLAGS}
clean_BF506F:
	"${MAKE}" -fmakefile.mk -CBF506F clean ${NEW_FLAGS}
squeaky_BF506F:
	"${MAKE}" -fmakefile.mk -CBF506F squeaky ${NEW_FLAGS}
dist_clean_BF506F:
	"${MAKE}" -fmakefile.mk -CBF506F dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF506F: install_base
	"${MAKE}" -fmakefile.mk -CBF506F install ${INSTALL_FLAGS} -i
install_debug_BF506F:   install_base
	"${MAKE}" -fmakefile.mk -CBF506F install_debug ${INSTALL_FLAGS} -i
#####
# BF512
#
build_BF512:
	"${MAKE}" -fmakefile.mk -CBF512 ${NEW_FLAGS}
clean_BF512:
	"${MAKE}" -fmakefile.mk -CBF512 clean ${NEW_FLAGS}
squeaky_BF512:
	"${MAKE}" -fmakefile.mk -CBF512 squeaky ${NEW_FLAGS}
dist_clean_BF512:
	"${MAKE}" -fmakefile.mk -CBF512 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF512:
	"${MAKE}" -fmakefile.mk -CBF512 install ${INSTALL_FLAGS} -i
install_debug_BF512:
	"${MAKE}" -fmakefile.mk -CBF512 install_debug ${INSTALL_FLAGS} -i
#####
# BF514
#
build_BF514: build_BF512
clean_BF514: clean_BF512
	"${MAKE}" -fmakefile.mk -CBF514 clean ${NEW_FLAGS}
squeaky_BF514: squeaky_BF512
	"${MAKE}" -fmakefile.mk -CBF514 squeaky ${NEW_FLAGS}
dist_clean_BF514:
	"${MAKE}" -fmakefile.mk -CBF514 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF514:install_BF512
	"${MAKE}" -fmakefile.mk -CBF514 install ${INSTALL_FLAGS} -i
install_debug_BF514:  install_debug_BF512
	"${MAKE}" -fmakefile.mk -CBF514 install_debug ${INSTALL_FLAGS} -i
#####
# BF516
#
build_BF516: build_BF512
clean_BF516: clean_BF512
	"${MAKE}" -fmakefile.mk -CBF516 clean ${NEW_FLAGS}
squeaky_BF516: squeaky_BF512
	"${MAKE}" -fmakefile.mk -CBF516 squeaky ${NEW_FLAGS}
dist_clean_BF516:
	"${MAKE}" -fmakefile.mk -CBF516 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF516:install_BF512
	"${MAKE}" -fmakefile.mk -CBF516 install ${INSTALL_FLAGS} -i
install_debug_BF516:  install_debug_BF512
	"${MAKE}" -fmakefile.mk -CBF516 install_debug ${INSTALL_FLAGS} -i
#####
# BF518
#
build_BF518: build_BF512
clean_BF518: clean_BF512
	"${MAKE}" -fmakefile.mk -CBF518 clean ${NEW_FLAGS}
squeaky_BF518: squeaky_BF512
	"${MAKE}" -fmakefile.mk -CBF518 squeaky ${NEW_FLAGS}
dist_clean_BF518:
	"${MAKE}" -fmakefile.mk -CBF518 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF518:install_BF512
	"${MAKE}" -fmakefile.mk -CBF518 install ${INSTALL_FLAGS} -i
install_debug_BF518:  install_debug_BF512
	"${MAKE}" -fmakefile.mk -CBF518 install_debug ${INSTALL_FLAGS} -i
#####
# BF522
#
build_BF522:
	"${MAKE}" -fmakefile.mk -CBF522 ${NEW_FLAGS}
clean_BF522:
	"${MAKE}" -fmakefile.mk -CBF522 clean ${NEW_FLAGS}
squeaky_BF522:
	"${MAKE}" -fmakefile.mk -CBF522 squeaky ${NEW_FLAGS}
dist_clean_BF522:
	"${MAKE}" -fmakefile.mk -CBF522 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF522:
	"${MAKE}" -fmakefile.mk -CBF522 install ${INSTALL_FLAGS} -i
install_debug_BF522:
	"${MAKE}" -fmakefile.mk -CBF522 install_debug ${INSTALL_FLAGS} -i

#####
# BF523
#
build_BF523:
	"${MAKE}" -fmakefile.mk -CBF523 ${NEW_FLAGS}
clean_BF523:
	"${MAKE}" -fmakefile.mk -CBF523 clean ${NEW_FLAGS}
squeaky_BF523:
	"${MAKE}" -fmakefile.mk -CBF523 squeaky ${NEW_FLAGS}
dist_clean_BF523:
	"${MAKE}" -fmakefile.mk -CBF523 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF523:
	"${MAKE}" -fmakefile.mk -CBF523 install ${INSTALL_FLAGS} -i
install_debug_BF523:
	"${MAKE}" -fmakefile.mk -CBF523 install_debug ${INSTALL_FLAGS} -i

#####
# BF524
#
build_BF524: build_BF522
	"${MAKE}" -fmakefile.mk -CBF524 ${NEW_FLAGS}
clean_BF524: clean_BF522
	"${MAKE}" -fmakefile.mk -CBF524 clean ${NEW_FLAGS}
squeaky_BF524: squeaky_BF522
	"${MAKE}" -fmakefile.mk -CBF524 squeaky ${NEW_FLAGS}
dist_clean_BF524:
	"${MAKE}" -fmakefile.mk -CBF524 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF524:install_BF522
	"${MAKE}" -fmakefile.mk -CBF524 install ${INSTALL_FLAGS} -i
install_debug_BF524:  install_debug_BF522
	"${MAKE}" -fmakefile.mk -CBF524 install_debug ${INSTALL_FLAGS} -i
#####
# BF525
#
build_BF525: build_BF523
clean_BF525: clean_BF523
	"${MAKE}" -fmakefile.mk -CBF525 clean ${NEW_FLAGS}
squeaky_BF525: squeaky_BF523
	"${MAKE}" -fmakefile.mk -CBF525 squeaky ${NEW_FLAGS}
dist_clean_BF525:
	"${MAKE}" -fmakefile.mk -CBF525 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF525:install_BF523
	"${MAKE}" -fmakefile.mk -CBF525 install ${INSTALL_FLAGS} -i
install_debug_BF525:  install_debug_BF523
	"${MAKE}" -fmakefile.mk -CBF525 install_debug ${INSTALL_FLAGS} -i

#####
# BF526
#
build_BF526: build_BF522
	"${MAKE}" -fmakefile.mk -CBF526 ${NEW_FLAGS}
clean_BF526: clean_BF522
	"${MAKE}" -fmakefile.mk -CBF526 clean ${NEW_FLAGS}
squeaky_BF526: squeaky_BF522
	"${MAKE}" -fmakefile.mk -CBF526 squeaky ${NEW_FLAGS}
dist_clean_BF526:
	"${MAKE}" -fmakefile.mk -CBF526 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF526:install_BF522
	"${MAKE}" -fmakefile.mk -CBF526 install ${INSTALL_FLAGS} -i
install_debug_BF526:  install_debug_BF522
	"${MAKE}" -fmakefile.mk -CBF526 install_debug ${INSTALL_FLAGS} -i

#####
# BF527
#
build_BF527: build_BF523
clean_BF527: clean_BF523
	"${MAKE}" -fmakefile.mk -CBF527 clean ${NEW_FLAGS}
squeaky_BF527: squeaky_BF523
	"${MAKE}" -fmakefile.mk -CBF527 squeaky ${NEW_FLAGS}
dist_clean_BF527:
	"${MAKE}" -fmakefile.mk -CBF527 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF527:install_BF523
	"${MAKE}" -fmakefile.mk -CBF527 install ${INSTALL_FLAGS} -i
install_debug_BF527:  install_debug_BF523
	"${MAKE}" -fmakefile.mk -CBF527 install_debug ${INSTALL_FLAGS} -i

build_21532_dir:
	"${MAKE}" -fmakefile.mk -C21532 ${NEW_FLAGS} REVISION=${REVISION}
clean_21532_dir:
	"${MAKE}" -fmakefile.mk -C21532 clean ${NEW_FLAGS}
squeaky_21532_dir:
	"${MAKE}" -fmakefile.mk -C21532 squeaky ${NEW_FLAGS}
dist_clean_21532_dir:
	"${MAKE}" -fmakefile.mk -C21532 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_21532_dir:  install_base
	"${MAKE}" --debug=a -fmakefile.mk -C21532 install ${INSTALL_FLAGS} -i
install_debug_21532_dir:  install_base
	"${MAKE}" -fmakefile.mk -C21532 install_debug ${INSTALL_FLAGS} -i

#####
# 21535
#
build_21535:
	"${MAKE}" -fmakefile.mk -C21535 ${NEW_FLAGS} REVISION=${REVISION}
clean_21535:
	"${MAKE}" -fmakefile.mk -C21535 clean ${NEW_FLAGS}
squeaky_21535:
	"${MAKE}" -fmakefile.mk -C21535 squeaky ${NEW_FLAGS}
dist_clean_21535:
	"${MAKE}" -fmakefile.mk -C21535 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_21535: install_base
	"${MAKE}" -fmakefile.mk -C21535 install ${INSTALL_FLAGS} -i
install_debug_21535:  install_base
	"${MAKE}" -fmakefile.mk -C21535 install_debug ${INSTALL_FLAGS} -i

#####
# BF533
#
build_BF533: build_21532
clean_BF533: clean_21532
	"${MAKE}" -fmakefile.mk -CBF533 clean ${NEW_FLAGS}
squeaky_BF533: squeaky_21532
	"${MAKE}" -fmakefile.mk -CBF533 squeaky ${NEW_FLAGS}
dist_clean_BF533:
	"${MAKE}" -fmakefile.mk -CBF533 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF533:install_21532
	"${MAKE}"  -fmakefile.mk -CBF533 install ${INSTALL_FLAGS} -i
install_debug_BF533:  install_debug_21532
	"${MAKE}" -fmakefile.mk -CBF533 install_debug ${INSTALL_FLAGS} -i

#####
# BF539
#
build_BF539: build_BF534
clean_BF539: clean_BF534
	"${MAKE}" -fmakefile.mk -CBF539 clean ${NEW_FLAGS}
squeaky_BF539: squeaky_BF534
	"${MAKE}" -fmakefile.mk -CBF539 squeaky ${NEW_FLAGS}
dist_clean_BF539:
	"${MAKE}" -fmakefile.mk -CBF539 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF539:install_base
	"${MAKE}" -fmakefile.mk -CBF539 install ${INSTALL_FLAGS} -i
install_debug_BF539:  install_base
	"${MAKE}" -fmakefile.mk -CBF539 install_debug ${INSTALL_FLAGS} -i
#####
# BF534
#
build_BF534:
	"${MAKE}" -fmakefile.mk -CBF534 ${NEW_FLAGS}
clean_BF534:
	"${MAKE}" -fmakefile.mk -CBF534 clean ${NEW_FLAGS}
squeaky_BF534:
	"${MAKE}" -fmakefile.mk -CBF534 squeaky ${NEW_FLAGS}
dist_clean_BF534:
	"${MAKE}" -fmakefile.mk -CBF534 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF534:install_base
	"${MAKE}" -fmakefile.mk -CBF534 install ${INSTALL_FLAGS} -i
install_debug_BF534:  install_base
	"${MAKE}" -fmakefile.mk -CBF534 install_debug ${INSTALL_FLAGS} -i
#####
# BF536
#
build_BF536: build_BF534
clean_BF536: clean_BF534
	"${MAKE}" -fmakefile.mk -CBF536 clean ${NEW_FLAGS}
squeaky_BF536: squeaky_BF534
	"${MAKE}" -fmakefile.mk -CBF536 squeaky ${NEW_FLAGS}
dist_clean_BF536:
	"${MAKE}" -fmakefile.mk -CBF536 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF536:install_BF534
	"${MAKE}" -fmakefile.mk -CBF536 install ${INSTALL_FLAGS} -i
install_debug_BF536:  install_debug_BF534
	"${MAKE}" -fmakefile.mk -CBF536 install_debug ${INSTALL_FLAGS} -i
#####
# BF537
#
build_BF537: build_BF534
clean_BF537: clean_BF534
	"${MAKE}" -fmakefile.mk -CBF537 clean ${NEW_FLAGS}
squeaky_BF537: squeaky_BF534
	"${MAKE}" -fmakefile.mk -CBF537 squeaky ${NEW_FLAGS}
dist_clean_BF537:
	"${MAKE}" -fmakefile.mk -CBF537 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF537:install_BF534
	"${MAKE}" -fmakefile.mk -CBF537 install ${INSTALL_FLAGS} -i
install_debug_BF537:  install_debug_BF534
	"${MAKE}" -fmakefile.mk -CBF537 install_debug ${INSTALL_FLAGS} -i
#####
# BF538
#
build_BF538: build_BF534
clean_BF538: clean_BF534
	"${MAKE}" -fmakefile.mk -CBF538 clean ${NEW_FLAGS}
squeaky_BF538: squeaky_BF534
	"${MAKE}" -fmakefile.mk -CBF538 squeaky ${NEW_FLAGS}
dist_clean_BF538:
	"${MAKE}" -fmakefile.mk -CBF538 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF538:install_BF534
	"${MAKE}" -fmakefile.mk -CBF538 install ${INSTALL_FLAGS} -i
install_debug_BF538:  install_debug_BF534
	"${MAKE}" -fmakefile.mk -CBF538 install_debug ${INSTALL_FLAGS} -i
#####
# BF542
#
build_BF542:
	"${MAKE}" -fmakefile.mk -CBF542 ${NEW_FLAGS}
clean_BF542:
	"${MAKE}" -fmakefile.mk -CBF542 clean ${NEW_FLAGS}
squeaky_BF542:
	"${MAKE}" -fmakefile.mk -CBF542 squeaky ${NEW_FLAGS}
dist_clean_BF542:
	"${MAKE}" -fmakefile.mk -CBF542 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF542:
	"${MAKE}" -fmakefile.mk -CBF542 install ${INSTALL_FLAGS} -i
install_debug_BF542:
	"${MAKE}" -fmakefile.mk -CBF542 install_debug ${INSTALL_FLAGS} -i
#####
# BF544
#
build_BF544: build_BF542
clean_BF544: clean_BF542
	"${MAKE}" -fmakefile.mk -CBF544 clean ${NEW_FLAGS}
squeaky_BF544: squeaky_BF542
	"${MAKE}" -fmakefile.mk -CBF544 squeaky ${NEW_FLAGS}
dist_clean_BF544:
	"${MAKE}" -fmakefile.mk -CBF544 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF544:install_BF542
	"${MAKE}" -fmakefile.mk -CBF544 install ${INSTALL_FLAGS} -i
install_debug_BF544:  install_debug_BF542
	"${MAKE}" -fmakefile.mk -CBF544 install_debug ${INSTALL_FLAGS} -i
#####
# BF547
#
build_BF547: build_BF542
clean_BF547: clean_BF542
	"${MAKE}" -fmakefile.mk -CBF547 clean ${NEW_FLAGS}
squeaky_BF547: squeaky_BF542
	"${MAKE}" -fmakefile.mk -CBF547 squeaky ${NEW_FLAGS}
dist_clean_BF547:
	"${MAKE}" -fmakefile.mk -CBF547 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF547:install_BF542
	"${MAKE}" -fmakefile.mk -CBF547 install ${INSTALL_FLAGS} -i
install_debug_BF547:  install_debug_BF542
	"${MAKE}" -fmakefile.mk -CBF547 install_debug ${INSTALL_FLAGS} -i
#####
# BF548
#
build_BF548: build_BF542
clean_BF548: clean_BF542
	"${MAKE}" -fmakefile.mk -CBF548 clean ${NEW_FLAGS}
squeaky_BF548: squeaky_BF542
	"${MAKE}" -fmakefile.mk -CBF548 squeaky ${NEW_FLAGS}
dist_clean_BF548:
	"${MAKE}" -fmakefile.mk -CBF548 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF548:install_BF542
	"${MAKE}" -fmakefile.mk -CBF548 install ${INSTALL_FLAGS} -i
install_debug_BF548:  install_debug_BF542
	"${MAKE}" -fmakefile.mk -CBF548 install_debug ${INSTALL_FLAGS} -i
#####
# BF549
#
build_BF549: build_BF542
clean_BF549: clean_BF542
	"${MAKE}" -fmakefile.mk -CBF549 clean ${NEW_FLAGS}
squeaky_BF549: squeaky_BF542
	"${MAKE}" -fmakefile.mk -CBF549 squeaky ${NEW_FLAGS}
dist_clean_BF549:
	"${MAKE}" -fmakefile.mk -CBF549 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF549:install_BF542
	"${MAKE}" -fmakefile.mk -CBF549 install ${INSTALL_FLAGS} -i
install_debug_BF549:  install_debug_BF542
	"${MAKE}" -fmakefile.mk -CBF549 install_debug ${INSTALL_FLAGS} -i
#####
# BF542M
#
build_BF542M: build_BF542
clean_BF542M: clean_BF542
	"${MAKE}" -fmakefile.mk -CBF542M clean ${NEW_FLAGS}
squeaky_BF542M: squeaky_BF542
	"${MAKE}" -fmakefile.mk -CBF542M squeaky ${NEW_FLAGS}
dist_clean_BF542M:
	"${MAKE}" -fmakefile.mk -CBF542M dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF542M:install_BF542
	"${MAKE}" -fmakefile.mk -CBF542M install ${INSTALL_FLAGS} -i
install_debug_BF542M:  install_debug_BF542
	"${MAKE}" -fmakefile.mk -CBF542M install_debug ${INSTALL_FLAGS} -i
#####
# BF544M
#
build_BF544M: build_BF542
clean_BF544M: clean_BF542
	"${MAKE}" -fmakefile.mk -CBF544M clean ${NEW_FLAGS}
squeaky_BF544M: squeaky_BF542
	"${MAKE}" -fmakefile.mk -CBF544M squeaky ${NEW_FLAGS}
dist_clean_BF544M:
	"${MAKE}" -fmakefile.mk -CBF544M dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF544M:install_BF542
	"${MAKE}" -fmakefile.mk -CBF544M install ${INSTALL_FLAGS} -i
install_debug_BF544M:  install_debug_BF542
	"${MAKE}" -fmakefile.mk -CBF544M install_debug ${INSTALL_FLAGS} -i
#####
# BF547M
#
build_BF547M: build_BF542
clean_BF547M: clean_BF542
	"${MAKE}" -fmakefile.mk -CBF547M clean ${NEW_FLAGS}
squeaky_BF547M: squeaky_BF542
	"${MAKE}" -fmakefile.mk -CBF547M squeaky ${NEW_FLAGS}
dist_clean_BF547M:
	"${MAKE}" -fmakefile.mk -CBF547M dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF547M:install_BF542
	"${MAKE}" -fmakefile.mk -CBF547M install ${INSTALL_FLAGS} -i
install_debug_BF547M:  install_debug_BF542
	"${MAKE}" -fmakefile.mk -CBF547M install_debug ${INSTALL_FLAGS} -i
#####
# BF548M
#
build_BF548M: build_BF542
clean_BF548M: clean_BF542
	"${MAKE}" -fmakefile.mk -CBF548M clean ${NEW_FLAGS}
squeaky_BF548M: squeaky_BF542
	"${MAKE}" -fmakefile.mk -CBF548M squeaky ${NEW_FLAGS}
dist_clean_BF548M:
	"${MAKE}" -fmakefile.mk -CBF548M dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF548M:install_BF542
	"${MAKE}" -fmakefile.mk -CBF548M install ${INSTALL_FLAGS} -i
install_debug_BF548M:  install_debug_BF542
	"${MAKE}" -fmakefile.mk -CBF548M install_debug ${INSTALL_FLAGS} -i
#####
# BF549M
#
build_BF549M: build_BF542
clean_BF549M: clean_BF542
	"${MAKE}" -fmakefile.mk -CBF549M clean ${NEW_FLAGS}
squeaky_BF549M: squeaky_BF542
	"${MAKE}" -fmakefile.mk -CBF549M squeaky ${NEW_FLAGS}
dist_clean_BF549M:
	"${MAKE}" -fmakefile.mk -CBF549M dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF549M:install_BF542
	"${MAKE}" -fmakefile.mk -CBF549M install ${INSTALL_FLAGS} -i
install_debug_BF549M:  install_debug_BF542
	"${MAKE}" -fmakefile.mk -CBF549M install_debug ${INSTALL_FLAGS} -i
#####
# BF531
#
build_BF531: build_21532
clean_BF531: clean_21532
	"${MAKE}" -fmakefile.mk -CBF531 clean ${NEW_FLAGS}
squeaky_BF531: squeaky_21532
	"${MAKE}" -fmakefile.mk -CBF531 squeaky ${NEW_FLAGS}
dist_clean_BF531:
	"${MAKE}" -fmakefile.mk -CBF531 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF531:install_21532
	"${MAKE}" -fmakefile.mk -CBF531 install ${INSTALL_FLAGS} -i
install_debug_BF531:  install_debug_21532
	"${MAKE}" -fmakefile.mk -CBF531 install_debug ${INSTALL_FLAGS} -i

#####
# BF561
#

build_BF561:
	"${MAKE}" -fmakefile.mk -CBF561 ${NEW_FLAGS} REVISION=${REVISION}
clean_BF561:
	"${MAKE}" -fmakefile.mk -CBF561 clean ${NEW_FLAGS}
squeaky_BF561: clean_BF561
	"${MAKE}" -fmakefile.mk -CBF561 squeaky ${NEW_FLAGS}
dist_clean_BF561:
	"${MAKE}" -fmakefile.mk -CBF561 dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF561:install_base
	"${MAKE}" -fmakefile.mk -CBF561 install ${INSTALL_FLAGS} -i
install_debug_BF561:  install_base
	"${MAKE}" -fmakefile.mk -CBF561 install_debug ${INSTALL_FLAGS} -i

#####
# BF592-A
#
build_BF592-A:
	"${MAKE}" -fmakefile.mk -CBF592-A ${NEW_FLAGS}
clean_BF592-A:
	"${MAKE}" -fmakefile.mk -CBF592-A clean ${NEW_FLAGS}
squeaky_BF592-A:
	"${MAKE}" -fmakefile.mk -CBF592-A squeaky ${NEW_FLAGS}
dist_clean_BF592-A:
	"${MAKE}" -fmakefile.mk -CBF592-A dist_clean ${NEW_FLAGS} ${INSTALL_FLAGS} -i
install_BF592-A: install_base
	"${MAKE}" -fmakefile.mk -CBF592-A install ${INSTALL_FLAGS} -i
install_debug_BF592-A:
	"${MAKE}" -fmakefile.mk -CBF592-A install_debug ${INSTALL_FLAGS} -i
#####
# Setup some flags for assembly access
#
OFFSET_CXXFLAGS	= ${subst " -c ",,${CXXFLAGS}} -BF533
offsets:		asm_offsets.h
asm_offsets.h:	GENERATE_OFFSETS.DXE
	../../common/generate_offsets.bat ADSP-BF533 $< ${VDSP_VERSION} > asm_offsets.h
GENERATE_OFFSETS.DXE:	generate_offsets.cpp
	${CXX} ${OFFSET_CXXFLAGS} ${DEFINES} ${INCLUDE_PATH} -g `cygpath -w $<` -Wremark 265
squeaky_offsets:
	${RM} asm_offsets.h generate_offsets.dxe
