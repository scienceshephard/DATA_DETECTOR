#include <iostream>
#include <thread>

void myThread(int arg){
    std::cout << "Hello from this thread"<< arg <<std::endl;
}

auto myLambdaThread = [](std::string message){
    std::cout << "Lamda thread says: " << message << std::endl;
};

int main(){

    std::thread t1(myThread, 42); // Thread running myThreadFunction with argument 42
    std::thread t2(myLambdaThread, "Greetings!"); // Thread running the lambda with a string argument

            std::thread backgroundThread([](){ 
                for (int i = 0; i < 5; i++)
                {
                    std::cout << "Running at the bacground" << std::endl;
                }
                
             });
    t1.join(); // Wait for t1 to finish
        backgroundThread.detach(); // Allow backgroundThread to run independently
    t2.join(); // Wait for t2 to finish

    return 0;
}