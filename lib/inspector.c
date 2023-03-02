// not the safest code
#include <libxl/libxl.h>
#include <stdio.h>
#include <stdlib.h>

#include "inspector.h"
#include "util.h"

static const short MIN_ROW = 1;
static const short MIN_COL = 0;

static const short SEPARATOR_N_COLS = 1;
static const short ARR_INSPECTOR_N_COLS = 3;
static const short VAR_INSPECTOR_N_COLS = 2;
static const char FONT_NAME[] = "Arial";
static const short FONT_SIZE = 12;

static void inspector_create_formats(inspector_t *inspector) {

  FormatHandle cell_format, text_format, title_format, arr_index_format;
  FontHandle font;

  font = xlBookAddFont(inspector->wb, 0);
  xlFontSetName(font, FONT_NAME);
  xlFontSetSize(font, FONT_SIZE);

  cell_format = xlBookAddFormat(inspector->wb, NULL);
  xlFormatSetAlignH(cell_format, ALIGNH_CENTER);
  xlFormatSetBorder(cell_format, BORDERSTYLE_MEDIUM);
  xlFormatSetBorderColor(cell_format, COLOR_BLACK);
  xlFormatSetFont(cell_format, font);

  text_format = xlBookAddFormat(inspector->wb, NULL);
  xlFormatSetAlignH(text_format, ALIGNH_CENTER);
  xlFormatSetBorder(text_format, BORDERSTYLE_MEDIUM);
  xlFormatSetBorderColor(text_format, COLOR_BLACK);
  xlFormatSetFont(text_format, font);
  xlFormatSetPatternForegroundColor(text_format, COLOR_YELLOW);

  arr_index_format = xlBookAddFormat(inspector->wb, NULL);
  xlFormatSetAlignH(arr_index_format, ALIGNH_CENTER);
  xlFormatSetAlignV(arr_index_format, ALIGNV_CENTER);
  xlFormatSetBorder(arr_index_format, BORDERSTYLE_MEDIUM);
  xlFormatSetBorderColor(arr_index_format, COLOR_GREEN);
  xlFormatSetFont(arr_index_format, font);
  xlFormatSetPatternForegroundColor(arr_index_format, COLOR_GREEN);

  title_format = xlBookAddFormat(inspector->wb, NULL);
  xlFormatSetAlignH(title_format, ALIGNH_CENTER);
  xlFormatSetBorder(title_format, BORDERSTYLE_MEDIUM);
  xlFormatSetBorderColor(title_format, COLOR_BLACK);
  xlFormatSetFont(title_format, font);
  xlFormatSetFillPattern(title_format, FILLPATTERN_SOLID);
  xlFormatSetPatternForegroundColor(title_format, COLOR_YELLOW);

  inspector->cell_format = cell_format;
  inspector->text_format = text_format;
  inspector->title_format = title_format;
  inspector->arr_index_format = arr_index_format;
}

static void write_byte(inspector_t *inspector, unsigned char *byte) {
  char *str;

  str = build_str("%p", byte);
  xlSheetWriteStr(inspector->sheet, inspector->row, inspector->col, str,
                  inspector->text_format);
  free(str);

  str = build_str("0x%02X", *byte);
  xlSheetWriteStr(inspector->sheet, inspector->row, inspector->col + 1, str,
                  inspector->cell_format);
  free(str);
}

static void write_arr_byte(inspector_t *inspector, unsigned char *byte,
                           const char *name, int index) {
  char *str;

  str = build_str("%p", byte);
  xlSheetWriteStr(inspector->sheet, inspector->row, inspector->col, str,
                  inspector->text_format);
  free(str);

  str = build_str("0x%02X", *byte);
  xlSheetWriteStr(inspector->sheet, inspector->row, inspector->col + 1, str,
                  inspector->cell_format);
  free(str);
}

static void resize_cols(inspector_t *inspector) {
  xlSheetSetCol(inspector->sheet, 0, inspector->col, -1, NULL, 0);
}

static void write_inspect_header(inspector_t *inspector, char *str, size_t n) {
  xlSheetWriteStr(inspector->sheet, inspector->row++, inspector->col, str,
                  inspector->title_format);
  xlSheetSetMerge(inspector->sheet, inspector->row - 1, inspector->row - 1,
                  inspector->col, inspector->col + n);
}

static void write_arr_index_name(inspector_t *inspector, const char *name,
                                 const int arr_size, const int type_size,
                                 const int byte_count, const int i) {
  xlSheetSetMerge(inspector->sheet, inspector->row,
                  inspector->row - 1 + byte_count / arr_size,
                  inspector->col + 2, inspector->col + 2);

  for (size_t j = inspector->row;
       j <= (inspector->row - 1 + byte_count / arr_size); j++) {
    xlSheetWriteBlank(inspector->sheet, j, inspector->col + 2,
                      inspector->arr_index_format);
  }
  char *str = build_str("%s[%d]", name, (i - 1) / type_size);
  xlSheetWriteStr(inspector->sheet, inspector->row, inspector->col + 2, str,
                  inspector->arr_index_format);
  free(str);
}

inspector_t *inspector_create(void) {

  BookHandle book = xlCreateBook();
  if (book == NULL) {
    return NULL;
  }

  SheetHandle sheet = xlBookAddSheet(book, "Inspector result", 0);
  if (sheet == NULL) {
    return NULL;
  }

  inspector_t *inspector = (inspector_t *)malloc(sizeof(inspector_t));
  inspector->wb = book;
  inspector->sheet = sheet;
  inspector->row = MIN_ROW;
  inspector->col = MIN_COL;
  inspector_create_formats(inspector);

  return inspector;
}

int inspector_save(inspector_t *inspector, const char *filename) {

  if (inspector == NULL)
    return -1;

  int r = xlBookSave(inspector->wb, filename);

  return r;
}

int inspector_destroy(inspector_t *inspector) {
  if (inspector == NULL)
    return -1;

  free(inspector);
  return 0;
}

void _inspect(inspector_t *inspector, const char *name, const int size,
              unsigned char *start) {

  char *str = build_str("%s", get_rid_of_pointer(name));
  write_inspect_header(inspector, str, 1);
  free(str);

  for (size_t i = size; i > 0; i--) {
    unsigned char *vptr = start + (i - 1);
    write_byte(inspector, vptr);
    inspector->row++;
  }

  resize_cols(inspector);

  inspector->col += VAR_INSPECTOR_N_COLS + SEPARATOR_N_COLS;
  inspector->row = MIN_ROW;
}

void _inspect_arr(inspector_t *inspector, const char *name, const int arr_size,
                  const int type_size, unsigned char *start) {

  char *str;
  str = build_str("%s[%d]", get_rid_of_pointer(name), arr_size);
  write_inspect_header(inspector, str, 2);
  free(str);

  const size_t byte_count = arr_size * type_size;
  for (size_t i = byte_count; i > 0; i--) {
    if (!(i % type_size))
      write_arr_index_name(inspector, name, arr_size, type_size, byte_count, i);

    write_arr_byte(inspector, start + (i - 1), name, (i - 1) / type_size);
    inspector->row++;
  }

  resize_cols(inspector);

  inspector->col += ARR_INSPECTOR_N_COLS + SEPARATOR_N_COLS;
  inspector->row = MIN_ROW;
}
