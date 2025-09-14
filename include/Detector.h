#pragma once

// These functions are called by the hooking mechanism (e.g., our macros or LD_PRELOAD)
void handleThreadStart();
void handleLockAcquire(const void* mutex_addr);
void handleLockRelease(const void* mutex_addr);
void handleMemoryAccess(const void* addr, bool is_write);