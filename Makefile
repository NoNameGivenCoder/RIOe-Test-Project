#-------------------------------------------------------------------------------
.SUFFIXES:
#-------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)

#-------------------------------------------------------------------------------
# APP_NAME sets the long name of the application
# APP_SHORTNAME sets the short name of the application
# APP_AUTHOR sets the author of the application
#-------------------------------------------------------------------------------
#APP_NAME	:= Application Name
#APP_SHORTNAME	:= App Name
#APP_AUTHOR	:= Built with devkitPPC & wut

include $(DEVKITPRO)/wut/share/wut_rules

#-------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# CONTENT is the path to the bundled folder that will be mounted as /vol/content/
# ICON is the game icon, leave blank to use default rule
# TV_SPLASH is the image displayed during bootup on the TV, leave blank to use default rule
# DRC_SPLASH is the image displayed during bootup on the DRC, leave blank to use default rule
#-------------------------------------------------------------------------------
TARGET		:=	helloworld_cpp
BUILD		:=	build
SOURCES		:=	lib/rio/src \
				lib/rio/src/audio \
				lib/rio/src/container \
				lib/rio/src/controller \
				lib/rio/src/controller/cafe \
				lib/rio/src/filedevice \
				lib/rio/src/filedevice/cafe \
				lib/rio/src/gfx \
				lib/rio/src/gfx/cafe \
				lib/rio/src/gfx/lyr \
				lib/rio/src/gfx/mdl \
				lib/rio/src/gfx/mdl/res \
				lib/rio/src/gpu \
				lib/rio/src/gpu/cafe \
				lib/rio/src/math \
				lib/rio/src/math/impl \
				lib/rio/src/task \
				src \
				lib/rio-e/src/rio-e \
				lib/rio-e/src/rio-e/Types \
				lib/rio-e/src/rio-e/Properties \
				lib/yaml-cpp/src \
DATA		:=	data
INCLUDES	:=	lib/rio/include \
				include \
			lib/rio-e/include \
			lib/yaml-cpp/include \
			lib/tinygltf \
CONTENT		:=	build/fs/content
ICON		:=
TV_SPLASH	:=
DRC_SPLASH	:=

#-------------------------------------------------------------------------------
# options for code generation
#-------------------------------------------------------------------------------
CFLAGS	:=	-Wall -Wmissing-include-dirs -Wundef -Wredundant-decls -Wcast-align \
			-Og -g \
			-ffunction-sections -fdata-sections \
			$(MACHDEP)

CFLAGS	+=	$(INCLUDE) -D__WIIU__ -D__WUT__ -D__powerpc__ \
			-DRIO_DEBUG \
			-DRIO_CAFE_MAIN_FILE_DEVICE_AS_CONTENT

CXXFLAGS	:= $(CFLAGS) -std=c++17

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-g $(ARCH) $(RPXSPECS) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lwut

#-------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level
# containing include and lib
#-------------------------------------------------------------------------------
LIBDIRS	:= $(WUT_ROOT) $(PORTLIBS)

#-------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#-------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#-------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/build/fs/code/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#-------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#-------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#-------------------------------------------------------------------------------
	export LD	:=	$(CC)
#-------------------------------------------------------------------------------
else
#-------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifneq (,$(strip $(CONTENT)))
	export APP_CONTENT := $(TOPDIR)/$(CONTENT)
endif

ifneq (,$(strip $(ICON)))
	export APP_ICON := $(TOPDIR)/$(ICON)
else ifneq (,$(wildcard $(TOPDIR)/$(TARGET).png))
	export APP_ICON := $(TOPDIR)/$(TARGET).png
else ifneq (,$(wildcard $(TOPDIR)/icon.png))
	export APP_ICON := $(TOPDIR)/icon.png
endif

ifneq (,$(strip $(TV_SPLASH)))
	export APP_TV_SPLASH := $(TOPDIR)/$(TV_SPLASH)
else ifneq (,$(wildcard $(TOPDIR)/tv-splash.png))
	export APP_TV_SPLASH := $(TOPDIR)/tv-splash.png
else ifneq (,$(wildcard $(TOPDIR)/splash.png))
	export APP_TV_SPLASH := $(TOPDIR)/splash.png
endif

ifneq (,$(strip $(DRC_SPLASH)))
	export APP_DRC_SPLASH := $(TOPDIR)/$(DRC_SPLASH)
else ifneq (,$(wildcard $(TOPDIR)/drc-splash.png))
	export APP_DRC_SPLASH := $(TOPDIR)/drc-splash.png
else ifneq (,$(wildcard $(TOPDIR)/splash.png))
	export APP_DRC_SPLASH := $(TOPDIR)/splash.png
endif

.PHONY: $(BUILD) clean all

#-------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#-------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).wuhb $(TARGET).rpx $(OUTPUT).elf

#-------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#-------------------------------------------------------------------------------
# main targets
#-------------------------------------------------------------------------------
all	:	$(OUTPUT).wuhb

$(OUTPUT).wuhb	:	$(OUTPUT).rpx
$(OUTPUT).rpx	:	$(OUTPUT).elf
$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#-------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#-------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#-------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------