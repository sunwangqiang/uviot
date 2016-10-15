
LINK_LIBS += -L$(TOP_DIR)/lib/libuv/.libs/ -luv

##define your subdir_y subdir_m and obj_y

subdir_y += 

subdir_m +=

obj_y += uv_idle.o

TARGET = uv_idle

include $(TOP_DIR)/MakeRule

$(TARGET):$(BUILDIN_OBJ)
	$(Q)$(CC) $(LINK_FLAGS) $(LINK_LIBS) $^  -o $@
	@echo "Compile    " $^  -o $@