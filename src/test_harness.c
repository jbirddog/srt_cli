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

static void test_dict() {
  START_TESTS;

  TEST("can new and free", {
    srt_dict *d = srt_dict_new(2);
    srt_dict_free(d);
  });

  TEST("can new with kvs and free", {
    srt_dict *d = srt_dict_new_with_kvs(3, "key1", srt_value_new_bool(true),
                                        "key2", srt_value_new_int64(13), "key3",
                                        srt_value_new_bool(false));
    assert(srt_dict_len(d) == 3);
    srt_dict_free(d);
  });

  END_TESTS;
}

static void test_task_data() {
  START_TESTS;

  TEST_WITH_CTX("returns unknown key when getting unset bool", {
    const int32_t result = srt_task_data_try_get_bool(ctx, "x", NULL);
    assert(result == SRT_UNKNOWN_KEY);
  });

  TEST_WITH_CTX("returns unknown key when getting unset int64", {
    const int32_t result = srt_task_data_try_get_int64(ctx, "x", NULL);
    assert(result == SRT_UNKNOWN_KEY);
  });

  TEST_WITH_CTX("can set and get bool", {
    {
      const int32_t result = srt_task_data_try_set_bool(ctx, "x", true);
      assert(result == SRT_SUCCESS);
    }

    bool value;
    const int32_t result = srt_task_data_try_get_bool(ctx, "x", &value);
    assert(result == SRT_SUCCESS);
    assert(value == true);
  });

  TEST_WITH_CTX("can set and get dict", {
    srt_dict *d = srt_dict_new(2);
    assert(d != NULL);

    {
      const int32_t result = srt_task_data_try_set_dict(ctx, "x", d);
      assert(result == SRT_SUCCESS);
    }

    srt_dict *value;
    const int32_t result = srt_task_data_try_get_dict(ctx, "x", &value);
    assert(result == SRT_SUCCESS);
    assert(value == d);

    srt_dict_free(d);
  });

  TEST_WITH_CTX("can set and get int64", {
    {
      const int32_t result = srt_task_data_try_set_int64(ctx, "x", 11);
      assert(result == SRT_SUCCESS);
    }

    int64_t value;
    const int32_t result = srt_task_data_try_get_int64(ctx, "x", &value);
    assert(result == SRT_SUCCESS);
    assert(value == 11);
  });

  TEST_WITH_CTX("can reset and get int64", {
    {
      const int32_t result = srt_task_data_try_set_int64(ctx, "x", 11);
      assert(result == SRT_SUCCESS);
    }

    {
      const int32_t result = srt_task_data_try_set_int64(ctx, "x", 22);
      assert(result == SRT_SUCCESS);
    }

    int64_t value;
    const int32_t result = srt_task_data_try_get_int64(ctx, "x", &value);
    assert(result == SRT_SUCCESS);
    assert(value == 22);
  });

  TEST_WITH_CTX("can set and get different int64", {
    const int32_t result_x = srt_task_data_try_set_int64(ctx, "x", 11);
    assert(result_x == SRT_SUCCESS);

    const int32_t result_y = srt_task_data_try_set_int64(ctx, "y", 22);
    assert(result_y == SRT_SUCCESS);

    int64_t value_x;
    const int32_t result_x2 = srt_task_data_try_get_int64(ctx, "x", &value_x);
    assert(result_x2 == SRT_SUCCESS);
    assert(value_x == 11);

    int64_t value_y;
    const int32_t result_y2 = srt_task_data_try_get_int64(ctx, "y", &value_y);
    assert(result_y2 == SRT_SUCCESS);
    assert(value_y == 22);
  });

  TEST_WITH_CTX("can get a type mismatch", {
    {
      const int32_t result = srt_task_data_try_set_bool(ctx, "x", true);
      assert(result == SRT_SUCCESS);
    }

    int64_t value;
    const int32_t result = srt_task_data_try_get_int64(ctx, "x", &value);
    assert(result == SRT_KEY_TYPE_MISMATCH);
  });

  TEST_WITH_CTX("can delete", {
    assert(srt_task_data_try_set_int64(ctx, "x", 11) == SRT_SUCCESS);
    assert(srt_task_data_try_delete(ctx, "x") == SRT_SUCCESS);
    assert(srt_task_data_try_get_int64(ctx, "x", NULL) == SRT_UNKNOWN_KEY);
  });

  TEST_WITH_CTX("can happy path without panic", {
    srt_task_data_set_int64(ctx, "x", 11);
    assert(srt_task_data_get_int64(ctx, "x") == 11);
    srt_task_data_delete(ctx, "x");
    srt_task_data_set_int64(ctx, "x", 13);
    assert(srt_task_data_get_int64(ctx, "x") == 13);
  });

  END_TESTS;
}

int main(int argc, char **argv) {
  printf("libsrt_cli.a test harness\n\n");
  printf("running tests...\n\n");

  test_ctx();
  test_dict();
  test_task_data();

  return 0;
}
