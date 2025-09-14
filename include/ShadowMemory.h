#pragma once
#include "VectorClock.h"
#include "LockSet.h"
#include <mutex>

// Metadata stored for every shared memory location we track.
struct VarMetadata {
    VectorClock write_clock;  // Vector clock of the last write
    VectorClock read_clock;   // Vector clock of the last read (can be complex, often simplified)
    std::set<const void*> candidate_locks; // Candidate lockset for this variable
    // ... other flags ...
};

// The main shadow memory table. Maps a memory address to its metadata.
class ShadowMemory {
private:
    std::unordered_map<const void*, VarMetadata> shadow_map_;
    std::mutex table_mutex_; // Protects concurrent access to the shadow_map_
public:
    VarMetadata& getMetadata(const void* addr); // Gets or creates metadata for `addr`
    void checkRace(const void* addr, bool is_write, const VectorClock& current_clock, const LockSet& current_lockset);
};