#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include <mutex>  
int count = 0;
std::mutex glocker;
void do_save(int number)
{
    glocker.lock();
    count = count + number;
    glocker.unlock();
    std::cout << "count is " << count << std::endl;
}
void do_withdraw(int number)
{
    glocker.lock();
    count = count - number;
    glocker.unlock();
    std::cout << "count is " << count << std::endl;
}

class test_timed_mutex_c
{
public:
    test_timed_mutex_c() {}
    virtual ~test_timed_mutex_c() {}
public:
    int get_data()
    {
        int data = 0;
        if (_mutex.try_lock_for(std::chrono::milliseconds(2000)))
            _mutex.unlock();
        data = _data;

    }
    void set_data(int data)
    {
        if (_mutex.try_lock_for(std::chrono::milliseconds(2000)))
            _mutex.unlock();
        _data = data;
    }
private:
    std::timed_mutex _mutex;
    int _data;
};
class test_mutex_c
{
public:
    test_mutex_c(){}
    virtual ~test_mutex_c() {}
public:
    int get_data()
    {
        int data = 0;
        std::lock_guard<std::mutex> locker(_mutex);

        data = _data;

    }
    void set_data(int data)
    {
        std::lock_guard<std::mutex> locker(_mutex);
        if (data == 0)
            return;
        if (data == 1)
            throw 1;
        _data = data;
    }
private:
    std::mutex _mutex;
    int _data;
};

int main()
{
    std::vector<std::thread> threads;
    //启动10个线程
    for (int i = 0; i < 10; ++i)
    {
        if(i % 2 == 0)
            threads.push_back(std::thread(&do_save,100));
        else
            threads.push_back(std::thread(&do_withdraw, 100));

    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
    //等待所有线程结束
    for (auto& thread : threads)
    {
        thread.join();
    }
    std::cout << "The last count is " << count << std::endl;
}