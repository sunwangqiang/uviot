##define your subdir_y subdir_m and obj_y
# shrext_cmds='`test .$module = .yes && echo .so || echo .dylib`'

TARGET = libuvco.so

LINK_LIBS +=  \
             -L$(TOP_DIR)/lib/$(LIBUV)/.libs/ -luv \
             -L$(TOP_DIR)/lib/$(JANSSON)/src/.libs/ -ljansson 
			 
subdir_y += task

obj_y +=
obj_y += uvco.o
obj_y += uvco_log.o
obj_y += uvco_section.o
obj_y += uvco_module.o
obj_y += uvco_node.o
obj_y += uvco_conn.o
obj_y += uvco_stream.o
obj_y += uvco_diag.o
obj_y += uvco_pipe.o

include $(TOP_DIR)/MakeRule

$(TARGET):$(BUILDIN_OBJ)
	$(Q)$(CC) $(LINK_FLAGS) $(LINK_LIBS) $^  -shared -o $@
	@echo "Compile    " $^  -o $@
