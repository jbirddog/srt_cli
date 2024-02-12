#include "value.h"
#include <stdio.h>
#include <stdlib.h>

#define NEW(t, f)                                                              \
  do {                                                                         \
    srt_value *v = calloc(1, sizeof(*v));                                      \
    if (!v) {                                                                  \
      return NULL;                                                             \
    }                                                                          \
    v->tag = t;                                                                \
    v->f = value;                                                              \
    return v;                                                                  \
  } while (0)

srt_value *srt_value_new_bool(bool value) { NEW(SRT_BOOL, b); }
srt_value *srt_value_new_dict(srt_dict *value) { NEW(SRT_DICT, dict); }
srt_value *srt_value_new_int64(int64_t value) { NEW(SRT_INT64, int64); }
srt_value *srt_value_new_str(char *value) { NEW(SRT_STR, str); }

void srt_value_free(srt_value *value) { free(value); }

void srt_value_print(srt_value *value) {
  if (!value) {
    printf("<NULL>\n");
    return;
  }

  switch (value->tag) {
  case SRT_BOOL:
    printf("bool = %s", value->b ? "true" : "false");
    break;
  case SRT_DICT:
    printf("dict = %p", (void *)value->dict);
    break;
  case SRT_INT64:
    printf("int64 = %ld", value->int64);
    break;
  case SRT_STR:
    printf("str = %s", value->str);
    break;
  }
}
