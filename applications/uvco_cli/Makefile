##define your subdir_y subdir_m and obj_y

TARGET = uvco_cli

LINK_LIBS = -L$(TOP_DIR)/lib/$(LIBUVCO)/ -luvco

subdir_y +=

subdir_m +=

obj_y += uvco_cli.o

include $(TOP_DIR)/MakeRule

$(TARGET):$(BUILDIN_OBJ)
	$(Q)$(CC) $(LINK_FLAGS) $(LINK_LIBS) $^  -o $@
	@echo "Compile    " $^  -o $@
