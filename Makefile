# -------------------------------------------------------------------
# Build tools definitions
# -------------------------------------------------------------------

CPP = $(PREFIX)g++
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
ECHO=echo

# -------------------------------------------------------------------
# Project path definitions
# -------------------------------------------------------------------

PROJECT_PATH=.
SRC_DIR = $(PROJECT_PATH)/src
TOOLS_DIR = $(SRC_DIR)/tools

ifeq ($(DEBUG), 1)
BUILD_DIR = $(PROJECT_PATH)/$(TARGET)_Debug
else
BUILD_DIR = $(PROJECT_PATH)/$(TARGET)_Release
endif

# -------------------------------------------------------------------
# Sorce files
# -------------------------------------------------------------------
ifeq ($(OS), Windows_NT)
TOOLS_PLATFORM_SRC = \
	$(TOOLS_DIR)/src/platform/tools_time_windows.c
else
TOOLS_PLATFORM_SRC = \
	$(TOOLS_DIR)/src/platform/tools_time_linux.c
endif

TOOLS_SOURCES = \
	$(TOOLS_PLATFORM_SRC) \
	$(TOOLS_DIR)/src/tools_debug.c \
	$(TOOLS_DIR)/src/tools_hexdump.c \
	$(TOOLS_DIR)/src/tools_print.c \
	$(TOOLS_DIR)/src/tools_string.c

C_SOURCES = \
	$(TOOLS_SOURCES) \
	$(SRC_DIR)/main.c


# -------------------------------------------------------------------
# Include files
# -------------------------------------------------------------------
C_INCLUDES = \
	-I/usr/lib/x86_64-linux-gnu/dbus-1.0/include \
	-I$(THIRD_PARTY_DIR)/cJSON \
	-I$(TOOLS_DIR)/inc \
	-I$(USBD_DIR)/inc \
	-I$(DEVICE_DIR)/inc \
	-I$(COMMON_DIR)/inc \
	-I$(SRC_DIR)

# -------------------------------------------------------------------
# C Defines
# -------------------------------------------------------------------
C_DEFS =
	-DTOOLS_OPT="tools_config.h"

# -------------------------------------------------------------------
# Build flags
# -------------------------------------------------------------------

ifeq ($(DEBUG), 1)
OPT = -O0 -g3 # -gdwarf-2
else
OPT = -Os
endif

CFLAGS = -std=gnu11 $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

# -------------------------------------------------------------------
# Linker flags
# -------------------------------------------------------------------

# Libraries
LIBS = \
	-lc \
	-lm

LIBDIR =

LDFLAGS = $(LIBDIR) $(LIBS)

# Objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))


# -------------------------------------------------------------------
# Build section
# -------------------------------------------------------------------
all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@$(ECHO) "compiling: "$@
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile
	@$(ECHO) "linking: "$@
	$(CPP) -o $@ $(OBJECTS) $(LDFLAGS)
	$(SZ) $@

$(BUILD_DIR):
	@$(ECHO) "create directory: "$@
	@mkdir $@

# -------------------------------------------------------------------
# Clean up
# -------------------------------------------------------------------
clean:
	@$(ECHO) "remove: "$@
	@rm -fRv $(BUILD_DIR)

# -------------------------------------------------------------------
# Dependencies
# -------------------------------------------------------------------
-include $(wildcard $(BUILD_DIR)/*.d)

# EOF

