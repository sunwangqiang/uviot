#ifndef __UVCO_DIAGNOSE_H
#define __UVCO_DIAGNOSE_H


typedef struct uvco_diag_item{
    char *path;
    int (*ls)(struct uvco_diag_item *);
    int (*cd)(struct uvco_diag_item *);
    int (*cat)(struct uvco_diag_item *);
}UVCO_DIAG_ITEM;

int uvco_diag_module_init(void);

#endif
