#pragma once

// Forward declarations for the functions defined in Detector.cpp
// This tells the compiler these functions exist somewhere else (they will be linked later).
void handleLockAcquire(const void* mutex_addr);
void handleLockRelease(const void* mutex_addr);
void handleMemoryAccess(const void* addr, bool is_write);

// --- Macros to Instrument User Code ---
// WARNING: This is a crude method. A real tool uses compiler instrumentation.

// Wrap std::mutex::lock and unlock
// We use a void* cast to make the function more generic
#define mutex_lock(m) do { \
    handleLockAcquire(static_cast<const void*>(m)); \
    (m)->lock(); \
} while(0)

#define mutex_unlock(m) do { \
    (m)->unlock(); \
    handleLockRelease(static_cast<const void*>(m)); \
} while(0)

// Macro to instrument a memory READ
#define READ(x) (handleMemoryAccess(static_cast<const void*>(&(x)), false), (x))

// Macro to instrument a memory WRITE
#define WRITE(x, value) (handleMemoryAccess(static_cast<const void*>(&(x)), true), (x) = (value))