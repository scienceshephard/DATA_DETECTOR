#include <iostream>
#include <thread>
#include <limits>
#include <mutex>
#include "Hook.h"

int shared_value = 0;
std::mutex mtx;

void unsafe_increment() {
    for (int i = 0; i < 100000; ++i) {
        // INTENTIONALLY UNSAFE: No locking, direct access
        // This should trigger your data race detector
        int temp = READ(shared_value);    // Use instrumented READ
        WRITE(shared_value, temp + 1);    // Use instrumented WRITE
        // Don't use mutex_lock/unlock here - we WANT a race!
    }
}

void safe_increment() {
    for (int i = 0; i < 100000; ++i) {
        // Safe version with locking
        mutex_lock(&mtx);
        int temp = READ(shared_value);
        WRITE(shared_value, temp + 1);
        mutex_unlock(&mtx);
    }
}

int main() {
    std::cout << "Testing UNSAFE increment (should show data races):" << std::endl;
    
    // Test UNSAFE version (should trigger races)
    shared_value = 0;
    std::thread t1(unsafe_increment);
    std::thread t2(unsafe_increment);
    t1.join();
    t2.join();
    
    std::cout << "Final value (unsafe): " << shared_value << std::endl;
    
    std::cout << "\nTesting SAFE increment (should show no races):" << std::endl;
    
    // Test SAFE version (should not trigger races)
    shared_value = 0;
    std::thread t3(safe_increment);
    std::thread t4(safe_increment);
    t3.join();
    t4.join();
    
    std::cout << "Final value (safe): " << shared_value << std::endl;
    
    // Keep window open
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    return 0;
}