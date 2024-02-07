#include <stdint.h>

#define RESULT(n, v) static const uint32_t SRT_##n = v

RESULT(SUCCESS, 0);
RESULT(UNKNOWN_KEY, 1);
RESULT(KEY_TYPE_MISMATCH, 2);
RESULT(UNKNOWN_ERROR, 3);
