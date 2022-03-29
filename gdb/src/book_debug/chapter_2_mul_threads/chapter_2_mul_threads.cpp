#include <thread>
#include <iostream>
#include <vector>
#include <string>
int count = 0;
void do_work(void *arg) 
{
    std::cout << "线程函数开始"<< std::endl;
    //模拟做一些事情
    int data = count;
    count++;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "线程函数结束" << std::endl;
}
int main()
{
    std::vector<std::thread> threads;
    //启动10个线程
    for (int i = 0; i < 10; ++i) 
    {
        threads.push_back(std::thread(do_work,&i));
        std::cout << "启动新线程：" << i << std::endl;
    } 
    //等待所有线程结束
    for (auto& thread : threads)
    {
        thread.join();
    }
}
