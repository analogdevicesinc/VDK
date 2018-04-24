# Top level Makefile for building VDK kernel and Platform libraries

CWD = $(shell pwd)
KERNEL_DIR = Kernel
KERNEL_FILE = TMK_Build.tcl
PLATFORM_DIR = Platform
VDSP_DIR = c:/analog/VisualDSP5.1.2
# Set ARCH to blackfin or SHARC
ARCH = SHARC
# Set PROC to the target processor, watch out for funny case
PROC = 21462 
# Set SI_REV to the revision you are targetting
SI_REV=0.1
# Set Core to be one of 10x(Blackfin), 2106X, 2116X, 2126X, 2136X, 2146X, 2147X, 2148X 
CORE=2146X
OUT_DIR = $(CWD)/out/$(ARCH)/$(PROC)
TCLCMD = tclsh8.6
VDK_BUILD_MODE = -target=release,install


PLATFORM_ACTIONS=release install

KERNEL_ARCH = $(ARCH)
LIB_ARCH = $(ARCH)
ifeq ($(ARCH),blackfin)
  LIB_ARCH = BLACKFIN
  KERNEL_ARCH = blackfin
endif
ifeq ($(ARCH),BLACKFIN)
  LIB_ARCH = BLACKFIN
  KERNEL_ARCH = BLACKFIN
endif

.PHONY: all
all: kernel libs

.PHONY: clean
clean: clean_kernel clean_libs

.PHONY: clean_kernel
clean_kernel: .never_up_to_date
	rm -rf $(KERNEL_DIR)/$(ARCH)/$(PROC)

.PHONY: kernel
kernel: TCL_FILE = $(KERNEL_FILE)
kernel: BUILD_DIR = $(KERNEL_DIR)
kernel: .never_up_to_date
	cd $(BUILD_DIR) && \
	$(TCLCMD) $(TCL_FILE) -device=$(PROC) -si-revision=$(SI_REV) \
	-vdsp_install="$(VDSP_DIR)" -install_dir=$(OUT_DIR) $(VDK_BUILD_MODE)

.PHONY: libs
libs: .never_up_to_date
libs: BUILD_DIR = $(PLATFORM_DIR)
libs:
	cp $(KERNEL_DIR)/LCK.h $(PLATFORM_DIR)/common
	cp $(KERNEL_DIR)/TMK.h $(PLATFORM_DIR)/common
	cp $(KERNEL_DIR)/ADI_attributes.h $(PLATFORM_DIR)/common
	-make -CPlatform/common -f makefile.mk $(PLATFORM_ACTIONS) \
	FAMILY=$(LIB_ARCH) CORE=$(CORE)  DEVICE=$(PROC) SI_REV=$(SI_REV) \
	LIB_VERISON="5.0.10" VDSP_VERSION=5.0 \
	TOP_DIR=$(CWD)/Platform \
	UP_INSTALL_DIR=$(OUT_DIR) NO_ASSERT_TMK=1

.never_up_to_date:
