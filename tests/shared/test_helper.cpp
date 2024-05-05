#include "test_helper.hpp"

#include <intrin.h>

namespace test_helper
{
__declspec(noinline)void no_optimize(void* ptr)
{
  (void)ptr;                                                                 \
   _ReadWriteBarrier();                                                       \
}

}

