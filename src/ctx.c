#include "ctx.h"
#include "dict.h"
#include <stdlib.h>

srt_context *srt_ctx_new(bool verbose) {
  srt_context *ctx = calloc(1, sizeof(*ctx));
  if (!ctx) {
    return NULL;
  }

  ctx->verbose = verbose;

  ctx->task_data = srt_dict_new(64);
  if (!ctx->task_data) {
    free(ctx);
    return NULL;
  }

  return ctx;
}

void srt_ctx_free(srt_context *ctx) {
  srt_dict_free(ctx->task_data);
  free(ctx);
}

bool srt_ctx_verbose(const srt_context *ctx) { return ctx->verbose; }
