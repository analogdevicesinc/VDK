#
#   $RCSfile$
#
#   Description: Makefile for compiling the VDK
#
#   Last modified $Date$
#	Last modified by $Author$
#   $Revision$
#   $Source$
#
# -----------------------------------------------------------------------------
#  Comments: contains the most basic VDK definitions
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
# API Version Number (to be reflected by the build)
#
#  Format: 0xA.B.CD --> Version #: A.B.CD
#
###########################################################

API_VERSION = 0x05001000

###########################################################
# Command macros
###########################################################
RM=rm -f
CP=cp -p
RMDIR=rm -rf
MKDIR=mkdir -p
TYPE=cat
ECHO=@echo -n


###########################################################
# Default flags
###########################################################
ifeq (${VDSP_VERSION},5.0)
UP_INSTALL_DIR	= C:/Progra~1/Analog~1/Visual~2.0
else
ifeq (${VDSP_VERSION},4.5)
UP_INSTALL_DIR	= C:/Progra~1/Analog~1/Visual~1.5
else
UP_INSTALL_DIR	= C:/Progra~1/Analog~1/Visualdsp
endif
endif
UP_SRC		=  VDK_API_CreateThreadEx2.cpp VDK_Thread.cpp main.cpp Initialize.cpp RoundRobin.cpp exit.cpp IdleThread.cpp Semaphore.cpp VDK_API_PendSemaphore.cpp VDK_API_PostSemaphore.cpp VDK_API_CreateThread.cpp VDK_API_DestroyThread.cpp VDK_API_Regions.cpp VDK_API_CreateSemaphore.cpp VDK_API_DestroySemaphore.cpp VDK_API_Yield.cpp MessageQueue.cpp VDK_API_Message.cpp VDK_API_Periodic.cpp VDK_API_Priority.cpp VDK_API_GetThreadID.cpp VDK_API_DeviceFlags.cpp VDK_API_DeviceDrivers.cpp VDK_API_Events.cpp Event.cpp VDK_API_PoolMgr.cpp VDK_API_ThreadAcc.cpp VDK_ThreadLocalStorage.cpp VDK_API_Time.cpp VDK_RThread.cpp VDK_MemPool.cpp AllocatePool.cpp VDK_API_Error.cpp IDTable.cpp QueueMgr.cpp VDK_API_DD_CloseDevice.cpp ThreadMemoryAlloc.cpp InitBootPoolFunc.cpp InitBootDevFlagFunc.cpp BootSemaphoreInit.cpp InitBootIOObjectFunc.cpp VDK_API_IO.cpp VDK_StackMeasure.cpp VDK_HeapMarshaller.cpp VDK_PoolMarshaller.cpp NewHandler.cpp TerminateHandler.cpp VDK_RMutex.cpp VDK_API_InterruptMask.cpp VDK_API_CThreadError.cpp DispatchHook.cpp VDK_API_GetUptime.cpp VDK_API_ThreadStack.cpp VDK_API_ThreadDbg.cpp VDK_API_History.cpp VDK_RMutex_legacy.cpp VDK_API_DispatchError.cpp VDK_API_CreateMutex.cpp VDK_API_PendEvent.cpp VDK_API_MessageStatus.cpp VDK_API_SemaphoreStatus.cpp VDK_API_DeviceFlagsStatus.cpp VDK_API_PoolStatus.cpp VDK_Fifo.cpp VDK_API_CreateDeviceFlag.cpp VDK_API_DestroyDeviceFlag.cpp VDK_API_CreateMemPool.cpp VDK_API_DestroyMemPool.cpp VDK_Thread_ctor.cpp VDK_Thread_dtor.cpp VDK_Semaphore_ctor.cpp IdleThread_Run.cpp VDK_API_IsRunning.cpp VDK_API_IsInRegion.cpp


DEBUG_SRC	= VDK_History.cpp VDK_API_ReplaceHistory.cpp

