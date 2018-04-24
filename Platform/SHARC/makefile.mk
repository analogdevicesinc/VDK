#
#   $RCSfile$
#
#   Description: Makefile for compiling the RTOS on the SHARC
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
CXX			= cc21k
CXXFLAGS	= ${UP_CXXFLAGS} -reserve i0,b0,l0,m0,i1,b1,l1,m1,i8,b8,l8,m8,i9,b9,l9,m9 -no-circbuf
SRC			= ${UP_SRC} VDK_ClusterBusMarshaller.cpp
OBJS		= ${UP_OBJS} VDK_ClusterBusMarshaller.doj
VPATH		= ${CURDIR}:${UP_VPATH}
ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/SHARC)' ${UP_INCLUDE_PATH}
INTERNALS_DIR= ${TOP_DIR}/SHARC
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
INTERNALS_DIR= ${TOP_DIR}/SHARC
endif
INSTALL_HDRS= ${UP_INSTALL_HDRS}
INSTALL_DIR = ${UP_INSTALL_DIR}
DEFINES		= ${UP_DEFINES} -DDO_40_BIT
NEW_FLAGS	= UP_CXX="${CXX}"			   			\
			  UP_CXXFLAGS="${CXXFLAGS}"	   			\
			  UP_OBJS="${OBJS}"			   			\
			  UP_VPATH="${VPATH}"		   			\
			  OUT_DIR="${OUT_DIR}"		   			\
			  FAMILY="${FAMILY}"		   			\
			  UP_DEFINES="${DEFINES}"				\
			  CORE="${CORE}"			   			\
			  DEVICE="${DEVICE}"					\
			  EXTRA_SUFFIX="${EXTRA_SUFFIX}"					\
			  REVISION="${REVISION}"					\
			  UP_INCLUDE_PATH="${INCLUDE_PATH}"		\
			  TOP_DIR="${TOP_DIR}"		\
			  API_VERSION="${API_VERSION}" \
			  LIB_VERSION="${LIB_VERSION}" \
			  VDSP_VERSION="${VDSP_VERSION}"
INSTALL_FLAGS ="${INSTALL_DIR}"

#
# Core specific
#
.PHONY:		all default clean
all:		default
ifeq (${CORE},)
default:			build_2106X	build_2116X build_2126X build_2136X build_2137X build_2146X build_2147X build_2148X
clean:				clean_2106X	clean_2116X clean_2126X clean_2136X clean_2137X clean_2146X clean_2147X clean_2148X
squeaky:			squeaky_2106X	squeaky_2116X squeaky_2126X squeaky_2136X squeaky_2137X squeaky_2146X squeaky_2147X squeaky_2148X
dist_clean:		dist_clean_2106X	dist_clean_2116X dist_clean_2126X dist_clean_2136X dist_clean_2137X dist_clean_2146X dist_clean_2147X dist_clean_2148X
install:			install_2106X	install_2116X install_2126X install_2136X install_2137X install_2146X install_2147X install_2148X 
install_debug:		install_debug_2106X	install_debug_2116X install_debug_2126X install_debug_2136X install_debug_2137X install_debug_2146X install_debug_2147X install_debug_2148X
else
ifeq (${CORE},2106X)
default:			build_2106X
clean:				clean_2106X
squeaky:			squeaky_2106X
dist_clean:			dist_clean_2106X
install:			install_2106X
install_debug:		install_debug_2106X
else
ifeq (${CORE},2116X)
default:			build_2116X
clean:				clean_2116X
squeaky:			squeaky_2116X
dist_clean:			dist_clean_2116X
install:			install_2116X
install_debug:		install_debug_2116X
else
ifeq (${CORE},2126X)
default:			build_2126X
clean:				clean_2126X
squeaky:			squeaky_2126X
dist_clean:			dist_clean_2126X
install:			install_2126X
install_debug:		install_debug_2126X
else
ifeq (${CORE},2136X)
default:			build_2136X
clean:				clean_2136X
squeaky:			squeaky_2136X
dist_clean:			dist_clean_2136X
install_debug:		install_debug_2136X
install:			install_2136X
else
ifeq (${CORE},2137X)
default:			build_2137X
clean:				clean_2137X
squeaky:			squeaky_2137X
dist_clean:			dist_clean_2137X
install_debug:		install_debug_2137X
install:			install_2137X
else
ifeq (${CORE},2146X)
default:			build_2146X
clean:				clean_2146X
squeaky:			squeaky_2146X
dist_clean:			dist_clean_2146X
install_debug:		install_debug_2146X
install:			install_2146X
else
ifeq (${CORE},2147X)
default:            build_2147X
clean:              clean_2147X
squeaky:            squeaky_2147X
dist_clean:         dist_clean_2147X
install_debug:      install_debug_2147X
install:            install_2147X
else
ifeq (${CORE},2148X)
default:            build_2148X
clean:              clean_2148X
squeaky:            squeaky_2148X
dist_clean:         dist_clean_2148X
install_debug:      install_debug_2148X
install:            install_2148X
endif
endif
endif
endif
endif
endif
endif
endif
endif

