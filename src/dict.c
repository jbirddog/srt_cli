#include "dict.h"
#include "value.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROBE(f, b)                                                            \
  do {                                                                         \
    const uint64_t hash = hash_str(key);                                       \
    const size_t slot = hash & dict->mask;                                     \
    size_t i = slot;                                                           \
    do {                                                                       \
      srt_dict_item *item = &dict->items[i];                                   \
      b;                                                                       \
      i = (i + 1) & dict->mask;                                                \
      if (i == slot) {                                                         \
        return f;                                                              \
      }                                                                        \
    } while (1);                                                               \
  } while (0)

srt_dict *srt_dict_new(size_t capacity) {
  if (capacity == 0 || ((capacity & (capacity - 1)) != 0)) {
    return NULL;
  }

  srt_dict *dict = calloc(1, sizeof(*dict));
  if (!dict) {
    return NULL;
  }

  srt_dict_item *items = calloc(capacity, sizeof(*items));
  if (!items) {
    free(dict);
    return NULL;
  }

  dict->cap = capacity;
  dict->mask = capacity - 1;
  dict->items = items;

  return dict;
}

static void srt_dict_item_free(srt_dict_item *item) {
  free(item->key);
  srt_value_free(item->value);

  item->key = NULL;
  item->value = NULL;
  item->live = false;
}

void srt_dict_free(srt_dict *dict) {
  if (!dict) {
    return;
  }

  for (int i = 0; i < dict->cap; ++i) {
    srt_dict_item_free(&dict->items[i]);
  }

  free(dict->items);
  free(dict);
}

static uint64_t hash_str(const char *str) {
  const uint64_t prime = 0x100000001b3;
  uint64_t hash = 0xcbf29ce484222325;
  unsigned char *bytes = (unsigned char *)str;

  while (*bytes++) {
    hash ^= (uint64_t)*bytes;
    hash *= prime;
  }

  return hash;
}

srt_value *srt_dict_get(const srt_dict *dict, const char *key) {
  PROBE(NULL, {
    if (!item->key) {
      return NULL;
    }

    if (strcmp(item->key, key) == 0) {
      if (item->live) {
        return item->value;
      }

      return NULL;
    }
  });
}

static bool set(srt_dict_item *item, const char *key, srt_value *value) {
  if (!(item->key = strdup(key))) {
    return false;
  }

  item->value = value;
  item->live = true;

  return true;
}

bool srt_dict_set(srt_dict *dict, const char *key, srt_value *value) {
  PROBE(false, {
    if (!item->key) {
      return set(item, key, value);
    }

    if (!item->live || strcmp(item->key, key) == 0) {
      srt_dict_item_free(item);
      return set(item, key, value);
    }
  });
}

bool srt_dict_delete(const srt_dict *dict, const char *key) {
  PROBE(false, {
    if (!item->key) {
      return false;
    }

    if (strcmp(item->key, key) == 0) {
      srt_dict_item_free(item);
      return true;
    }
  });
}
