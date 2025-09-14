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
};
