#include <mutex>
#include "Detector.h" // This ensures handleMemoryAccess is declared

// --- Macros to Instrument User Code ---
// WARNING: This is a crude method. A real tool uses compiler instrumentation.

// Wrap std::mutex::lock and unlock
#define mutex_lock(m) do { \
    handleLockAcquire((m)); \
    (m)->lock(); \
} while(0)

#define mutex_unlock(m) do { \
    (m)->unlock(); \
    handleLockRelease((m)); \
} while(0)

// Macro to instrument a memory READ
#define READ(x) (handleMemoryAccess(&(x), false), (x))

// Macro to instrument a memory WRITE
#define WRITE(x, value) (handleMemoryAccess(&(x), true), (x) = (value))
