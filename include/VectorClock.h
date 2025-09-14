#pragma once
#include <unordered_map>
#include <atomic>
#include <cstddef> // for thread_id type
#include <thread>

// Represents the Happens-Before state of a thread.
// A vector clock is a map: `thread_id -> logical timestamp`
class VectorClock {
private:
    std::unordered_map<std::thread::id, size_t> clocks_;
public:
    VectorClock();
    void tick(std::thread::id tid); // Advance thread `tid`'s clock by one
    void merge(const VectorClock& other); // Join operation (max for each thread)
    
    // HB relation checks
    bool happensBefore(const VectorClock& other) const;
    bool happensAfter(const VectorClock& other) const; 

    size_t getClock(std::thread::id tid) const;
    // ... (optional) helper to print the clock for debugging ...
};