#include "const.h"
#include "ctx.h"
#include "dict.h"
#include "value.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int32_t srt_task_data_set_int64(const srt_context *ctx, const char *key,
                                int64_t value) {
  srt_value *v = calloc(1, sizeof(*v));

  if (!v) {
    return SRT_UNKNOWN_ERROR;
  }

  v->tag = SRT_INT64;
  v->int64 = value;

  if (srt_dict_set(ctx->task_data, key, v)) {
    if (ctx->verbose) {
      printf("set task_data var '%s: int64 = %ld'\n", key, value);
    }

    return SRT_SUCCESS;
  }

  return SRT_UNKNOWN_ERROR;
}

int32_t srt_task_data_get_int64(const srt_context *ctx, const char *key,
                                int64_t *value) {
  const srt_value *v = srt_dict_get(ctx->task_data, key);

  if (!v) {
    return SRT_UNKNOWN_KEY;
  }

  if (v->tag != SRT_INT64) {
    return SRT_KEY_TYPE_MISMATCH;
  }

  *value = v->int64;

  if (ctx->verbose) {
    printf("get task_data var '%s: int64 = %ld'\n", key, *value);
  }

  return SRT_SUCCESS;
}

int32_t srt_task_data_delete(const srt_context *ctx, const char *key) {
  if (srt_dict_delete(ctx->task_data, key)) {
    if (ctx->verbose) {
      printf("delete task_data var '%s'\n", key);
    }

    return SRT_SUCCESS;
  }

  return SRT_UNKNOWN_KEY;
}
