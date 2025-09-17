/*
    This is a typical example of a this issue showing up.
    Two threads are created to run the same function
    But this same function uses te same variable which is a global variable counter
    When the thread run at the background they could clash alot of time due to but of them trying to access 
        the same variable (memory) at the same time or even trying to run the same process (method) at the same time
    Therefore the program will not attempts its goal. The threads will never count up to it's final value.
    They will miss their count
*/



#include <iostream>
#include <thread>

int counter = 0; // A shared global variable

void increment() {
    for (int i = 0; i < 1000000; ++i) {
        counter++; // This is the CRITICAL SECTION: a read-modify-write operation.
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << "Counter value: " << counter << std::endl;
    // You will almost never get 1000000 due to the data race!
    
    std::cout <<"Press Enter to exit..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;
}