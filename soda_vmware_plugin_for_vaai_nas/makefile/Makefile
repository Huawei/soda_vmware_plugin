
MODULE   = OpenSDSVAAINasPlugin
MODULE_VERSION = 550.0.0-201024
MODULE_VIB_BASENAME = OpenSDSVAAINasPlugin-2.1.24
MODULE_MF_SCRIPT=OpenSDSVAAINas.mf

CURRENT_DIR := $(shell pwd)
SOURCE = $(CURRENT_DIR)/..
BUILD_DIR := $(SOURCE)/build
STAGE_DIR := $(BUILD_DIR)/stage
SCRATCH_DIR := $(BUILD_DIR)/scratch
BUNDLE_DIR := $(BUILD_DIR)/bundle
# Should be $(BUILD_DIR)/vib, but too late for now.
VIB_DIR := $(BUILD_DIR)/vib

# Should be $(BUILD_DIR)/vib, but we're too late in the cycle to change it now.
VIB_DIR := $(BUILD_DIR)/vib
BUNDLE_DIR := $(BUILD_DIR)/bundle
SCRATCH_DIR := $(BUILD_DIR)/scratch

VIB_DESCRIPTOR_XML = $(CURRENT_DIR)/descriptor.xml
VIB_BULLETIN_XML = $(CURRENT_DIR)/bulletin.xml
VIB_BULLETIN_ID = VMW-ESX-$(DESC_ESX_VERSION)-$(MODULE)-$(MODULE_VERSION)

DESC_TYPE=bootbank
DESC_NAME=$(MODULE_VIB_BASENAME)
DESC_ESX_VERSION=5.5.0
DESC_VENDOR_EMAIL=ChinaEnterprise_TAC@OpenSDS.com
DESC_VERSION=$(MODULE_VERSION)
DESC_VENDOR=OpenSDS
DESC_SUMMARY=OpenSDS VAAI-NAS Plugin for ESXi 5.5/6.0/6.5/6.7
DESC_DESCRIPTION=OpenSDS VAAI-NAS Plugin for ESXi 5.5/6.0/6.5/6.7
DESC_RELEASE_DATE=2018-07-11
# The next five variables name files, each of which contains
# one or more tag+value phrases in legal XML snippets.  The
# exact details vary somewhat from file to file, so please
# see descriptor-template.xml in the vib-suite distribution
# for details.
DESC_URLS=
DESC_DEPENDS=
DESC_CONFLICTS=
DESC_REPLACES=
DESC_PROVIDES=
DESC_MAINT=0
DESC_LIVE_INSTALL_ALLOWED=false
DESC_LIVE_REMOVE_ALLOWED=false
DESC_STATELESS_READY=false

BULL_KBURL=https://www.opensds.io/
# The platforms file is not currently used.
# BULL_PLATFORMS=bulletin-platforms.xml
BULL_VENDOR_CODE=VMW

# Each element of ARCHPAIRS is of the form a=$(arch):b=$(bitwidth),
# where a and b are the obvious abbreviations.  As shown below, it
# is intended that an element can be split and eval'd to get the
# two values in separate variables.  This approach has the advantage
# of working with any variant of make and sh.
#
# ARCH is being set to prevent unplanned-for scenarios.  The only
# time its value should matter is in the recursive invocations
# below in the libs and package targets.

ARCH ?= bad_arch

ARCHPAIRS := a=x86:b=32 a=x64:b=64

ifeq ($(ARCH), x86)
  ARCHBITS := 32
else
  ARCHBITS := 64
endif

include /opt/vmware/nvdk/samples/common/nvdk.inc

TCENV += $(GLIBC_$(ARCHBITS)_SYSROOT)
CC := $(CC_$(ARCHBITS))

DEVKITHDRDIRS += -I$(NVDKROOT)/include
		DEVKITLIBDIRS += -L$(NVDKROOT)/lib32 -L$(NVDKROOT)/lib64

# Have to add -shared here, rather than in the common file, because
# it's not appropriate when creating executables, but is needed for
# shared objects/libraries (which is what we're doing in this Makefile).
# The common file is trying to be relatively generic.

