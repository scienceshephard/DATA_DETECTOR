// samples/race_example.cpp
#include <iostream>
#include <thread>
#include <mutex>

// Include the header that defines our wrapping macros
#include "Hook.h" // This would define mutex_lock, READ, WRITE, etc.

int shared_value = 0;
std::mutex mtx;

void increment() {
    for (int i = 0; i < 100000; ++i) {
        // The user must use our macros for the tool to work in this simple model.
        mutex_lock(&mtx); // Using our wrapped version
        int temp = READ(shared_value); // Instrumented read
        WRITE(shared_value, temp + 1); // Instrumented write
        // mutex_unlock(&mtx);
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout << "Final value: " << shared_value << std::endl;
    return 0;
}