ifeq (${ADI_MAKE}, 1)
CDIR = ${TOP_DIR}/common
else
CDIR          = ${subst /,/,${CURDIR}}
endif
INSTALL_HDRS =	${CDIR}/VDK_Public.h ${CDIR}/VDK_Internals.h

###########################################################
# Setup the variables that are passed to sub calls of make
# These lines should change as they progress up the calls,
# but should basically remain the same from level to level
###########################################################
CXX				= ${UP_CXX}
ifeq (${VDSP_VERSION},4.0)
CXXFLAGS		= ${UP_CXXFLAGS} -c++ -DAPI_VERSION_NUMBER_="${API_VERSION}"  -include VDK_warning_removal.h
else
#CXXFLAGS		= ${UP_CXXFLAGS} +check_bril -c++ -DAPI_VERSION_NUMBER_="${API_VERSION}" -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30 -Werror-warnings -include VDK_warning_removal.h
CXXFLAGS		= ${UP_CXXFLAGS} -c++ -DAPI_VERSION_NUMBER_="${API_VERSION}" -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30 -Werror-warnings
# -include VDK_warning_removal.h
endif
SRC				= ${UP_SRC}
OBJS			= ${SRC:.cpp=.doj}
DEBUG_OBJS		= ${DEBUG_SRC:.cpp=.doj}
VPATH			= ${CURDIR}:${UP_VPATH}
OUT_DIR			= VDK-
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH            = -I '$(shell cygpath -w ${TOP_DIR}/common)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH            = -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
BASIC_FLAGS		= UP_CXX="${CXX}"							\
			TOP_DIR=${TOP_DIR} \
					UP_VPATH="${VPATH}"						\
					FAMILY="${FAMILY}"						\
					CORE="${CORE}"							\
					DEVICE="${DEVICE}"						\
					REVISION="${SI_REV}"						\
					ADI_MAKE="${ADI_MAKE}"						\
					API_VERSION="${API_VERSION}"						\
					LIB_VERSION="${LIB_VERSION}"						\
					UP_INCLUDE_PATH="${INCLUDE_PATH}"
INSTALL_DIR			= ${UP_INSTALL_DIR}
INSTALL_FLAGS		= UP_INSTALL_DIR="${INSTALL_DIR}"		\
						UP_INSTALL_HDRS="${INSTALL_HDRS}"	\
						GENERATOR_EXE="${TOP_DIR}/common/internals_generator/internals_generator.exe"	\
						UP_INCLUDE_PATH="${INCLUDE_PATH}"	\
						--ignore-errors
DCLEAN_FLAGS		= UP_INSTALL_DIR="${INSTALL_DIR}"		\
						UP_INSTALL_HDRS="${INSTALL_HDRS}"
CLEAN_FLAGS			= UP_OBJS="${OBJS} ${DEBUG_OBJS}" --ignore-errors

ifeq (${NO_ASSERT_TMK}, 1)
   ASSERT_FLAGS=-DNDEBUG -DLCK_INLINE=inline -DTMK_INLINE=inline
endif

RELEASE_FLAGS  		= -no-annotate
DEBUG_FLAGS 		= -g
ifeq (${FAMILY},SHARC)
DEBUG_FLAGS 		+= -no-annotate
endif
VDK_DEBUG_DEFINES 	=
OFFSETS_FLAGS		= UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${VDK_DEBUG_DEFINES} "


#
# Global Projects (Processor Independant)
#
.PHONY:		all debug squeaky release
all:		debug squeaky release
squeaky:	clean
dist_clean:	squeaky


install:	install_build
install_debug: install_build
debug:
	"${MAKE}" -fmakefile.mk clean d_i ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean d_e ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean d_n ${MAKEFLAGS} ${MAKEOVERRIDES}
ifeq (${FAMILY},SHARC)
	"${MAKE}" -fmakefile.mk clean d_i_swf ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean d_e_swf ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean d_n_swf ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean d_i_nw ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean d_e_nw ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean d_n_nw ${MAKEFLAGS} ${MAKEOVERRIDES}
