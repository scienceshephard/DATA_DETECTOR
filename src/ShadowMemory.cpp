#include "ShadowMemory.h"
#include <iostream>
#include <mutex> // Add this include

// Add a global mutex for thread-safe output
static std::mutex output_mutex;

// Gets the metadata for a given memory address.
// If it doesn't exist, it is created and initialized.
VarMetadata& ShadowMemory::getMetadata(const void* addr) {
    std::lock_guard<std::mutex> lock(table_mutex_); // Protect the shared map
    return shadow_map_[addr];
}

// The core function called on every memory access.
// It implements the right side of the diagram: HB check -> Lockset check -> Report -> Update.
void ShadowMemory::checkRace(const void* addr, bool is_write, const VectorClock& current_thread_clock, const LockSet& current_thread_locks) {
    std::lock_guard<std::mutex> lock(table_mutex_); // Protect the shared map
    VarMetadata& meta = shadow_map_[addr];

    // 1. HB Check: Are accesses ordered?
    bool is_ordered_by_hb = false;
    if (is_write) {
        // For a write, we must be after the last write AND the last read in HB.
        // For simplicity, we often just check the last write.
        is_ordered_by_hb = current_thread_clock.happensAfter(meta.write_clock);
        // A more precise check:
        // is_ordered_by_hb = current_thread_clock.happensAfter(meta.write_clock) &&
        //                    current_thread_clock.happensAfter(meta.read_clock);
    } else { // read
        // For a read, we must be after the last write in HB.
        is_ordered_by_hb = current_thread_clock.happensAfter(meta.write_clock);
    }

    // 2. If NOT ordered by HB, proceed to Lockset analysis
    if (!is_ordered_by_hb) {
        // Check for Lockset Violation
        bool common_lock_found = false;

        // If the candidate set is empty, it's the first access. No violation yet.
        if (!meta.candidate_locks.empty()) {
            // Check if the current thread holds ANY of the locks in the candidate set.
            for (const auto& candidate_lock : meta.candidate_locks) {
                if (current_thread_locks.contains(candidate_lock)) {
                    common_lock_found = true;
                    break;
                }
            }
            // A violation occurs if the candidate set is not empty AND the current thread
            // holds NONE of the candidate locks.
            if (!common_lock_found) {
                // 3. Report Tier 1 Race (HB & Lockset Violation)
                std::lock_guard<std::mutex> output_lock(output_mutex); // Add thread-safe output
                std::cerr << "========================" << std::endl;
                std::cerr << "DATA RACE DETECTED!" << std::endl;
                std::cerr << "Location: " << addr << std::endl;
                std::cerr << "Thread ID: " << std::this_thread::get_id() << std::endl;
                std::cerr << "Access type: " << (is_write ? "WRITE" : "READ") << std::endl;
                // TODO: Add stack trace printing here (using a library like libbacktrace)
                std::cerr << "========================" << std::endl;
            }
        }

        // 4. Update Candidate Lockset
        if (meta.candidate_locks.empty()) {
            // First access: initialize candidate set to the locks held *right now*
            meta.candidate_locks = current_thread_locks.getLocks();
        } else {
            if (!common_lock_found) {
                // We had a violation, so the candidate set becomes the empty set.
                // This means we will report every subsequent access as a race.
                meta.candidate_locks.clear();
            } else {
                // Standard case: Intersect the current candidate set with the locks
                // held by the current thread on this access.
                std::set<const void*> new_candidate_set;
                for (const auto& candidate_lock : meta.candidate_locks) {
                    if (current_thread_locks.contains(candidate_lock)) {
                        new_candidate_set.insert(candidate_lock);
                    }
                }
                meta.candidate_locks = std::move(new_candidate_set);
            }
        }
    }

    // 5. Update Variable Metadata (HB info)
    if (is_write) {
        meta.write_clock = current_thread_clock; // Record the VC of this write
    } else {
        // For reads, we merge the current clock into the read clock.
        // This is a conservative approximation.
        meta.read_clock.merge(current_thread_clock);
    }

    // Note: The "Advance Thread Clock" step is done by the caller (`handleMemoryAccess`)
    // after this function returns.
}