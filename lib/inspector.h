#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <libxl/libxl.h>

#define __GET_VARIABLE_NAME(v) #v

typedef struct {
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

void _inspect(inspector_t *, const char *name, const int type_size,
              unsigned char *start);
/**
 * @brief Inspect a variable
 * @param *insp Inspector
 * @param *value Variable to inspect
 */
#define inspect(insp, value)                                                   \
  _inspect(insp, __GET_VARIABLE_NAME(value), sizeof(*value),                   \
           (unsigned char *)value)

void _inspect_arr(inspector_t *, const char *name, const int arr_size,
                  const int type_size, unsigned char *start);
/**
 * @brief Inspect an array
 * @param *insp Inspector
 * @param arr_size Size of the array
 * @param *value Array to inspect
 */
#define inspect_arr(insp, arr_size, value)                                     \
  _inspect_arr(insp, __GET_VARIABLE_NAME(value), arr_size, sizeof(*value),     \
               (unsigned char *)value);

void _inspect_bytes(inspector_t *, const char *name, const int size,
                    unsigned char *start);
/**
 * @brief Inspect memory as bytes
 * @param *insp Inspector
 * @param size Size of the memory to inspect
 * @param *value Memory to inspect
 */
#define inspect_bytes(insp, size, value)                                       \
  _inspect_bytes(insp, __GET_VARIABLE_NAME(value), size, (unsigned char *)value)
#endif