endif

release:
	"${MAKE}" -fmakefile.mk clean r_i ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean r_e ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean r_n ${MAKEFLAGS} ${MAKEOVERRIDES}
ifeq (${FAMILY},SHARC)
	"${MAKE}" -fmakefile.mk clean r_i_swf ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean r_e_swf ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean r_n_swf ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean r_i_nw ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean r_e_nw ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean r_n_nw ${MAKEFLAGS} ${MAKEOVERRIDES}
endif

ifeq (${FAMILY},SHARC)
debug_swf:
	"${MAKE}" -fmakefile.mk clean d_i_swf ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean d_e_swf ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean d_n_swf ${MAKEFLAGS} ${MAKEOVERRIDES}
debug_normal_word:
	"${MAKE}" -fmakefile.mk clean d_i_nw ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean d_e_nw ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean d_n_nw ${MAKEFLAGS} ${MAKEOVERRIDES}

release_swf:
	"${MAKE}" -fmakefile.mk clean r_i_swf ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean r_e_swf ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean r_n_swf ${MAKEFLAGS} ${MAKEOVERRIDES}

release_normal_word:
	"${MAKE}" -fmakefile.mk clean r_i_nw ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean r_e_nw ${MAKEFLAGS} ${MAKEOVERRIDES}
	"${MAKE}" -fmakefile.mk clean r_n_nw ${MAKEFLAGS} ${MAKEOVERRIDES}

endif

#
# Specific Archive Debug Projects (Processor Independant)
#
.PHONY:		d_i
d_i:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS}"		\
							UP_OBJS="${OBJS} ${DEBUG_OBJS}"		\
							UP_DEFINES="${ASSERT_FLAGS} ${VDK_DEBUG_DEFINES} -DVDK_INSTRUMENTATION_LEVEL_=2"		\
							OUT_DIR="debug/${OUT_DIR}i-" VDSP_VERSION=${VDSP_VERSION}
d_i:		build_it

.PHONY:		d_e
d_e:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS}"		\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=1"		\
							OUT_DIR="debug/${OUT_DIR}e-" VDSP_VERSION=${VDSP_VERSION}
d_e:		build_it

.PHONY:		d_n
d_n:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS}" 		\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=0"		\
							OUT_DIR="debug/${OUT_DIR}n-" VDSP_VERSION=${VDSP_VERSION}
d_n:		build_it

ifeq (${FAMILY},SHARC)
.PHONY:		d_i_swf
d_i_swf:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS} "	\
							UP_OBJS="${OBJS} ${DEBUG_OBJS}"		\
							UP_DEFINES="${ASSERT_FLAGS} ${VDK_DEBUG_DEFINES} -DVDK_INSTRUMENTATION_LEVEL_=2"		\
							OUT_DIR="debug/${OUT_DIR}i-" EXTRA_SUFFIX=_SWF VDSP_VERSION=${VDSP_VERSION}
d_i_swf:		build_it

.PHONY:		d_e_swf
d_e_swf:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS} "	\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=1"		\
							OUT_DIR="debug/${OUT_DIR}e-" EXTRA_SUFFIX=_SWF
d_e_swf:		build_it

.PHONY:		d_n_swf
d_n_swf:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS} " 	\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=0"		\
							OUT_DIR="debug/${OUT_DIR}n-" EXTRA_SUFFIX=_SWF VDSP_VERSION=${VDSP_VERSION}
d_n_swf:		build_it
.PHONY:		d_i_nw
d_i_nw:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS} "	\
							UP_OBJS="${OBJS} ${DEBUG_OBJS}"		\
							UP_DEFINES="${ASSERT_FLAGS} ${VDK_DEBUG_DEFINES} -DVDK_INSTRUMENTATION_LEVEL_=2"		\
							OUT_DIR="debug/${OUT_DIR}i-" EXTRA_SUFFIX=_NW VDSP_VERSION=${VDSP_VERSION}
