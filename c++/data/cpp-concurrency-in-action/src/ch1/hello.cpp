#include <iostream>
#include <thread>

void hello()
{
    std::cout << "Hello Concurrent World\n";
}

int main()
{
    //boost::thread t([]()
    std::thread t([]() {
        std::cout << "Hello Concurrent World\n";
    });

    t.join();
}
