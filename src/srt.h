#include <stdbool.h>
#include <stdint.h>

/*
 * Constants
 *
 */

static const uint32_t SRT_SUCCESS = 0;
static const uint32_t SRT_UNKNOWN_KEY = 1;
static const uint32_t SRT_KEY_TYPE_MISMATCH = 2;
static const uint32_t SRT_UNKNOWN_ERROR = 3;

/*
 * Types
 *
 */

typedef struct srt_context srt_context;
typedef struct srt_value srt_value;

/*
 * Context
 *
 */

srt_context *srt_ctx_new(bool verbose);

void srt_ctx_free(srt_context *ctx);

bool srt_ctx_verbose(const srt_context *ctx);

/*
 * Life Cyle
 *
 */

int32_t srt_will_run_element(const srt_context *ctx, const char *process_id,
                             const char *element_id);

int32_t srt_did_run_element(const srt_context *ctx, const char *process_id,
                            const char *element_id);

/*
 * Task Handling
 *
 */

int32_t srt_handle_manual_task(const srt_context *ctx, const char *element_id,
                               const char *instructions);

/*
 * Task Data
 *
 */

int32_t srt_task_data_set_int64(const srt_context *ctx, const char *key,
                                int64_t value);

int32_t srt_task_data_get_int64(const srt_context *ctx, const char *key,
                                int64_t *value);

int32_t srt_task_data_delete(const srt_context *ctx, const char *key);
