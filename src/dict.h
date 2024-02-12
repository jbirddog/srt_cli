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

srt_dict *srt_dict_new(const size_t capacity);

srt_dict *srt_dict_new_with_kvs(const size_t kv_count, const char *key1,
                                srt_value *value1, ...);

void srt_dict_free(srt_dict *dict);

srt_value *srt_dict_get(const srt_dict *dict, const char *key);

bool srt_dict_set(srt_dict *dict, const char *key, srt_value *value);

bool srt_dict_delete(srt_dict *dict, const char *key);

size_t srt_dict_len(const srt_dict *dict);
