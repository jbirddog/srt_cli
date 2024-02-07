#include <stdbool.h>

typedef struct srt_dict srt_dict;

typedef struct srt_context {
  bool verbose;
  srt_dict *task_data;
} srt_context;

srt_context *srt_ctx_new(bool verbose);

void srt_ctx_free(srt_context *ctx);

bool srt_ctx_verbose(const srt_context *ctx);
