#include <iostream>
#include <thread>
#include <mutex>
#include <limits>
#include "Hook.h" 

int shared_value = 0;
std::mutex mtx;

void increment() {
    for (int i = 0; i < 100; ++i) {
        // mutex_lock(&mtx); // Comment this out to cause a race
        int temp = READ(shared_value); // Instrumented read
        // WRITE(shared_value, temp + 1); // Instrumented write
        
        mutex_unlock(&mtx);  //Comment this out to cause a race
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout << "Final value: " << shared_value << std::endl;

    std::cout <<"Press Enter to exit..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;
}