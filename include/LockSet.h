#pragma once
#include <set>

// Tracks the set of locks currently held by a specific thread.
// This is a thread-local object.
class LockSet {
    // Use a set of mutex addresses (as void*). The address is a unique identifier for the lock.
    std::set<const void*> locks_held_; // <-- KEY CHANGE: const void* instead of const std::mutex*
public:
    void addLock(const void* mutex_addr); // <-- Changed parameter type
    void removeLock(const void* mutex_addr); // <-- Changed parameter type
    const std::set<const void*>& getLocks() const; // <-- Changed return type
    bool contains(const void* mutex_addr) const; // <-- Changed parameter type
};