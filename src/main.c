#include "ctx.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

int32_t spiff_process_start(srt_context *ctx);

int main(int argc, char *argv[]) {
  bool verbose = false;

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-v") == 0) {
      verbose = true;
      break;
    }
  }

  srt_context *ctx = srt_ctx_new(verbose);
  const int result = spiff_process_start(ctx);

  srt_ctx_free(ctx);

  return result;
}
