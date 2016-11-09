
LINK_LIBS += -L$(TOP_DIR)/lib/libuv/.libs/ -luv

##define your subdir_y subdir_m and obj_y

subdir_y += 

subdir_m +=

obj_y += uviot_module_test.o
obj_y += uviot_module_test2.o
obj_y += uvco_task_test.o
obj_y += uvco_stream_test.o

include $(TOP_DIR)/MakeRule

$(TARGET):$(BUILDIN_OBJ)
	$(Q)$(CC) $(LINK_FLAGS) $(LINK_LIBS) $^  -o $@
	@echo "Compile    " $^  -o $@