d_i_nw:		build_it

.PHONY:		d_e_nw
d_e_nw:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS} "	\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=1"		\
							OUT_DIR="debug/${OUT_DIR}e-" EXTRA_SUFFIX=_NW
d_e_nw:		build_it

.PHONY:		d_n_nw
d_n_nw:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS} " 	\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=0"		\
							OUT_DIR="debug/${OUT_DIR}n-" EXTRA_SUFFIX=_NW VDSP_VERSION=${VDSP_VERSION}
d_n_nw:		build_it
endif
ifeq (${FAMILY},TS)
.PHONY:		d_i_ba
d_i_ba:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS} "			\
							UP_OBJS="${OBJS} ${DEBUG_OBJS}"		\
							UP_DEFINES="${ASSERT_FLAGS} ${VDK_DEBUG_DEFINES} -DVDK_INSTRUMENTATION_LEVEL_=2"		\
							OUT_DIR="debug/${OUT_DIR}i-" BYTE_ADDRESSING=_BA VDSP_VERSION=${VDSP_VERSION}
d_i_ba:		build_it

.PHONY:		d_e_ba
d_e_ba:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS} "		\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=1"		\
							OUT_DIR="debug/${OUT_DIR}e-" BYTE_ADDRESSING=_BA
d_e_ba:		build_it

.PHONY:		d_n_ba
d_n_ba:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${DEBUG_FLAGS} " 		\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=0"		\
							OUT_DIR="debug/${OUT_DIR}n-" BYTE_ADDRESSING=_BA
d_n_ba:		build_it

endif


#
# Specific Archive Release Projects (Processor Independant)
#
.PHONY:		r_i
r_i:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}" 	\
							UP_OBJS="${OBJS} ${DEBUG_OBJS}"			\
							UP_DEFINES="${ASSERT_FLAGS} ${VDK_DEBUG_DEFINES} -DVDK_INSTRUMENTATION_LEVEL_=2"			\
							OUT_DIR="release/${OUT_DIR}i-"
r_i:		build_it

.PHONY:		r_e
r_e:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}" 	\
							UP_OBJS="${OBJS}"						\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=1"					 	\
							OUT_DIR="release/${OUT_DIR}e-"
r_e:		build_it

.PHONY:		r_n
r_n:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}"	\
							UP_OBJS="${OBJS}"						\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=0"						\
							OUT_DIR="release/${OUT_DIR}n-"
r_n:		build_it
ifeq (${FAMILY},SHARC)
.PHONY:		r_i_swf
r_i_swf:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}"			\
							UP_OBJS="${OBJS} ${DEBUG_OBJS}"		\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=2"		\
							OUT_DIR="release/${OUT_DIR}i-" EXTRA_SUFFIX=_SWF
r_i_swf:		build_it

.PHONY:		r_e_swf
r_e_swf:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}"			\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=1"		\
							OUT_DIR="release/${OUT_DIR}e-" EXTRA_SUFFIX=_SWF
r_e_swf:		build_it

.PHONY:		r_n_swf
r_n_swf:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}" 			\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=0"					\
							OUT_DIR="release/${OUT_DIR}n-" EXTRA_SUFFIX=_SWF
r_n_swf:		build_it
.PHONY:		r_i_nw
r_i_nw:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}"			\
							UP_OBJS="${OBJS} ${DEBUG_OBJS}"		\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=2"		\
							OUT_DIR="release/${OUT_DIR}i-" EXTRA_SUFFIX=_NW
r_i_nw:		build_it

.PHONY:		r_e_nw
r_e_nw:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}"			\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=1"		\
							OUT_DIR="release/${OUT_DIR}e-" EXTRA_SUFFIX=_NW
r_e_nw:		build_it

