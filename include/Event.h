#pragma once

// Types of synchronization events our detector understands.
// This is useful for logging or more complex event handling in the future.
enum class EventType {
    ThreadStart,
    ThreadJoin,
    MutexLock,
    MutexUnlock,
    MemoryRead,
    MemoryWrite
    // Could add more: CondVar, Barrier, etc.
};

// (Optional) A structure to represent a logged event.
// This is more advanced and can be skipped for now.
/*
struct Event {
    EventType type;
    std::thread::id thread_id;
    const void* address; // Relevant for lock/memory events
    uint64_t timestamp;
};
*/