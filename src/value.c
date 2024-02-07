#include "value.h"
#include <stdlib.h>

void srt_value_free(srt_value *value) { free(value); }
