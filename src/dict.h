#include <stdbool.h>
#include <stddef.h>

typedef struct srt_value srt_value;

typedef struct srt_dict_item {
  char *key;
  srt_value *value;
  bool live;
} srt_dict_item;

typedef struct srt_dict {
  size_t cap;
  size_t len;
  size_t mask;
  srt_dict_item *items;
} srt_dict;

srt_dict *srt_dict_new(size_t capacity);

void srt_dict_free(srt_dict *dict);

srt_value *srt_dict_get(const srt_dict *dict, const char *key);

bool srt_dict_set(srt_dict *dict, const char *key, srt_value *value);

bool srt_dict_delete(const srt_dict *dict, const char *key);
