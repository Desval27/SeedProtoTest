TARGET = SeedProtoTest
#APP_TYPE = BOOT_QSPI

USE_DAISYSP_LGPL = 1
USE_DEBUG = 0
PLATFORM = DAISY_SEED

# Library Locations
LIBDAISY_DIR ?= ../../libDaisy
DAISYSP_DIR ?= ../../DaisySP

MONKEY_DAYSEYE_DIR = ..
MONKEY_DAYSEYE_SRC = $(MONKEY_DAYSEYE_DIR)/src
MONKEY_DAYSEYE_INC = $(MONKEY_DAYSEYE_DIR)/include
MONKEY_DAYSEYE_CPP_SOURCES = $(wildcard $(MONKEY_DAYSEYE_SRC)/*.cpp) 

APP_SRC = src
APP_INC = include
APP_CPP_SOURCES := $(wildcard $(APP_SRC)/*.cpp) 

# Sources
CPP_SOURCES = \
	$(APP_CPP_SOURCES) \
	$(MONKEY_DAYSEYE_CPP_SOURCES) \
	
C_DEFS += -DDAISY_PLATFORM -DPLATFORM=$(PLATFORM) -Wno-unused-variable -Wno-unused-function
#OPT = -Og 
OPT = -Og
C_INCLUDES += -I$(APP_INC) -I$(MONKEY_DAYSEYE_INC) 

.PHONY: garp


# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

show_size: # $(BUILD_DIR)/$(TARGET).elf
	@echo "Size of $(TARGET).elf:"
	@arm-none-eabi-nm --print-size --size-sort --reverse-sort $(BUILD_DIR)/$(TARGET).elf
	
garp:
	@echo $(CPP_SOURCES)

