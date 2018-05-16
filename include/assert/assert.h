#include <uart/log.h>

// If you want to remove ASSERT from the executable, define the macro DISABLE_ASSERT before including assert.h

#ifdef DISABLE_ASSERT
#define ASSERT(condition)

#else
#define ASSERT(condition) \
    if (condition) \
        print("ASSERT SUCCEEDED: %s, Line %d: %s", __FILE__, __LINE__, condition); \
    else \
        print("ASSERT FAILED: %s, Line %d: %s", __FILE__, __LINE__, condition);

#endif
