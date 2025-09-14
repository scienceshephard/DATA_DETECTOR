#include "VectorClock.h"
#include <thread>

VectorClock::VectorClock() {
    // Initialize with an empty map. A new thread's clock starts implicitly at 0.
}

void VectorClock::tick(std::thread::id tid) {
    // Advance the logical clock for thread `tid` by one.
    // If the thread id doesn't exist in the map, operator[] initializes it to 0, then we increment to 1.
    clocks_[tid] = clocks_[tid] + 1;
}

size_t VectorClock::getClock(std::thread::id tid) const {
    auto it = clocks_.find(tid);
    if (it != clocks_.end()) {
        return it->second;
    }
    return 0; // If the thread isn't in the map, its clock is implicitly 0.
}

void VectorClock::merge(const VectorClock& other) {
    // The join operation: for each thread ID, take the maximum clock value
    // between this vector clock and the other one.
    for (const auto& [tid, other_clock] : other.clocks_) {
        size_t& my_clock = clocks_[tid];
        my_clock = std::max(my_clock, other_clock);
    }
}

bool VectorClock::happensBefore(const VectorClock& other) const {
    // Check if *this* VectorClock happens-before *other*.
    // This is true iff for every thread T, this->clock(T) <= other.clock(T)
    // AND there exists at least one thread U where this->clock(U) < other.clock(U).

    bool at_least_one_strictly_less = false;

    // 1. Check all threads in *this* clock.
    for (const auto& [tid, clock_value] : clocks_) {
        size_t other_clock_value = other.getClock(tid);
        if (clock_value > other_clock_value) {
            return false; // Found a thread where this is ahead of other. HB is violated.
        }
        if (clock_value < other_clock_value) {
            at_least_one_strictly_less = true;
        }
    }

    // 2. Check for threads present in *other* but not in *this*.
    // For those threads, our clock is implicitly 0.
    for (const auto& [tid, other_clock_value] : other.clocks_) {
        if (clocks_.find(tid) == clocks_.end()) {
            // This thread is not in our map (so our clock is 0)
            if (0 > other_clock_value) {
                return false; // Can't happen (0 is always <=), but for logic completeness.
            }
            if (0 < other_clock_value) {
                at_least_one_strictly_less = true;
            }
        }
    }
    return at_least_one_strictly_less;
}

// A helper function to check if the current VC happens *after* another VC.
// This is just the inverse of happensBefore from the other's perspective.
// This is more intuitive to use in the detector logic.
bool VectorClock::happensAfter(const VectorClock& other) const {
    // This is the inverse of happensBefore.
    // Check if *other* happens-before *this*.
    return other.happensBefore(*this);
}