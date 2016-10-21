# 变更历史
# 模块简介
* uviot_module模块负责构建和维护uviot应用中的子模块


# API说明
# 调试诊断
# 数据结构说明
```clang
typedef struct uviot_module{
    char name[UVIOT_MODULE_NAME_SIZE];
	struct hlist_node hlist;
	struct hlist_head ev_head[UVIOT_EVENT_SLOT_SIZE];
}UVIOT_MODULE;
```