offsets:

squeaky_offsets:

#
# Setup some flags for assembly access
#

OFFSET_CXXFLAGS = ${subst " -c ",,${CXXFLAGS}}
offsets:        asm_offsets.h
asm_offsets.h:  GENERATE_OFFSETS.DXE
	../common/generate_offsets.bat ADSP-21060 $< ${VDSP_VERSION} > asm_offsets.h
GENERATE_OFFSETS.DXE:   generate_offsets.cpp
	${CXX} -proc ADSP-21060 ${OFFSET_CXXFLAGS} ${DEFINES} ${INCLUDE_PATH} -g `cygpath -w $<` -Wremark 265 -Wsuppress 1967
squeaky_offsets:
	${RM} asm_offsets.h generate_offsets.dxe

#
# Core specific
#

#
# 2106X
#
build_2106X:
ifneq (${EXTRA_SUFFIX},)
	@echo "ADSP-2106x does not have SWF, SW or NW libs"
else
	"${MAKE}" -fmakefile.mk -C2106X ${NEW_FLAGS}
endif
clean_2106X:
	"${MAKE}" -fmakefile.mk -C2106X clean ${NEW_FLAGS}
squeaky_2106X:
	"${MAKE}" -fmakefile.mk -C2106X squeaky ${NEW_FLAGS}
dist_clean_2106X:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/21k/include/${notdir ${file}};} 
	"${MAKE}" -fmakefile.mk -C2106X dist_clean INSTALL_DIR="${INSTALL_DIR}/21k" --ignore-errors
install_2106X: 
	${MKDIR} ${INSTALL_DIR}/21k/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/21k/include/${notdir ${file}} -I ${INTERNALS_DIR}/2106x ${INCLUDE_PATH}; } 
	"${MAKE}" -fmakefile.mk -C2106X install INSTALL_DIR="${INSTALL_DIR}/21k"
install_debug_2106X: 
	${MKDIR} ${INSTALL_DIR}/21k/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/21k/include/${notdir ${file}} -I ${INTERNALS_DIR}/2106x ${INCLUDE_PATH}; } 
	"${MAKE}" -fmakefile.mk -C2106X install_debug INSTALL_DIR="${INSTALL_DIR}/21k"

#
# 2116X
#
build_2116X:
ifeq (${EXTRA_SUFFIX},_SW)
	@echo "ADSP-2116x does not have SW libs"
else
ifeq (${EXTRA_SUFFIX},_NW)
	@echo "ADSP-2116x does not have NW libs"
else
	"${MAKE}" -fmakefile.mk -C2116X ${NEW_FLAGS}
endif
endif

clean_2116X:
	"${MAKE}" -fmakefile.mk -C2116X clean ${NEW_FLAGS}
squeaky_2116X:
	"${MAKE}" -fmakefile.mk -C2116X squeaky ${NEW_FLAGS}
dist_clean_2116X:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/211xx/include/${notdir ${file}};} 
	"${MAKE}" -fmakefile.mk -C2116X dist_clean INSTALL_DIR="${INSTALL_FLAGS}/211XX" --ignore-errors
install_2116X:
	${MKDIR} ${INSTALL_DIR}/211xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/211xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2116x ${INCLUDE_PATH};} 
	"${MAKE}" -fmakefile.mk -C2116X install INSTALL_DIR="${INSTALL_FLAGS}/211XX"
