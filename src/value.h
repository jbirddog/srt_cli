#include <stdbool.h>
#include <stdint.h>

typedef struct srt_dict srt_dict;

typedef enum srt_value_tag {
  SRT_BOOL,
  SRT_DICT,
  SRT_INT64,
  SRT_STR
} srt_value_tag;

typedef struct srt_value {
  srt_value_tag tag;
  union {
    bool b;
    srt_dict *dict;
    int64_t int64;
    char *str;
  };
} srt_value;

srt_value *srt_value_new_bool(bool value);
srt_value *srt_value_new_dict(srt_dict *value);
srt_value *srt_value_new_int64(int64_t value);
srt_value *srt_value_new_str(char *value);

#define srt_value_new(X)                                                       \
  _Generic((X), \
    bool: srt_value_new_bool, \
    srt_dict *: srt_value_new_dict, \
    int64_t: srt_value_new_int64, \
    char *: srt_value_new_str)(X)

void srt_value_free(srt_value *value);

void srt_value_print(srt_value *value);
