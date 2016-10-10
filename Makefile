
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
HOSTCFLAGS   = -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer -std=gnu89
HOSTCXXFLAGS = -O2
ifeq ($(shell $(HOSTCC) -v 2>&1 | grep -c "clang version"), 1)
HOSTCFLAGS  += -Wno-unused-value -Wno-unused-parameter \
		-Wno-missing-field-initializers -fno-delete-null-pointer-checks
endif
export HOSTCC HOSTCXX HOSTCXXFLAGS

BUILD_CFLAGS   := -Wall -Werror -Wundef -Wstrict-prototypes -Wno-trigraphs \
		   -fno-strict-aliasing -fno-common \
		   -Werror-implicit-function-declaration \
		   -Wno-format-security \
		   -std=gnu89 

INCLUDE_DIR    := \
		-I$(TOP_DIR)/include

BUILD_CFLAGS += $(INCLUDE_DIR)
LINK_FLAGS +=

export BUILD_CFLAGS INCLUDE_DIR LINK_FLAGS


#compile process

##define subdir subdir_obj and subdir_clean
subdir_y += core
subdir_y += lib

subdir_obj = $(addsuffix /buildin.o,$(subdir_y))
subdir_clean := $(addprefix _clean_, $(subdir_y))

##define current_dir obj and src file
obj_y += hello.o
src_y = ${obj_y:%.o=%.c}


TARGET_OBJ = uviot

.PHONY: all $(subdir_y)
all:prepare $(TARGET_OBJ)

ifeq ($(strip $(src_y)),)
prepare:

prepare_clean:
	
else
prepare:
	$(Q)$(CC) -MM $(BUILD_CFLAGS) $(src_y)  > .depend

prepare_clean:
	$(Q)$(RM) -fr .depend
endif
	
-include .depend

# $(MAKE) $(JN) -C $@ || exit 1 ;
$(subdir_obj):$(subdir_y)
	
$(subdir_y):
	$(Q)$(MAKE)  -C $@ || exit 1 ;
	
$(TARGET_OBJ):$(obj_y) $(subdir_obj)
	$(Q)$(CC) $(LINK_FLAGS) $^  -o $@
	
.c.o:
	$(Q)$(CC) $(BUILD_CFLAGS) -c $<

clean: prepare_clean $(subdir_clean)
	$(Q)$(RM) -fr $(TARGET_OBJ) $(obj_y)

$(subdir_clean):
	$(Q)$(MAKE) -C $(patsubst _clean_%,%,$@) clean || exit 1 ;

	