install_debug_2116X:
	${MKDIR} ${INSTALL_DIR}/211xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/211xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2116x ${INCLUDE_PATH};} 
	"${MAKE}" -fmakefile.mk -C2116X install_debug INSTALL_DIR="${INSTALL_FLAGS}/211XX"


#
# 2126X
#
build_2126X:
ifneq (${EXTRA_SUFFIX},)
	@echo "ADSP-2126x does not have SWF, SW or NW libs"
else
	"${MAKE}" -fmakefile.mk -C2126X ${NEW_FLAGS}
endif
clean_2126X:
	"${MAKE}" -fmakefile.mk -C2126X clean ${NEW_FLAGS}
squeaky_2126X:
	"${MAKE}" -fmakefile.mk -C2126X squeaky ${NEW_FLAGS}
dist_clean_2126X:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/212xx/include/${notdir ${file}};} 
	"${MAKE}" -fmakefile.mk -C2126X dist_clean INSTALL_DIR="${INSTALL_FLAGS}/212XX" --ignore-errors
install_2126X:
	${MKDIR} ${INSTALL_DIR}/212xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/212xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2126x ${INCLUDE_PATH};} 
	"${MAKE}" -fmakefile.mk -C2126X install INSTALL_DIR="${INSTALL_FLAGS}/212XX"
install_debug_2126X:
	${MKDIR} ${INSTALL_DIR}/212xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/212xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2126x ${INCLUDE_PATH};} 
	"${MAKE}" -fmakefile.mk -C2126X install_debug INSTALL_DIR="${INSTALL_FLAGS}/212XX"

#
# 2136X
#
build_2136X:
ifneq (${EXTRA_SUFFIX},)
	@echo "ADSP-2126x does not have SWF, SW or NW libs"
else
	"${MAKE}" -fmakefile.mk -C2136X ${NEW_FLAGS}
endif
clean_2136X:
	"${MAKE}" -fmakefile.mk -C2136X clean ${NEW_FLAGS}
squeaky_2136X:
	"${MAKE}" -fmakefile.mk -C2136X squeaky ${NEW_FLAGS}
dist_clean_2136X:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/213xx/include/${notdir ${file}};}
	"${MAKE}" -fmakefile.mk -C2136X dist_clean INSTALL_DIR="${INSTALL_FLAGS}/213XX" --ignore-errors
install_2136X:
	${MKDIR} ${INSTALL_DIR}/213xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/213xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2136x ${INCLUDE_PATH};}
	"${MAKE}" -fmakefile.mk -C2136X install INSTALL_DIR="${INSTALL_FLAGS}/213XX"
install_debug_2136X:
	${MKDIR} ${INSTALL_DIR}/213xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/213xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2136x ${INCLUDE_PATH};}
	"${MAKE}" -fmakefile.mk -C2136X install_debug INSTALL_DIR="${INSTALL_FLAGS}/213XX"
#
# 2137X
#
build_2137X:
ifneq (${EXTRA_SUFFIX},)
	@echo "ADSP-2126x does not have SWF, SW or NW libs"
else
	"${MAKE}" -fmakefile.mk -C2137X ${NEW_FLAGS}
endif
clean_2137X:
	"${MAKE}" -fmakefile.mk -C2137X clean ${NEW_FLAGS}
squeaky_2137X:
	"${MAKE}" -fmakefile.mk -C2137X squeaky ${NEW_FLAGS}
dist_clean_2137X:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/213xx/include/${notdir ${file}};}
	"${MAKE}" -fmakefile.mk -C2137X dist_clean INSTALL_DIR="${INSTALL_FLAGS}/213XX" --ignore-errors
install_2137X:
	${MKDIR} ${INSTALL_DIR}/213xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/213xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2137x ${INCLUDE_PATH};}
	"${MAKE}" -fmakefile.mk -C2137X install INSTALL_DIR="${INSTALL_FLAGS}/213XX"
install_debug_2137X:
	${MKDIR} ${INSTALL_DIR}/213xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/213xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2137x ${INCLUDE_PATH};}
	"${MAKE}" -fmakefile.mk -C2137X install_debug INSTALL_DIR="${INSTALL_FLAGS}/213XX"
#
# 2146X
#
build_2146X:
ifeq (${EXTRA_SUFFIX},_SWF)
	@echo "ADSP-2146x does not have SWF libs"
