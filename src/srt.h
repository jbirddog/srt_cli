#include <stdbool.h>
#include <stddef.h>
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
typedef struct srt_dict srt_dict;
typedef struct srt_value srt_value;

/*
 * Context
 *
 */

srt_context *srt_ctx_new(bool verbose);

void srt_ctx_free(srt_context *ctx);

bool srt_ctx_verbose(const srt_context *ctx);

/*
 * Value
 *
 */

srt_value *srt_value_new_bool(bool value);
srt_value *srt_value_new_dict(srt_dict *value);
srt_value *srt_value_new_int64(int64_t value);
srt_value *srt_value_new_str(char *value);

void srt_value_free(srt_value *value);

void srt_value_print(srt_value *value);

/*
 * Dict
 *
 */

srt_dict *srt_dict_new(size_t capacity);

srt_dict *srt_dict_new_with_kvs(const size_t kv_count, const char *key1,
                                srt_value *value1, ...);

void srt_dict_free(srt_dict *dict);

srt_value *srt_dict_get(const srt_dict *dict, const char *key);

bool srt_dict_set(srt_dict *dict, const char *key, srt_value *value);

bool srt_dict_delete(srt_dict *dict, const char *key);

size_t srt_dict_len(const srt_dict *dict);

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

//
// these flavors attempt the operation and return an error code if unsuccessful.
//

int32_t srt_task_data_try_get_bool(const srt_context *ctx, const char *key,
                                   bool *value);

int32_t srt_task_data_try_set_bool(const srt_context *ctx, const char *key,
                                   bool value);

int32_t srt_task_data_try_get_dict(const srt_context *ctx, const char *key,
                                   srt_dict **value);

int32_t srt_task_data_try_set_dict(const srt_context *ctx, const char *key,
                                   srt_dict *value);

int32_t srt_task_data_try_get_int64(const srt_context *ctx, const char *key,
                                    int64_t *value);

int32_t srt_task_data_try_set_int64(const srt_context *ctx, const char *key,
                                    int64_t value);

int32_t srt_task_data_try_delete(const srt_context *ctx, const char *key);

//
// these flavors attempt the operation and panic if unsuccessful.
//

int64_t srt_task_data_get_bool(const srt_context *ctx, const char *key);

void srt_task_data_set_bool(const srt_context *ctx, const char *key,
                            int64_t value);

srt_dict *srt_task_data_get_dict(const srt_context *ctx, const char *key);

void srt_task_data_set_dict(const srt_context *ctx, const char *key,
                            srt_dict *value);

int64_t srt_task_data_get_int64(const srt_context *ctx, const char *key);

void srt_task_data_set_int64(const srt_context *ctx, const char *key,
                             int64_t value);

void srt_task_data_delete(const srt_context *ctx, const char *key);