.PHONY:		r_n_nw
r_n_nw:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}" 			\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=0"					\
							OUT_DIR="release/${OUT_DIR}n-" EXTRA_SUFFIX=_NW
r_n_nw:		build_it

endif
ifeq (${FAMILY},TS)

.PHONY:		r_i_ba
r_i_ba:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}"			\
							UP_OBJS="${OBJS} ${DEBUG_OBJS}"		\
							UP_DEFINES="${ASSERT_FLAGS} ${VDK_DEBUG_DEFINES} -DVDK_INSTRUMENTATION_LEVEL_=2"		\
							OUT_DIR="release/${OUT_DIR}i-" BYTE_ADDRESSING=_BA
r_i_ba:		build_it

.PHONY:		r_e_ba
r_e_ba:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}"			\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=1"		\
							OUT_DIR="release/${OUT_DIR}e-" BYTE_ADDRESSING=_BA
r_e_ba:		build_it

.PHONY:		r_n_ba
r_n_ba:		BUILD_FLAGS	= 	UP_CXXFLAGS="${CXXFLAGS} ${RELEASE_FLAGS} ${RBUILD_FLAGS}" 			\
							UP_OBJS="${OBJS}"					\
							UP_DEFINES="${ASSERT_FLAGS} -DVDK_INSTRUMENTATION_LEVEL_=0"					\
							OUT_DIR="release/${OUT_DIR}n-" BYTE_ADDRESSING=_BA
r_n_ba:		build_it

endif

w:

.PHONY:		build_it clean squeaky
ifeq (${FAMILY},)
build_it:		build_num build_SHARC build_blackfin build_num
clean:			clean_SHARC clean_blackfin
dist_clean:		dist_clean_SHARC dist_clean_blackfin
squeaky:		squeaky_SHARC squeaky_blackfin
install:		install_SHARC install_blackfin
install_debug:		install_debug_SHARC install_debug_blackfin
offsets:		offsets_SHARC offsets_blackfin
squeaky_offsets:	squeaky_offsets_SHARC	squeaky_offsets_blackfin
else
ifeq (${FAMILY},SHARC)
RBUILD_FLAGS	= -Os -always-inline -Wsuppress 1645,1646 +small_debug
build_it:			build_num				build_SHARC
clean:				clean_SHARC
dist_clean:			dist_clean_SHARC
squeaky:			squeaky_SHARC
install:			install_SHARC
install_debug:		install_debug_SHARC
offsets:			offsets_SHARC
squeaky_offsets:	squeaky_offsets_SHARC
else
ifeq (${FAMILY},BLACKFIN)
RBUILD_FLAGS	= -O +small_debug
build_it:			build_num				build_blackfin
clean:				clean_blackfin
dist_clean:			dist_clean_blackfin
squeaky:			squeaky_blackfin
install:			install_blackfin
install_debug:		install_debug_blackfin
offsets:			offsets_blackfin
squeaky_offsets:	squeaky_offsets_blackfin
else
endif
endif
endif


###########################################################
# Global build commands
###########################################################
install_build:

build_num:
	@echo "Updating the build number ================================="
	@echo -n "#define VERSION_BUILD_NUMBER_  0x"> BuildNumber.h
	exec date  +%y%m%d >> BuildNumber.h

###########################################################
# Family specific
###########################################################

###############################################################################
# SHARC
#
.PHONY:		build_SHARC clean_SHARC squeaky_SHARC install_common_SHARC install_SHARC install_debug_SHARC
build_SHARC:	FORCE
	@echo "Building SHARC ============================================"
	"${MAKE}" -fmakefile.mk -C../SHARC ${BASIC_FLAGS} ${BUILD_FLAGS}
clean_SHARC:	FORCE
	@echo "Cleaning SHARC ********************************************"
	"${MAKE}" -fmakefile.mk -C../SHARC clean ${BASIC_FLAGS} ${CLEAN_FLAGS}
