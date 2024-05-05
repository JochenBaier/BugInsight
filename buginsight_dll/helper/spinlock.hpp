#include <atomic>
#include <thread>

//from https://rigtorp.se/spinlock/
struct spinlock_t {
  std::atomic<bool> lock_ { false };

  void lock() noexcept {
    for (;;) {
      // Optimistically assume the lock is free on the first try
      if (!lock_.exchange(true, std::memory_order_acquire)) {
        return;
      }
      // Wait for lock to be released without generating cache misses
      while (lock_.load(std::memory_order_relaxed)) {
        // Issue X86 PAUSE or ARM YIELD instruction to reduce contention between
        // hyper-threads
#ifdef _WIN32
        _mm_pause();
#else
        __builtin_ia32_pause();
#endif
      }
    }
  }

  void unlock() noexcept {
    lock_.store(false, std::memory_order_release);
  }
};
