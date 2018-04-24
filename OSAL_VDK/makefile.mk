include variables.mk
.SUFFIXES:
.SUFFIXES: .c .s .asm .cpp ${OPT}.doj

CURRENT_DIR = ${shell pwd}

COMMON_OBJECTS= adi_osal_vdk${OPT}.doj adi_osal_vdk_threadtemplate${OPT}.doj adi_osal_vdk_thread${OPT}.doj adi_osal_vdk_timing${OPT}.doj adi_osal_vdk_mutex${OPT}.doj adi_osal_vdk_sem${OPT}.doj adi_osal_vdk_message${OPT}.doj adi_osal_vdk_critical${OPT}.doj adi_osal_vdk_tls${OPT}.doj adi_osal_vdk_event${OPT}.doj 

# There are no platform specific objects at the moment
ifeq (${FAMILY},blackfin)
    PLATFORM_OBJECTS =  adi_osal_vdk_int${OPT}.doj
endif
ifeq (${FAMILY},sharc)
PLATFORM_OBJECTS = 
endif

OBJECTS = ${COMMON_OBJECTS} ${PLATFORM_OBJECTS}
.c${OPT}.doj:
	$(CC) $(CCOPTS) $(ATTRIBUTES) -Werror-warnings -misra-strict -misra-no-runtime -c -o $@ `cygpath -w $<`

.cpp${OPT}.doj:
	$(CC) $(CCOPTS) $(ATTRIBUTES) -Werror-warnings -c -c++ -o $@ `cygpath -w $<`

.asm${OPT}.doj:
	$(CC) $(ASMOPTS) $(CCOPTS) $(ATTRIBUTES) -c -o $@ `cygpath -w $<`

create_version:
	@echo "Updating the build number ================================="
	@echo "#ifndef ADI_OSAL_VDK_VERSION_H_"> adi_osal_vdk_version.h
	@echo "#define ADI_OSAL_VDK_VERSION_H_">> adi_osal_vdk_version.h
	@echo "">> adi_osal_vdk_version.h
	@echo "/* Format of the version is dddyy - the number of days (ddd) from the start of the year (yy) when this file was generated.  The 6-digit ddmmyy is not used as it cannot fit into a 16bit field, so the current format with a maximum of 36699 (366th day of 2099) is used as it will fit.  To get a more readable date form for a given osal build version, use date.exe in cygwin: 'date --date=\"20yy-01-01 ddd days -1 days\", e.g. 30010 'date --date=\"2010-01-01 300 days -1 days\"'*/">> adi_osal_vdk_version.h
	@echo "">> adi_osal_vdk_version.h
	@echo -n "#define ADI_OSAL_BUILD_VER  ">> adi_osal_vdk_version.h
	exec date  +%-j%yu >> adi_osal_vdk_version.h
	@echo "">> adi_osal_vdk_version.h
	@echo "#endif /* ADI_OSAL_VDK_VERSION_H_ */">> adi_osal_vdk_version.h
build_libs: ${OBJECTS} 
	@echo "Current folder: ${CURRENT_DIR}"
	$(CC) -build-lib -flags-lib -tx,version.txt -o $(LIBRARY)${OPT}.dlb $(OBJECTS)
	@echo "**** DONE ${LIBDIR}/$(LIBRARY)${OPT}.dlb build"
build: create_version build_libs
clean:
	rm -f $(OBJECTS) $(LIBRARY)${OPT}.dlb
install:
	@echo "Current folder: ${CURRENT_DIR}"
	@mkdir -p $(INSTALL_DIR)
	cp $(LIBRARY)${OPT}.dlb $(INSTALL_DIR)
	@echo "*** DONE ${LIBDIR}/$(LIBRARY)${OPT}.dlb install ***"
