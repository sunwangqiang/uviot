##define your subdir_y subdir_m and obj_y

LINK_LIBS = -L$(TOP_DIR)/lib/$(LIBUVCO)/ -luvco

TARGET = uviot

subdir_y +=
subdir_m +=

obj_y += uviot.o

include $(TOP_DIR)/MakeRule

$(TARGET):$(BUILDIN_OBJ)
	$(Q)$(CC) $(LINK_FLAGS) $(LINK_LIBS) $^  -o $@
	@echo "Compile    " $^  -o $@