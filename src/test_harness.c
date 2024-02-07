#include "srt.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define START_TESTS printf("%s...\n", __func__)

#define TEST(n, b)                                                             \
  do {                                                                         \
    printf("\t" n);                                                            \
    b;                                                                         \
    printf(" - Ok.\n");                                                        \
  } while (0)

#define TEST_WITH_CTX(n, b)                                                    \
  do {                                                                         \
    srt_context *ctx = srt_ctx_new(false);                                     \
    TEST(n, b);                                                                \
    srt_ctx_free(ctx);                                                         \
  } while (0)

#define END_TESTS printf("\n")

static void test_ctx() {
  START_TESTS;

  TEST("verbose flag can be set to true", {
    srt_context *ctx = srt_ctx_new(true);
    assert(ctx != NULL);
    assert(srt_ctx_verbose(ctx) == true);
    srt_ctx_free(ctx);
  });

  TEST("verbose flag can be set to false", {
    srt_context *ctx = srt_ctx_new(false);
    assert(ctx != NULL);
    assert(srt_ctx_verbose(ctx) == false);
    srt_ctx_free(ctx);
  });

  END_TESTS;
}

static void test_task_data() {
  START_TESTS;

  TEST_WITH_CTX("returns unknown key when getting unset int64", {
    const int32_t result = srt_task_data_get_int64(ctx, "x", NULL);
    assert(result == SRT_UNKNOWN_KEY);
  });

  TEST_WITH_CTX("can set and get int64", {
    {
      const int32_t result = srt_task_data_set_int64(ctx, "x", 11);
      assert(result == SRT_SUCCESS);
    }

    int64_t value;
    const int32_t result = srt_task_data_get_int64(ctx, "x", &value);
    assert(result == SRT_SUCCESS);
    assert(value == 11);
  });

  TEST_WITH_CTX("can reset and get int64", {
    {
      const int32_t result = srt_task_data_set_int64(ctx, "x", 11);
      assert(result == SRT_SUCCESS);
    }

    {
      const int32_t result = srt_task_data_set_int64(ctx, "x", 22);
      assert(result == SRT_SUCCESS);
    }

    int64_t value;
    const int32_t result = srt_task_data_get_int64(ctx, "x", &value);
    assert(result == SRT_SUCCESS);
    assert(value == 22);
  });

  TEST_WITH_CTX("can set and get different int64", {
    const int32_t result_x = srt_task_data_set_int64(ctx, "x", 11);
    assert(result_x == SRT_SUCCESS);

    const int32_t result_y = srt_task_data_set_int64(ctx, "y", 22);
    assert(result_y == SRT_SUCCESS);

    int64_t value_x;
    const int32_t result_x2 = srt_task_data_get_int64(ctx, "x", &value_x);
    assert(result_x2 == SRT_SUCCESS);
    assert(value_x == 11);

    int64_t value_y;
    const int32_t result_y2 = srt_task_data_get_int64(ctx, "y", &value_y);
    assert(result_y2 == SRT_SUCCESS);
    assert(value_y == 22);
  });

  TEST_WITH_CTX("can delete", {
    assert(srt_task_data_set_int64(ctx, "x", 11) == SRT_SUCCESS);
    assert(srt_task_data_delete(ctx, "x") == SRT_SUCCESS);
    assert(srt_task_data_get_int64(ctx, "x", NULL) == SRT_UNKNOWN_KEY);
  });

  END_TESTS;
}

int main(int argc, char **argv) {
  printf("libsrt_cli.a test harness\n\n");
  printf("running tests...\n\n");

  test_ctx();
  test_task_data();

  return 0;
}