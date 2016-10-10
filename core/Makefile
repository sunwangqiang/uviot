#compile process

##define subdir subdir_obj and subdir_clean
subdir_y += 
subdir_obj = $(addsuffix /buildin.o,$(subdir_y))
subdir_clean := $(addsuffix _clean_, $(subdir_y))

##define current_dir obj and src file
obj_y += hello.o
src_y = ${obj_y:%.o=%.c}

TARGET_OBJ = $(BUILDIN_OBJ)
.PHONY: all $(subdir_y)
all:prepare $(TARGET_OBJ)

prepare:
	$(Q)$(CC) -MM $(BUILD_CFLAGS) $(src_y)  > .depend
	
prepare_clean:
	-rm .depend
	
-include .depend

# $(MAKE) $(JN) -C $@ || exit 1 ;
$(subdir_obj):$(subdir_y)
	
$(subdir_y):
	$(MAKE)  -C $@ || exit 1 ;
	
$(TARGET_OBJ):$(obj_y) $(subdir_obj)
	$(Q)$(LD) -r $(LINK_FLAGS) -o $@ $^
	
.c.o:
	$(Q)$(CC) $(BUILD_CFLAGS) -c $<

clean: prepare_clean $(subdir_clean)
	-rm -f $(TARGET_OBJ) $(obj_y)

$(subdir_clean):
	$(MAKE) -C $(patsubst _clean_%,%,$@) clean || exit 1 ;