else
	"${MAKE}" -fmakefile.mk -C2146X ${NEW_FLAGS}
endif

clean_2146X:
	"${MAKE}" -fmakefile.mk -C2146X clean ${NEW_FLAGS}
squeaky_2146X:
	"${MAKE}" -fmakefile.mk -C2146X squeaky ${NEW_FLAGS}
dist_clean_2146X:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/214xx/include/${notdir ${file}};}
	"${MAKE}" -fmakefile.mk -C2146X dist_clean INSTALL_DIR="${INSTALL_FLAGS}/214XX" --ignore-errors
install_2146X:
	${MKDIR} ${INSTALL_DIR}/214xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/214xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2146x ${INCLUDE_PATH};}
	"${MAKE}" -fmakefile.mk -C2146X install INSTALL_DIR="${INSTALL_FLAGS}/214XX"
install_debug_2146X:
	${MKDIR} ${INSTALL_DIR}/214xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/214xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2146x ${INCLUDE_PATH};}
	"${MAKE}" -fmakefile.mk -C2146X install_debug INSTALL_DIR="${INSTALL_FLAGS}/214XX"

#
# 2147X
#
build_2147X:
ifeq (${EXTRA_SUFFIX},_SWF)
	@echo "ADSP-2147x does not have SWF libs"
else
	"${MAKE}" -fmakefile.mk -C2147X ${NEW_FLAGS}
endif

clean_2147X:
	"${MAKE}" -fmakefile.mk -C2147X clean ${NEW_FLAGS}
squeaky_2147X:
	"${MAKE}" -fmakefile.mk -C2147X squeaky ${NEW_FLAGS}
dist_clean_2147X:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/214xx/include/${notdir ${file}};}
	"${MAKE}" -fmakefile.mk -C2147X dist_clean INSTALL_DIR="${INSTALL_FLAGS}/214XX" --ignore-errors
install_2147X:
	${MKDIR} ${INSTALL_DIR}/214xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/214xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2147x -I ${INTERNALS_DIR}/2146x ${INCLUDE_PATH};}
	"${MAKE}" -fmakefile.mk -C2147X install INSTALL_DIR="${INSTALL_FLAGS}/214XX"

install_debug_2147X:
	${MKDIR} ${INSTALL_DIR}/214xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/214xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2147x -I ${INTERNALS_DIR}/2146x ${INCLUDE_PATH};}
	"${MAKE}" -fmakefile.mk -C2147X install_debug INSTALL_DIR="${INSTALL_FLAGS}/214XX"

#
# 2148X
#
build_2148X:
ifeq (${EXTRA_SUFFIX},_SWF)
	@echo "ADSP-2148x does not have SWF libs"
else
	"${MAKE}" -fmakefile.mk -C2148X ${NEW_FLAGS}
endif

clean_2148X:
	"${MAKE}" -fmakefile.mk -C2148X clean ${NEW_FLAGS}
squeaky_2148X:
	"${MAKE}" -fmakefile.mk -C2148X squeaky ${NEW_FLAGS}
dist_clean_2148X:
	${foreach file,${INSTALL_HDRS},${RM} ${INSTALL_DIR}/214xx/include/${notdir ${file}};}
	"${MAKE}" -fmakefile.mk -C2148X dist_clean INSTALL_DIR="${INSTALL_FLAGS}/214XX" --ignore-errors
install_2148X:
	${MKDIR} ${INSTALL_DIR}/214xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/214xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2148x -I ${INTERNALS_DIR}/2146x ${INCLUDE_PATH};}
	"${MAKE}" -fmakefile.mk -C2148X install INSTALL_DIR="${INSTALL_FLAGS}/214XX"
install_debug_2148X:
	${MKDIR} ${INSTALL_DIR}/214xx/include
	${foreach file,${INSTALL_HDRS},${GENERATOR_EXE} ${file} -o ${INSTALL_DIR}/214xx/include/${notdir ${file}} -I ${INTERNALS_DIR}/2148x -I ${INTERNALS_DIR}/2146x ${INCLUDE_PATH};}
	"${MAKE}" -fmakefile.mk -C2148X install_debug INSTALL_DIR="${INSTALL_FLAGS}/214XX"