VMW_UFLAGS += -fno-strict-aliasing
CCFLAGS += $(VMW_UFLAGS) $(VMW_$(OBJDIR)_FLAGS) $(TCENV) $(DEVKITHDRDIRS)
LDFLAGS += -shared $(LD_$(ARCHBITS)_OPTS_UW) $(TCENV) $(DEVKITLIBDIRS)

SHAREDOBJ := $(BUILD_DIR)/obj-$(ARCH)/libnvdkOpenSDSVAAINasPlugin.so
SRCS := exampleNas.c plugin.c mount_clnt.c mount_xdr.c nfs_prot_clnt.c \
	nfs_prot_xdr.c vaai_nas_clnt.c vaai_nas_xdr.c rpcb_prot_clnt.c rpcb_prot_xdr.c Common.c ExtendedStats.c \
	FileCloning.c FileSpaceReservation.c memcpy_s.c  snprintf_s.c strncpy_s.c memset_s.c  vsnprintf_s.c securecutil.c strcpy_s.c\
	secureinput_w.c secureprintoutput_a.c secureinput_a.c secureprintoutput_w.c \
	memmove_s.c  sprintf_s.c strtok_s.c  vsprintf_s.c

VPATH=$(SOURCE)/code
VPATH+=$(SOURCE)/secLib/src
THIRDPARTY_INCLS=-I$(SOURCE)/code
SECLIB_INCLS=-I$(SOURCE)/secLib/include

OBJS := $(SRCS:%.c=$(BUILD_DIR)/obj-$(ARCH)/%.o)

CCFLAGS += $(THIRDPARTY_INCLS)
CCFLAGS += $(SECLIB_INCLS)

all: 	vib
wb-xml-checks-package:
	make -s xml-checks >& /dev/null;\
	if [ $$? == 0 ]; then \
		make package; \
	fi


libs:
	for ap in $(ARCHPAIRS); do \
		eval `echo $$ap | tr : ' '`; \
		if [ ! -d $(BUILD_DIR)/obj-$${a} ]; then \
			$(MKDIR) -p $(BUILD_DIR)/obj-$${a}; \
		fi; \
		$(MAKE) ARCH=$${a} $(BUILD_DIR)/obj-$${a}/libnvdkOpenSDSVAAINasPlugin.so; \
	done

build-dirs:
	@for dir in $(BUILD_DIR) $(STAGE_DIR); do \
		$(MKDIR) -p $$dir; \
	done

all-workbench:	build-dirs libs wb-xml-checks-package
vib:	build-dirs libs minimal-descriptor fill-bulletin xml-checks package

package:
	$(RM) -rf $(STAGE_DIR)
	@echo "**  Building $(MODULE_VIB_BASENAME) vib"
	$(MKDIR) -p $(STAGE_DIR)/etc/vmware/init/init.d
	$(MKDIR) -p $(STAGE_DIR)/etc/vmware/init/manifests
	$(MKDIR) -p $(STAGE_DIR)/etc/vmware/firewall
	$(CP) $(SOURCE)/initscripts/OpenSDSVAAINasPlugin-firewall.xml $(STAGE_DIR)/etc/vmware/firewall
	for ap in $(ARCHPAIRS); do \
		eval `echo $$ap | tr : ' '`; \
		$(MKDIR) -p $(STAGE_DIR)/usr/lib/vmware/nas_plugins/lib$$b; \
		$(CP) $(BUILD_DIR)/obj-$$a/libnvdkOpenSDSVAAINasPlugin.so $(STAGE_DIR)/usr/lib/vmware/nas_plugins/lib$$b; \
	done
	@if [ -n "$(SIGNOPTS)" ];                      \
	then                                              \
	   echo "**  Signing $(MODULE_VIB_BASENAME).vib"; \
	fi;
	$(MAKE) -f Makefile vib-bundle-safe


include $(SOURCE)/initscripts/build-bundle.inc

clean:
	$(RM) -rf $(BUILD_DIR)

$(SHAREDOBJ): $(OBJS)
	$(CC) $(LDFLAGS) $(CCFLAGS) -o $@ $?

$(BUILD_DIR)/obj-$(ARCH)/%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $?
