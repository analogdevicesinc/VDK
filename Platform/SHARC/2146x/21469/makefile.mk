#
#   $RCSfile$
#
#   Description: Makefile for compiling the 21469
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
TYPE=cat
ECHO=@echo -n

###########################################################
# Setup the variables that are passed to sub calls of make
# These lines should change as they progress up the calls,
# but should basically remain the same from level to level
###########################################################
PLATFORM=ADSP-21469
ASM			= EASM21K
ifeq (${NESTED_MAKE}, YES)
ASMFLAGS	= ${UP_ASMFLAGS}
else
ASMFLAGS	= ${UP_ASMFLAGS} -proc ADSP-21469 -file-attr ADI_OS=VDK -file-attr prefersMem=internal -file-attr prefersMemNum=30 -double-size-any -Wwarn-error
endif
CXX			= ${UP_CXX}
CXXFLAGS	= -proc ADSP-21469 ${UP_CXXFLAGS} -double-size-any

ifeq (${EXTRA_SUFFIX},_NW)
ASMFLAGS        += -nwc
SUFFIX = _nwc
else
ASMFLAGS        += -D__SHORT_WORD_CODE__
endif

NEW_FLAGS       = UP_CXX="${CXX}"                                       \
                          UP_CXXFLAGS="${UP_CXXFLAGS}"          \
                          UP_ASMFLAGS="${ASMFLAGS}"             \
                          UP_VPATH="${VPATH}"                           \
                          OUT_DIR="${OUT_DIR}"                          \
                          FAMILY="${FAMILY}"                            \
                          UP_DEFINES="${DEFINES}"                       \
                          REVISION="${REVISION}"                        \
                          CORE="${CORE}" \
                          DEVICE="${DEVICE}"                            \
                          UP_INCLUDE_PATH="${INCLUDE_PATH}"     \
                          VDSP_VERSION="${VDSP_VERSION}"
SRC		= TimerInit.cpp IMASKInit.cpp Version.cpp

ifeq (${NESTED_MAKE}, YES)
CORE_VECS       = $(shell ${TYPE} ../../IntVectors.dat)
CORE_VECS2      = $(shell ${TYPE} ../../IntVectors2.dat)
TMR_INTERRUPTS  = $(shell ${TYPE} ../../TimerInt.dat)
CORE_IRQNAME    = $(shell ${TYPE} ../../IntIRQs.dat)
CORE_NOIRQNAME  = $(shell ${TYPE} ../../IntNoIRQs.dat)
DEFINES		= ${UP_DEFINES}
else
CORE_VECS       = $(shell ${TYPE} ../IntVectors.dat)
CORE_VECS2      = $(shell ${TYPE} ../IntVectors2.dat)
TMR_INTERRUPTS  = $(shell ${TYPE} ../TimerInt.dat)
CORE_IRQNAME    = $(shell ${TYPE} ../IntIRQs.dat)
CORE_NOIRQNAME  = $(shell ${TYPE} ../IntNoIRQs.dat)
DEFINES		= ${UP_DEFINES} -D VERSION_PRODUCT_="VDK::k21469" -DVDK_INT_=INT_INT14_P
endif
CORE_IRQS   = $(addsuffix .asm, $(CORE_IRQNAME))
CORE_NOIRQS = $(addsuffix .asm, $(CORE_NOIRQNAME))
CORE_CB7I   = seg_CB7I.asm
CORE_SRCS   = $(CORE_IRQS) $(CORE_NOIRQS) $(CORE_CB7I) VDK_hdr_SHARC.asm panic.asm adi_stack_overflow.asm
COMMA		= ,
PLUS		= +
ENDCOMMA	= ,,
ENDCODE		= \"+\",,
QUOTE		= \"
DOS_ENDL   = @cmd /c echo.
CORE_INAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA))
CORE_INAMES2 = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS2))$(COMMA))
CORE_HLIST	= $(filter-out $(ENDCOMMA), $(patsubst seg_%, __seg_% $(COMMA), $(CORE_VECS) )$(COMMA))
CORE_OBJS	= $(CORE_SRCS:.asm=.doj) ${SRC:.cpp=.doj}
VPATH		= ${CURDIR}:${UP_VPATH}
OUTPUT		= ${OUT_DIR}21469${SUFFIX}.dlb

