#include "LockSet.h"

// Change 1: Parameter type is now const void*
void LockSet::addLock(const void* mutex_addr) {
    locks_held_.insert(mutex_addr);
}

// Change 2: Parameter type is now const void*
void LockSet::removeLock(const void* mutex_addr) {
    locks_held_.erase(mutex_addr);
}

// Change 3: Return type is now std::set<const void*>
const std::set<const void*>& LockSet::getLocks() const {
    return locks_held_;
}

// Change 4: Parameter type is now const void*
bool LockSet::contains(const void* mutex_addr) const {
    return locks_held_.find(mutex_addr) != locks_held_.end();
}