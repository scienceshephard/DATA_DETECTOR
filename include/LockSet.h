#pragma once
#include <set>

// Tracks the set of locks currently held by a specific thread.
// This is a thread-local object.
class LockSet {
    std::set<const void*> locks_held_;
public:
    void addLock(const void* mutex_addr);
    void removeLock(const void* mutex_addr);
    const std::set<const void*>& getLocks() const;
    bool contains(const void* mutex_addr) const;
};