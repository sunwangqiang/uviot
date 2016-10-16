
TOP_DIR  = $(shell pwd)
RELEASE_DIR = $(TOP_DIR)/release
BUILDIN_OBJ = buildin.o

$(shell [ -d $(RELEASE_DIR) ] || mkdir -p $(RELEASE_DIR))
export TOP_DIR RELEASE_DIR BUILDIN_OBJ

# If V equals 0 then the above command will be hidden.
# If V equals 1 then the above command is displayed.
V = 1
ifeq ($(V),1)
  Q =
else
  Q = @
endif
MAKE = make
export Q MAKE

# Toolchain and flags cfg
CROSS_COMPILE = 

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
export AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP

HOSTCC       = gcc
HOSTCXX      = g++
HOSTCFLAGS   = -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer 
HOSTCXXFLAGS = -O2
ifeq ($(shell $(HOSTCC) -v 2>&1 | grep -c "clang version"), 1)
HOSTCFLAGS  += -Wno-unused-value -Wno-unused-parameter \
		-Wno-missing-field-initializers -fno-delete-null-pointer-checks
endif
export HOSTCC HOSTCXX HOSTCXXFLAGS

BUILD_CFLAGS   := -Wall -Werror -Wundef -Wstrict-prototypes -Wno-trigraphs \
		   -fno-strict-aliasing -fno-common \
		   -Werror-implicit-function-declaration \
		   -Wno-format-security 

INCLUDE_DIR    := \
		-I$(TOP_DIR)/include \
		-I$(TOP_DIR)/lib/libuv/include

BUILD_CFLAGS += $(INCLUDE_DIR)

LINK_LIBS += -L$(TOP_DIR)/lib/libuv/.libs/ -luv
# -w disable MAC OS X PIE warning
LINK_FLAGS +=  -w

export BUILD_CFLAGS INCLUDE_DIR LINK_FLAGS

##define subdir_y subdir_m  obj_y and TARGET
## lib目录下既有编译到可执行文件的库，又有独立的库
## 先通过subdir_m优先进行编译，然后通过subdir_y_obj链接其生成的buildin.o
# subdir_y += lib

subdir_y += core

subdir_m += lib
subdir_y += test 

subdir_y_obj += lib/$(BUILDIN_OBJ)

TARGET = uviot

include $(TOP_DIR)/MakeRule

$(TARGET):$(BUILDIN_OBJ)
	$(Q)$(CC) $(LINK_FLAGS) $(LINK_LIBS) $^  -o $@
	@echo "Compile    " $^  -o $@
