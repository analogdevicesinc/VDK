#
#   $RCSfile$
#
#   Description: Makefile for compiling the BF537
#
#   Last modified $Date$
#	Last modified by $Author$
#   $Revision$
#   $Source$ 
#
# -----------------------------------------------------------------------------
#  Comments: 
# -----------------------------------------------------------------------------
#  Copyright (c) 2004 Analog Devices, Inc.,  All rights reserved
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
TYPE=cat
ECHO=@echo -n

###########################################################
# Setup the variables that are passed to sub calls of make
# These lines should change as they progress up the calls,
# but should basically remain the same from level to level
###########################################################
CORE_VECS	= $(shell ${TYPE} ../21535/IntVectors.dat)
COMMA		= ,
ENDCOMMA	= ,,
ENDCODE		= \"+\",,
QUOTE		=\"
DOS_ENDL   = @cmd /c echo.
PLUS		= +
CORE_INAMES = $(filter-out $(ENDCOMMA), $(patsubst seg_%,% $(COMMA), $(CORE_VECS))$(COMMA)) 
CORE_HINAMES     = $(filter-out $(ENDCODE), $(patsubst seg_%,% $(QUOTE)$(PLUS)$(QUOTE)$(COMMA), $(CORE_VECS))$(COMMA)) 
VDKGEN          = ../../../Utilities/VdkGen/Release/VdkGen.exe
CURR_DIR	= ${subst \,\\,${shell cygpath -w `pwd`}}

ifeq (${VDSP_VERSION},5.0)
VDKGEN_FLAGS = -proc ADSP-BF537 ++vdkgen_for_rt ++VDSP_5.0  ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
EXAMPLES_DIR =Examples_35
ifeq (${VDSP_VERSION},4.5)
VDKGEN_FLAGS = -proc ADSP-BF537 ++vdkgen_for_rt ++VDSP_4.5  ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
EXAMPLES_DIR =Examples_35
else
VDKGEN_FLAGS = -proc ADSP-BF537 ++vdkgen_for_rt ++VDSP_4.0  ++install_dir '$(shell cygpath -w ${INSTALL_DIR}/..)'
EXAMPLES_DIR =Examples_35
endif
endif

VDK_FILES_LIST	= ${subst /,\\,${shell ls -1 ${EXAMPLES_DIR}/*/*.vdk}}
EXAMPLES_LIST	= ${shell find ${EXAMPLES_DIR}/* -type d \! -name CVS -prune -exec ls -1d {} \;}

######
# Generate the .INI file for this part
#
FORCE:

ADSP-BF537.ini:FORCE
	${ECHO}   ""                   						>  $@
	${DOS_ENDL}						>> $@
	${ECHO} "[ADSP-BF537]"								>> $@
	${DOS_ENDL}						>> $@
	${ECHO} ""                          				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Family = Blackfin"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "NaturalWordSize = 32"						>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "MaxUnsignedInt = 4294967295"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "MaxLong = 2147483647"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "MinLong = -2147483648"				>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Templates = $(QUOTE)blackfin\vdk\\$(QUOTE)"			>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "HeapSupport = true"						>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "LinkerFile = $(QUOTE)blackfin\ldf\VDK-BF537.ldf$(QUOTE)"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "Interrupts = $(QUOTE)$(CORE_INAMES)$(QUOTE)" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "ExceptionHandler = ExceptionHandler-BF537.asm" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Template = VDK_NMI_Asm_Source.asm" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_C_Template = VDK_NMI_C_Source.c" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "EVT_NMI_Cpp_Template = VDK_NMI_CPP_Source.cpp" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "AsmIsrCodeTemplate = BF537_ISR_Asm_Source.asm"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "CIsrCodeTemplate = VDK_ISR_C_Source.c"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "CppIsrCodeTemplate = VDK_ISR_CPP_Source.cpp"	>> $@
	${DOS_ENDL}						>> $@
	${ECHO} "ClockFrequency = 270" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "TickPeriod = 0.1" >>$@
	${DOS_ENDL}						>> $@
	${ECHO} "VdkVersion = 5.0" >>$@
	${DOS_ENDL}						>> $@	
	${ECHO} "IdleThreadStack = 256"				>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "DefaultTimerInterrupt = EVT_IVTMR"		>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "DefaultHistoryBufferSize = 256"		>> $@
	${DOS_ENDL}						>> $@	
	${ECHO} "NumContextAreas = 0" >>$@
	${DOS_ENDL}						>> $@	
	${ECHO} "NumThreadStacks = 1" >>$@
	${DOS_ENDL}						>> $@	
	${ECHO} "NumStackSizes = 1" 				>> $@
	${DOS_ENDL}						>> $@
	${ECHO}  ""                    				>> $@
	${DOS_ENDL}						>> $@
${VDKGEN}:
	"${MAKE}" -fmakefile.mk -C../../../Utilities/VdkGen
	@echo "**************************************"
	@echo "VdkGen Built"
	@echo "**************************************"

install: ADSP-BF537.ini  ${VDKGEN}
	${MKDIR} ${INSTALL_DIR}/vdk
	${MKDIR} ${INSTALL_DIR}/ldf
	${MKDIR} ${INSTALL_DIR}/lib
	${CP} VDK-BF537.ldf ${INSTALL_DIR}/ldf
	${CP} ADSP-BF537.ini ${INSTALL_DIR}/vdk
	${CP} ${VDKGEN} ${INSTALL_DIR}/vdk
	@echo "**************Creating LDF Vdk.cpp and Vdk.h files for examples ***********"
	@echo ""
	${foreach example_dir,${EXAMPLES_LIST}, cp VDK-BF537.ldf ${example_dir};}
	${foreach example_dir,${EXAMPLES_LIST}, cp ../templates/ExceptionHandler-BF537.asm.tf ${example_dir}/ExceptionHandler-BF537.asm; }
	${foreach the_kernel_file,${VDK_FILES_LIST}, ${INSTALL_DIR}/vdk/Vdkgen.exe ${VDKGEN_FLAGS} '$(shell cygpath -w ${CURR_DIR}/${the_kernel_file})'; }
	${foreach example_dir,${EXAMPLES_LIST}, ${RM} ${example_dir}/*.rbld; }
	@echo "**************LDF Vdk.cpp and Vdk.h for examples created ***********"
	@echo ""
	"${MAKE}" -i -f../../../../common/Examples/makefile.mk -C${EXAMPLES_DIR} UP_EXAMPLES_INSTALL_DIR=${INSTALL_DIR}/Examples/"ADSP-BF537\ EZ-KIT\ Lite"/VDK install

dist_clean:
	${RM} ${INSTALL_DIR}/ldf/VDK-BF537.ldf
	${RM} ${INSTALL_DIR}/vdk/ADSP-BF532.ini
	${RM} ${INT_VEC_SRCS}
squeaky: clean
	@echo "**** CLEANING EXAMPLES ****"
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK-BF537.ldf; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/ExceptionHandler-BF537.asm; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK.cpp; }
	${foreach example_dir,${EXAMPLES_LIST}, rm ${example_dir}/VDK.h; }

clean:
	${RM} ADSP-BF537.ini

