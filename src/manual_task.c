#include "ctx.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int32_t srt_handle_manual_task(const srt_context *ctx, const char *element_id,
                               const char *instructions) {
  printf("Manual Task %s\n", element_id);

  if (instructions && *instructions != '\0') {
    printf("  * %s\n", instructions);
  }

  if (isatty(STDIN_FILENO)) {
    printf("Press enter to continue.\n");
    getchar();
  } else {
    printf(
        "Not in interactive mode, automatically completing manual task...\n");
  }

  return 0;
}