squeaky_SHARC:	FORCE
	@echo "SQUEAKY Cleaning SHARC ********************************************"
	"${MAKE}" -fmakefile.mk -C../SHARC squeaky ${BASIC_FLAGS} ${CLEAN_FLAGS}
dist_clean_SHARC:	FORCE
	@echo "DIST Cleaning SHARC ********************************************"
	"${MAKE}" -fmakefile.mk -C../SHARC dist_clean ${BASIC_FLAGS} ${DCLEAN_FLAGS} ${CLEAN_FLAGS}
install_common_SHARC:	FORCE
	@echo "INSTALLING SHARC ********************************************"

install_debug_SHARC: install_common_SHARC
	"${MAKE}" -fmakefile.mk -C../SHARC install_debug ${BASIC_FLAGS} ${INSTALL_FLAGS}
	@echo "DONE INSTALL DEBUG SHARC"

install_SHARC: install_common_SHARC
	"${MAKE}" -fmakefile.mk -C../SHARC install ${BASIC_FLAGS} ${INSTALL_FLAGS}
	@echo "DONE INSTALL SHARC"

offsets_SHARC:	FORCE
	@echo "Generating SHARC offests ********************************************"
	"${MAKE}" -fmakefile.mk -C../SHARC offsets ${BASIC_FLAGS} ${OFFSETS_FLAGS}
squeaky_offsets_SHARC:	FORCE
	@echo "Squeaky cleaning SHARC offests ********************************************"
	"${MAKE}" -fmakefile.mk -C../SHARC squeaky_offsets ${BASIC_FLAGS} -i


###############################################################################
# BLACKFIN
#
.PHONY:		build_blackfin clean_blackfin squeaky_blackfin install_blackfin install_debug_blackfin install_common_blackfin
build_blackfin:		FORCE
	@echo "Building BLACKFIN ============================================"
	"${MAKE}" -fmakefile.mk -C../blackfin ${BASIC_FLAGS} ${BUILD_FLAGS}
clean_blackfin:		FORCE
	@echo "Cleaning BLACKFIN ********************************************"
	"${MAKE}" -fmakefile.mk -C../blackfin clean ${BASIC_FLAGS} ${CLEAN_FLAGS}
squeaky_blackfin:	FORCE
	@echo "SQUEAKY Cleaning BLACKFIN ********************************************"
	"${MAKE}" -fmakefile.mk -C../blackfin squeaky ${BASIC_FLAGS} ${CLEAN_FLAGS}
dist_clean_blackfin:	FORCE
	@echo "DIST Cleaning BLACKFIN ********************************************"
	"${MAKE}" -fmakefile.mk -C../blackfin dist_clean ${BASIC_FLAGS} ${DCLEAN_FLAGS} ${CLEAN_FLAGS}
install_common_blackfin:	FORCE
	@echo "INSTALLING BLACKFIN ********************************************"
install_blackfin:install_common_blackfin
	"${MAKE}" -fmakefile.mk -C../blackfin install ${BASIC_FLAGS} ${INSTALL_FLAGS}
	@echo "DONE INSTALL blackfin"
install_debug_blackfin:install_common_blackfin
	"${MAKE}" -fmakefile.mk -C../blackfin install_debug ${BASIC_FLAGS} ${INSTALL_FLAGS}
	@echo "DONE INSTALL DEBUG blackfin"
offsets_blackfin:	FORCE
	@echo "Generating BLACKFIN offests ********************************************"
	"${MAKE}" -fmakefile.mk -C../blackfin offsets ${BASIC_FLAGS} ${OFFSETS_FLAGS}
squeaky_offsets_blackfin:	FORCE
	@echo "SQUEAKY Cleaning BLACKFIN offests ********************************************"
	"${MAKE}" -fmakefile.mk -C../blackfin squeaky_offsets ${BASIC_FLAGS}

FORCE:

# This is a bogus comment that is here to test cr/lf issues....
