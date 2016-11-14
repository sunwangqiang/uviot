
##define your subdir_y subdir_m and obj_y

TARGET = uvco_test
LINK_LIBS = -L$(TOP_DIR)/lib/$(LIBUVCO)/ -luvco

subdir_y += 
subdir_m +=


obj_y += uviot_module_test.o
obj_y += uviot_module_test2.o
obj_y += uvco_task_test.o
obj_y += uvco_stream_test.o
obj_y += uvco_test_main.o

include $(TOP_DIR)/MakeRule

$(TARGET):$(BUILDIN_OBJ)
	$(Q)$(CC) $(LINK_FLAGS) $(LINK_LIBS) $^  -o $@
	@echo "Compile    " $^  -o $@