ifeq (${ADI_MAKE}, 1)
INCLUDE_PATH= -I '$(shell cygpath -w ${TOP_DIR}/2146x/21469)' ${UP_INCLUDE_PATH}
else
INCLUDE_PATH= -I '$(shell cygpath -w ${CURDIR})' ${UP_INCLUDE_PATH}
endif
VDKGEN	  = ${TOP_DIR}/Utilities/VdkGen/Release/VdkGen.exe
CURR_DIR=${subst \,\\,${shell cygpath -w `pwd`}}
EXAMPLES_DIR = Examples_35

ifeq (${VDSP_VERSION},5.0)
VDKGEN_FLAGS = -proc ADSP-21469 ++vdkgen_for_rt ++VDSP_5.0 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
else
VDKGEN_FLAGS = -proc ADSP-21469 ++vdkgen_for_rt ++VDSP_4.5 ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
endif

VDK_FILES_LIST= ${subst /,\\,${shell ls -1 ${EXAMPLES_DIR}/*/*.vdk}}
EXAMPLES_LIST   = ${shell find ${EXAMPLES_DIR}/* -type d \! -name CVS -prune -exec ls -1d {} \;}

ifeq ($(findstring debug,$(OUTPUT)),debug)
CORE_OUTPUT	= ./debug/VDK-CORE-21469${SUFFIX}.dlb
INSTALL_TYPE= debug
else
CORE_OUTPUT	= ./release/VDK-CORE-21469${SUFFIX}.dlb
INSTALL_TYPE= release
endif

#
# Projects
#
.PHONY:		all default
all:		default
default:	build_21469
build_21469_libs:$(CORE_OUTPUT)
install:install_libs install_ldfs_and_examples
install_debug:install_debug_libs install_ldfs_and_examples
squeaky: squeaky_current_dir clean

ifeq (${REVISION},all)
build_21469: asm_file_generation build_21469_any build_21469_none
clean: clean_current_dir clean_21469_any clean_21469_none
install_libs: install_21469_any install_21469_none install_ldfs_and_examples
install_debug_libs: install_debug_21469_any install_debug_21469_none
else
ifeq (${REVISION},none)
build_21469: asm_file_generation build_21469_none
clean: clean_current_dir clean_21469_none
install_libs: install_21469_none install_ldfs_and_examples
install_debug_libs: install_debug_21469_none
else
ifeq (${REVISION},any)
build_21469: asm_file_generation build_21469_any
clean: clean_current_dir clean_21469_any
install_libs: install_21469_any install_ldfs_and_examples
install_debug_libs: install_debug_21469_any
else
build_21469: asm_file_generation build_21469_any build_21469_none
clean: clean_current_dir clean_21469_any clean_21469_none
install_libs: install_21469_any install_21469_none install_ldfs_and_examples
install_debug_libs: install_debug_21469_any install_debug_21469_none
endif
endif
endif

##############################
# Fastest libraries with no workarounds
build_21469_none:
	${MKDIR} 2146x_rev_none
	"${MAKE}" -f../makefile.mk  -C 2146x_rev_none build_21469_libs ${NEW_FLAGS} VEROPTIONS="-si-revision none" UP_CXXFLAGS+="-si-revision none" UP_ASMFLAGS+="-si-revision none" NESTED_MAKE="YES"

clean_21469_none:
	${MKDIR} 2146x_rev_none
	"${MAKE}" -f../makefile.mk  -C 2146x_rev_none clean_21469_libs ${NEW_FLAGS} UP_CXXFLAGS+="-si-revision none" UP_ASMFLAGS+="-si-revision none" NESTED_MAKE=YES

install_21469_none:
	"${MAKE}" -i -fmakefile.mk  install_21469_libs ${NEW_FLAGS} SI_REV="2146x_rev_none" INST_DIR=""

install_debug_21469_none:
	"${MAKE}" -i -fmakefile.mk  install_debug_21469_libs ${NEW_FLAGS} SI_REV="2146x_rev_none" INST_DIR=""

##############################
# Valid for any silicon. All the workarounds in the world
build_21469_any:
	${MKDIR} 2146x_rev_any
	"${MAKE}" -f../makefile.mk  -C 2146x_rev_any build_21469_libs ${NEW_FLAGS} VEROPTIONS="-si-revision any" UP_CXXFLAGS+="-si-revision any" UP_ASMFLAGS+="-si-revision any" NESTED_MAKE="YES"



clean_21469_any:
	${MKDIR} 2146x_rev_any
	"${MAKE}" -f../makefile.mk  -C 2146x_rev_any clean_21469_libs ${NEW_FLAGS} NESTED_MAKE=YES

install_21469_any:
	"${MAKE}" -i -fmakefile.mk  install_21469_libs ${NEW_FLAGS} SI_REV="2146x_rev_any" INST_DIR="21469_rev_any"

install_debug_21469_any:
	"${MAKE}" -i -fmakefile.mk  install_debug_21469_libs ${NEW_FLAGS} SI_REV="2146x_rev_any" INST_DIR="21469_rev_any"

asm_file_generation: seg_int_hook.asm $(CORE_IRQS) $(CORE_NOIRQS) $(CORE_CB7I)

seg_int_hook.asm:	../IntVectors.dat
	${ECHO} " .extern "	> $@
	${ECHO} " ${CORE_HLIST} ;" 			  >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} ".SECTION/DM 	seg_int_hooks;" 	>>  $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} ".global __hook_seg_int_all;"	>> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} ".var __hook_seg_int_all[] =" 	>> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "${CORE_HLIST} ;" 				>> $@
	${DOS_ENDL}	                                     >> $@

######
# Generate the version information. It will be different in all libs
#

ver_core.txt:
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

######
# Generate the .INI file for this part
#
FORCE:

ADSP-21469.ini:FORCE
	${ECHO} ""  >  $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "[ADSP-21469]" >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} ""	                                                  >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "Family = SHARC" >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "NaturalWordSize = 32" >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "MaxUnsignedInt = 4294967295"	                       >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "MaxLong = 2147483647"                          >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "MinLong = -2147483648"                         >> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "Templates = 214xx\vdk\ "									>> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "HeapSupport = true"										>> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "LinkerFile = $(QUOTE)214xx\ldf\VDK-21469.ldf$(QUOTE)"	       >> $@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "Interrupts = $(QUOTE)$(CORE_INAMES)$(QUOTE)" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "Interrupts2 = $(QUOTE)$(CORE_INAMES2)$(QUOTE)" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "ClockFrequency = 300" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "TickPeriod = 0.1" >>$@
	${DOS_ENDL}                                             >> $@
	${ECHO} "IdleThreadStack = 256"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "DefaultTimerInterrupt = TMZHI"		>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "DefaultHistoryBufferSize = 256"		>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "NumContextAreas = 1" 				>>$@
	${DOS_ENDL}						>> $@
	${ECHO} "NumThreadStacks = 1" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "NumStackSizes = 1" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} "VdkVersion = 5.0" >>$@
	${DOS_ENDL}	                                     >> $@
	${ECHO} ""	                                                  >> $@
	${DOS_ENDL}	                                     >> $@

$(CORE_IRQS):   IntIRQs.dat
	${ECHO} "#include $(QUOTE)VDK_IntVector.h$(QUOTE)" > $@
	${DOS_ENDL}                                             >> $@
	${ECHO} " .file_attr Encoding=\"NW\"; "	>> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "INTVECTOR( $(basename $@), __InvalidIntIRQ )" >> $@
	${DOS_ENDL}                                             >> $@

$(CORE_NOIRQS): IntNoIRQs.dat
	${ECHO} "#include $(QUOTE)VDK_IntVector.h$(QUOTE)" > $@
	${DOS_ENDL}                                             >> $@
	${ECHO} " .file_attr Encoding=\"NW\"; "	>> $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "INTVECTOR( $(basename $@), __InvalidInt )" >> $@
	${DOS_ENDL}                                             >> $@

$(CORE_CB7I):  FORCE
	${ECHO} "#include $(QUOTE)VDK_IntVector.h$(QUOTE)" > $@
	${DOS_ENDL}                                             >> $@
	${ECHO} "INTVECTOR( seg_CB7I, _adi_stack_overflowed)" >> $@
	${DOS_ENDL}                                             >> $@

$(CORE_OBJS):	VDK_IntVector.h BuildNumber.h $(CORE_SRCS)

$(CORE_OUTPUT):	 ${CORE_OBJS} seg_int_hook.doj VDK_ISR_API.doj ver_core.txt
	@${MKDIR} debug
	@${MKDIR} release
	elfar -tx ver_core.txt -c $(CORE_OUTPUT) ${CORE_OBJS} seg_int_hook.doj
	@echo "--------------------"
	@echo "$(CORE_OUTPUT) build completed at:"
	@date +%c
	@echo "--------------------"
clean_21469_libs:
	${RM} *.ii
	${RM} *.ti
	${RM} ${CORE_OBJS} ${ASM_OBJS}
	${RM} ${CORE_CB7I}
	${RM} seg_int_hook.doj
	${RM} ver_core.txt

clean_current_dir:
	${RM} ADSP-21469.ini

squeaky_current_dir:
	${RM} *.ii
	${RM} *.ti
	${RM} ${CORE_OBJS}
	${RM} ${CORE_IRQS}
	${RM} ${CORE_NOIRQS}
	${RM} ADSP-21469.ini
	${RM} seg_int_hook.doj seg_int_hook.asm
	@echo "**** CLEANING EXAMPLES ****"
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK-21469.ldf; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK.cpp; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK.h; }
	${RM} ver_core.txt

${VDKGEN}:
	"${MAKE}" -fmakefile.mk -C../../../Utilities/VdkGen
	@echo "**************************************"
	@echo "VdkGen Built"
	@echo "**************************************"

install_ldfs_and_examples: install_ldfs install_examples

install_ldfs: ADSP-21469.ini ${VDKGEN}
	${MKDIR} ${INSTALL_DIR}/../214xx/ldf
	${MKDIR} ${INSTALL_DIR}/vdk
	${CP} VDK-21469.ldf ${INSTALL_DIR}/../214xx/ldf
	${CP} ADSP-21469.ini ${INSTALL_DIR}/../214xx/vdk
	${CP} ${VDKGEN} ${INSTALL_DIR}/vdk

install_examples: ADSP-21469.ini ${VDKGEN}
	@echo "**************Creating LDF Vdk.cpp and Vdk.h files for examples ***********"
	@echo ""
	${foreach example_dir,${EXAMPLES_LIST}, cp VDK-21469.ldf ${example_dir}; }
	${foreach the_kernel_file,${VDK_FILES_LIST}, ${INSTALL_DIR}/vdk/VdkGen.exe ${VDKGEN_FLAGS} '$(shell cygpath -w ${CURR_DIR}\\${the_kernel_file})'; }
	${RM} ${EXAMPLES_DIR}/MultipleHeaps/VDK-21469.ldf
	${foreach example_dir,${EXAMPLES_LIST}, ${RM} ${example_dir}/*.rbld; }
	@echo "**************LDF Vdk.cpp and Vdk.h for examples created ***********"
	"${MAKE}" -i -f../../../../common/examples/makefile.mk -C${EXAMPLES_DIR} UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/"No\ Hardware\ Required"/VDK install

install_debug_21469_libs:
	${MKDIR} ${INSTALL_DIR}/lib/${INST_DIR}
	${CP} ${SI_REV}/debug/VDK-*-21469*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

install_21469_libs:install_debug_21469_libs
	${CP} ${SI_REV}/release/VDK-*-21469*.dlb ${INSTALL_DIR}/lib/${INST_DIR}

dist_clean:
	"${MAKE}" -i -f../../../../common/examples/makefile.mk -C${EXAMPLES_DIR} UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/"No\ Hardware\ Required"/VDK dist_clean
	${RM} ${INSTALL_DIR}/ldf/VDK-21469.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-21469.ini
	${RM} ${INSTALL_DIR}/lib/VDK-*-21469*.dlb
	${RMDIR} debug
	${RMDIR} release
	${MKDIR} debug
	${MKDIR} release
