#include "Detector.h"
#include "ShadowMemory.h"
#include <iostream>
#include <thread>


// Thread-Local Storage for per-thread state
thread_local VectorClock current_vc;
thread_local LockSet current_lockset;

// Global Shadow Memory
ShadowMemory shadow_memory;

// A simple global map to store the vector clock of a lock at the time it was released.
// This is needed for a correct HB implementation for locks.
std::unordered_map<const void*, VectorClock> lock_release_vc_map; 
std::mutex lock_release_map_mutex; // Protects the map above

void handleThreadStart() {
    std::thread::id tid = std::this_thread::get_id();
    std::cout << "Thread started: " << tid << std::endl;
    current_vc.tick(tid); // Initialize the new thread's clock
}

void handleLockAcquire(const void* mutex_addr) {
  {
        std::lock_guard<std::mutex> lock(lock_release_map_mutex);
        auto it = lock_release_vc_map.find(mutex_addr);
        if (it != lock_release_vc_map.end()) {
            current_vc.merge(it->second);
        }
    }
    current_vc.tick(std::this_thread::get_id());
    current_lockset.addLock(mutex_addr); // LockSet::addLock also needs to accept const void*
    std::cout << "Lock Acquired: " << mutex_addr << " by thread " << std::this_thread::get_id() << std::endl;
}

// Parameter type changed from const std::mutex* to const void*
void handleLockRelease(const void* mutex_addr) {
    current_vc.tick(std::this_thread::get_id());
    {
        std::lock_guard<std::mutex> lock(lock_release_map_mutex);
        lock_release_vc_map[mutex_addr] = current_vc;
    }
    current_lockset.removeLock(mutex_addr); // LockSet::removeLock also needs to accept const void*
    std::cout << "Lock Released: " << mutex_addr << " by thread " << std::this_thread::get_id() << std::endl;
}

// handleMemoryAccess remains the same, it already uses const void*
void handleMemoryAccess(const void* addr, bool is_write) {
    shadow_memory.checkRace(addr, is_write, current_vc, current_lockset);
    current_vc.tick(std::this_thread::get_id());
}