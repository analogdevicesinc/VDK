include variables.mk
.SUFFIXES:
.SUFFIXES: .c .s .asm .cpp ${OPT}.doj

CURRENT_DIR = ${shell pwd}

ifneq (${REMOVE_COMMON_OBJS},YES)
COMMON_OBJECTS=MasterLocking${OPT}.doj DpcQueue${OPT}.doj Thread${OPT}.doj Globals${OPT}.doj ThreadList${OPT}.doj Inlines${OPT}.doj Scheduler${OPT}.doj Locking${OPT}.doj 

# AOS requires a special ready queue file, and omits the timequeue which is imlemented with AOS itself

ifeq (${OS}, AOS)
CUSTOM_OBJECTS = AOS_ReadyQueue${OPT}.doj
else
CUSTOM_OBJECTS = ReadyQueue${OPT}.doj TimeQueue${OPT}.doj
endif

ifeq (${FAMILY},blackfin)
PLATFORM_OBJECTS = Switch_BF${OPT}.doj QueueDpc_BF${OPT}.doj Timer_BF${OPT}.doj
endif
ifeq (${FAMILY},sharc)
PLATFORM_OBJECTS = Switch_SH${OPT}.doj QueueDpc_SH${OPT}.doj Timer_SH${OPT}.doj
ASMOPTS+=-flags-asm -I,'$(shell cygpath -w ../../..)'
endif
endif

ifeq (${BUILD_TYPE},ROM)
ROM_OBJECTS = ROM_asm_definitions${OPT}.doj  ROM_C_definitions${OPT}.doj
endif

OBJECTS = ${COMMON_OBJECTS} ${CUSTOM_OBJECTS} ${PLATFORM_OBJECTS} ${ROM_OBJECTS}
.c${OPT}.doj:
	$(CC) $(NO_ASMOPTS) $(CCOPTS) $(ATTRIBUTES) -c -o $@ `cygpath -w $<`

.cpp${OPT}.doj:
	$(CC) $(NO_ASMOPTS) $(CPPOPTS)  $(ATTRIBUTES) -c -o $@ `cygpath -w $<`

.asm${OPT}.doj:
	$(CC) $(ASMOPTS) $(CCOPTS) $(ATTRIBUTES) -c -o $@ `cygpath -w $<`

build: ${OBJECTS} 
	@echo "Current folder: ${CURRENT_DIR}"
	$(CC) -build-lib -flags-lib -tx,version.txt -o $(LIBRARY)${OPT}.dlb $(OBJECTS)
	@echo "**** DONE ${LIBDIR}/$(LIBRARY)${OPT}.dlb build"
clean:
	rm -f $(OBJECTS) $(LIBRARY)${OPT}.dlb
install:
	@mkdir -p $(INSTALL_DIR)
	@echo "Current folder: ${CURRENT_DIR}"
	cp $(LIBRARY)${OPT}.dlb $(INSTALL_DIR)
	@echo "*** DONE ${LIBDIR}/$(LIBRARY)${OPT}.dlb install ***"
