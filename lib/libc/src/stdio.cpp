#include "stdio.h"

#include <stdint.h>

namespace {

uint32_t num_chars(uint64_t num, uint8_t base) {
  uint32_t width = 0;
  while (num > 0) {
    num /= base;
    width++;
  }
  return width;
}

int sprint_base(char *str, uint64_t num, uint32_t base) {
  uint32_t width = num_chars(num, base);
  if (width == 0) {
    *str = '0';
    return 1;
  }

  uint64_t place = 1;
  while (num > 0) {
    // FIXME: We seem to have an issue with loading globals.
    const char *kHexChars = "0123456789abcdef";
    str[width - place] = kHexChars[num % base];
    place++;
    num /= base;
  }

  return width;
}

}  // namespace

int sprintf(char *str, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  int ret = vsprintf(str, format, arg);
  va_end(arg);
  return ret;
}

int vsprintf(char *str, const char *format, va_list arg) {
  uint32_t chars = 0;
  while (*format != '\0') {
    if (*format != '%') {
      *str = *format;
      chars++;
      str++;
      format++;
      continue;
    }
    format++;
    switch (*format) {
      case '%':
        *(str++) = *(format++);
        chars++;
        break;
      case 'l': {
        switch (*(++format)) {
          case 'x': {
            int width = sprint_base(str, va_arg(arg, uint64_t), 16);
            if (width == -1) {
              return -1;
            }
            chars += width;
            str += width;
            format++;
            break;
          }
          case 'u': {
            int width = sprint_base(str, va_arg(arg, uint64_t), 10);
            if (width == -1) {
              return -1;
            }
            chars += width;
            str += width;
            format++;
            break;
          }
        }
        break;
      }

      case 'x': {
        int width = sprint_base(str, va_arg(arg, uint32_t), 16);
        if (width == -1) {
          return -1;
        }
        chars += width;
        str += width;
        format++;
        break;
      }
      case 'u': {
        int width = sprint_base(str, va_arg(arg, uint32_t), 10);
        if (width == -1) {
          return -1;
        }
        chars += width;
        str += width;
        format++;
        break;
      }
      default:
        *(str++) = *(format++);
        chars++;
    }
  }
  *str = '\0';
  chars++;

  return chars;
}
