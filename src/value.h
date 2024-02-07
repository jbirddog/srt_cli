#include <stdint.h>

typedef struct srt_value {
  enum { SRT_INT64 } tag;
  union {
    int64_t int64;
  };
} srt_value;

void srt_value_free(srt_value *value);
