#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <libxl/libxl.h>

#ifdef WIN32
#include "util.h"
#define __GET_VARIABLE_NAME(v) get_rid_of_pointer(#v)
#else
#define __GETRIDOFPOINTER(v) ({ \
    for (int i = 0;; i++) { \
        if (v[i] == '*' || v[i] == '&') { \
            v++; \
        } else if (v[i] == '\0') { \
            break; \
        } \
    } \
})
#define __GET_VARIABLE_NAME(v) ({char *str = #v; __GETRIDOFPOINTER(str); str;})
#endif

typedef struct
{
    BookHandle wb;
    SheetHandle sheet;
    FormatHandle title_format;
    FormatHandle cell_format;
    FormatHandle text_format;
    FormatHandle arr_index_format;

    unsigned int row;
    unsigned int col;
} inspector_t;

inspector_t *inspector_create(void);
int inspector_save(inspector_t *, const char *filename);
int inspector_destroy(inspector_t *);

void _inspect(inspector_t *, const char *name, const int type_size, unsigned char *start);
#define inspect(insp, value) _inspect(insp, __GET_VARIABLE_NAME(value), sizeof(*value), (unsigned char *)value)

void _inspect_arr(inspector_t *, const char *name, const int arr_size, const int type_size, unsigned char *start);
#define inspect_arr(insp, arr_size, value) _inspect_arr(insp, __GET_VARIABLE_NAME(value), arr_size, sizeof(*value), (unsigned char *)value);
#endif