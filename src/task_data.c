#include "const.h"
#include "ctx.h"
#include "dict.h"
#include "value.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PANIC_UNLESS(a, e, s)                                                  \
  if ((a) != (e)) {                                                            \
    fprintf(stderr, "Panic in %s: " s " '%s'\n", __func__, key);               \
    exit(a);                                                                   \
  }

#define GET_PANIC_UNLESS(a)                                                    \
  PANIC_UNLESS(a, SRT_SUCCESS, "failed to get task data var")

#define SET_PANIC_UNLESS(a)                                                    \
  PANIC_UNLESS(a, SRT_SUCCESS, "failed to set task data var")

#define LOG_K(m, k)                                                            \
  if (ctx->verbose) {                                                          \
    printf(m " '%s'\n", k);                                                    \
  }

#define LOG_KV(m, k, v)                                                        \
  if (ctx->verbose) {                                                          \
    printf(m " '%s: ", k);                                                     \
    srt_value_print(v);                                                        \
    printf("'\n");                                                             \
  }

static int32_t try_get_value(const srt_context *ctx, const char *key,
                             srt_value_tag tag, srt_value **value) {
  LOG_K("will get task_data var", key);

  srt_value *v = srt_dict_get(ctx->task_data, key);

  if (!v) {
    LOG_K("unknown task_data var", key);

    return SRT_UNKNOWN_KEY;
  }

  if (v->tag != tag) {
    LOG_K("type mismatch for task_data var", key);

    return SRT_KEY_TYPE_MISMATCH;
  }

  *value = v;

  LOG_KV("did get task_data var", key, v);

  return SRT_SUCCESS;
}

static int32_t try_set_value(const srt_context *ctx, const char *key,
                             srt_value *value) {

  LOG_KV("will set task_data var", key, value);

  if (value && srt_dict_set(ctx->task_data, key, value)) {
    LOG_KV("did set task_data var", key, value);

    return SRT_SUCCESS;
  }

  LOG_KV("failed to set task_data var", key, value);

  if (value) {
    srt_value_free(value);
  }

  return SRT_UNKNOWN_ERROR;
}

//
// bool
//

int32_t srt_task_data_try_get_bool(const srt_context *ctx, const char *key,
                                   bool *value) {
  srt_value *v;
  const uint32_t result = try_get_value(ctx, key, SRT_BOOL, &v);

  if (result == SRT_SUCCESS) {
    *value = v->b;
  }

  return result;
}

bool srt_task_data_get_bool(const srt_context *ctx, const char *key) {
  bool value;
  const int32_t result = srt_task_data_try_get_bool(ctx, key, &value);

  GET_PANIC_UNLESS(result);

  return value;
}

int32_t srt_task_data_try_set_bool(const srt_context *ctx, const char *key,
                                   bool value) {
  return try_set_value(ctx, key, srt_value_new(value));
}

void srt_task_data_set_bool(const srt_context *ctx, const char *key,
                            bool value) {
  SET_PANIC_UNLESS(srt_task_data_try_set_bool(ctx, key, value));
}

//
// dict
//

int32_t srt_task_data_try_get_dict(const srt_context *ctx, const char *key,
                                   srt_dict **value) {
  srt_value *v;
  const uint32_t result = try_get_value(ctx, key, SRT_DICT, &v);

  if (result == SRT_SUCCESS) {
    *value = v->dict;
  }

  return result;
}

srt_dict *srt_task_data_get_dict(const srt_context *ctx, const char *key) {
  srt_dict *value;
  const int32_t result = srt_task_data_try_get_dict(ctx, key, &value);

  GET_PANIC_UNLESS(result);

  return value;
}

int32_t srt_task_data_try_set_dict(const srt_context *ctx, const char *key,
                                   srt_dict *value) {
  return try_set_value(ctx, key, srt_value_new(value));
}

void srt_task_data_set_dict(const srt_context *ctx, const char *key,
                            srt_dict *value) {
  SET_PANIC_UNLESS(srt_task_data_try_set_dict(ctx, key, value));
}

//
// int64
//

int32_t srt_task_data_try_get_int64(const srt_context *ctx, const char *key,
                                    int64_t *value) {
  srt_value *v;
  const uint32_t result = try_get_value(ctx, key, SRT_INT64, &v);

  if (result == SRT_SUCCESS) {
    *value = v->int64;
  }

  return result;
}

int64_t srt_task_data_get_int64(const srt_context *ctx, const char *key) {
  int64_t value;
  const int32_t result = srt_task_data_try_get_int64(ctx, key, &value);

  GET_PANIC_UNLESS(result);

  return value;
}

int32_t srt_task_data_try_set_int64(const srt_context *ctx, const char *key,
                                    int64_t value) {
  return try_set_value(ctx, key, srt_value_new(value));
}

void srt_task_data_set_int64(const srt_context *ctx, const char *key,
                             int64_t value) {
  SET_PANIC_UNLESS(srt_task_data_try_set_int64(ctx, key, value));
}

//
// delete
//

int32_t srt_task_data_try_delete(const srt_context *ctx, const char *key) {
  if (srt_dict_delete(ctx->task_data, key)) {
    if (ctx->verbose) {
      printf("delete task_data var '%s'\n", key);
    }

    return SRT_SUCCESS;
  }

  return SRT_UNKNOWN_KEY;
}

void srt_task_data_delete(const srt_context *ctx, const char *key) {
  const int32_t result = srt_task_data_try_delete(ctx, key);

  PANIC_UNLESS(result, SRT_SUCCESS, "failed to delete task data var");
}
