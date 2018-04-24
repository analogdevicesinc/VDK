#
#   $RCSfile$
#
#   Description:
#
#   Last modified $Date$
#	Last modified by $Author$
#   $Revision$
#   $Source$
#
# -----------------------------------------------------------------------------
#  Comments: contains the most basic VDK definitions
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

RM=rm -f
CP=cp -p
MV=mv -f
RMDIR=rm -rf
MKDIR=mkdir -p
DIR=ls -1
TYPE=cat

EXAMPLES_INSTALL_DIR		= ${UP_EXAMPLES_INSTALL_DIR}

EXAMPLES_LIST	= ${subst Cache_Cntrl_BF535,,${subst InterProcessorCommunication_DD,,${subst Create_vs_Construct,,${subst makefile.mk,,${subst CVS,,${shell ${DIR}}}}}}}
README_LIST  = ${shell ls */README.txt}

dist_clean:
	@echo "*******************************************************"
	@echo "Distribution cleaning examples in ${EXAMPLES_INSTALL_DIR}"
	@echo "*******************************************************"
	${foreach the_dir,${EXAMPLES_LIST}, ${RM} ${EXAMPLES_INSTALL_DIR}/${the_dir}/*.*; } 
	${foreach the_dir,${EXAMPLES_LIST}, ${RMDIR} ${EXAMPLES_INSTALL_DIR}/${the_dir}; } 
	@echo "*******************************************************"
	@echo "Distribution cleaned examples in ${EXAMPLES_INSTALL_DIR}"
	@echo "*******************************************************"

install: 
	@echo "*******************************************************"
	@echo "Installing the examples into ${EXAMPLES_INSTALL_DIR}"
	@echo "*******************************************************"
	${MKDIR} ${EXAMPLES_INSTALL_DIR}
	${foreach readme_file,${README_LIST}, mv ${readme_file} ${readme_file}.safe; mv ${readme_file}.safe `dirname ${readme_file}`/readme.txt;}
	${foreach the_dir,${EXAMPLES_LIST}, ${MKDIR} ${EXAMPLES_INSTALL_DIR}/${the_dir}; } 
	${foreach the_dir,${EXAMPLES_LIST}, ${CP} -r ${the_dir}/* ${EXAMPLES_INSTALL_DIR}/${the_dir}; find ${EXAMPLES_INSTALL_DIR}/${the_dir} -name CVS -exec rm -r {} \; ; }
	@echo "*******************************************************"
	@echo "Examples installed into ${EXAMPLES_INSTALL_DIR}"
	@echo "*******************************************************"





