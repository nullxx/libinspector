#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

const char *get_rid_of_pointer(const char *v) {
  for (int i = 0;; i++) {

    if (v[i] == '*' || v[i] == '&') {

      v++;
    } else if (v[i] == '\0') {

      break;
    }
  }

  return v;
}

// if not def __GNUC__ or __clang__
#if !defined(__GNUC__) || defined(__clang__)
int vscprintf(const char *format, va_list ap) {
  va_list ap_copy;
  va_copy(ap_copy, ap);
  int retval = vsnprintf(NULL, 0, format, ap_copy);
  va_end(ap_copy);
  return retval;
}

int vasprintf(char **strp, const char *format, va_list ap) {
  int len = vscprintf(format, ap);
  if (len == -1)
    return -1;
  char *str = (char *)malloc((size_t)len + 1);
  if (!str)
    return -1;
  int retval = vsnprintf(str, len + 1, format, ap);
  if (retval == -1) {
    free(str);
    return -1;
  }
  *strp = str;
  return retval;
}
#endif

char *build_str(const char *const format, ...) {
  char *str;
  va_list args;
  va_start(args, format);
  vasprintf(&str, format, args);
  va_end(args);

  return str;
}