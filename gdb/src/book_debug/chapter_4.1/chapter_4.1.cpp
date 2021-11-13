#include <thread>
#include <iostream>
#include <vector>
#include <string>
int count = 0;
int do_work()
{
    int data = count;
    std::cout << "count is " << data << std::endl;
    count++;
    return 0;
}
int main()
{
    std::vector<std::thread> threads;
    //启动10个线程
    for (int i = 0; i < 100; ++i)
    {
        threads.push_back(std::thread(&do_work));
    }
    //等待所有线程结束
    for (auto& thread : threads)
    {
        thread.join();
    }
    std::cout << "The last count is " << count << std::endl;
}
