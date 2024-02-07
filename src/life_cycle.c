#include "ctx.h"
#include <stdint.h>
#include <stdio.h>

int32_t srt_will_run_element(const srt_context *ctx, const char *process_id,
                             const char *element_id) {
  if (srt_ctx_verbose(ctx)) {
    printf("will run %s_%s\n", process_id, element_id);
  }

  return 0;
}

int32_t srt_did_run_element(const srt_context *ctx, const char *process_id,
                            const char *element_id) {
  if (srt_ctx_verbose(ctx)) {
    printf("did run %s_%s\n", process_id, element_id);
  }

  return 0;
}
