# How to Stop or Terminate a Thread


```cpp
#include <thread>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <future>

/*
 * Class that encapsulates promise and future object and
 * provides API to set exit signal for the thread
 */
class Stoppable
{
    std::promise<void> exitSignal;
    std::future<void> futureObj;
public:
    Stoppable() :
        futureObj(exitSignal.get_future())
    {

    }
    Stoppable(Stoppable&& obj) : exitSignal(std::move(obj.exitSignal)),
        futureObj(std::move(obj.futureObj))
    {
        std::cout << "Move Constructor is called" << std::endl;
    }

    Stoppable& operator=(Stoppable&& obj)
    {
        std::cout << "Move Assignment is called" << std::endl;
        exitSignal = std::move(obj.exitSignal);
        futureObj = std::move(obj.futureObj);
        return *this;
    }

    // Task need to provide defination  for this function
    // It will be called by thread function
    virtual void run() = 0;


    // Thread function to be executed by thread
    void operator()()
    {
        run();
    }

    //Checks if thread is requested to stop
    bool stopRequested()
    {
        // checks if value in future object is available
        if (futureObj.wait_for(std::chrono::milliseconds(0)) ==
            std::future_status::timeout) {
            return false;
        }

        return true;
    }
    // Request the thread to stop by setting value in promise object
    void stop()
    {
        exitSignal.set_value();
    }
};

/*
 * A Task class that extends the Stoppable Task
 */
class MyTask: public Stoppable
{
public:
    // Function to be executed by thread function
    void run()
    {
        std::cout << "Task Start" << std::endl;

        // Check if thread is requested to stop ?
        while (stopRequested() == false) {
            std::cout << "Doing Some Work" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        }

        std::cout << "Task End" << std::endl;
    }
};

int main()
{
    // Creating our Task
    MyTask task;

    //Creating a thread to execute our task
    std::thread th([&]() {
        task.run();
    });

    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Asking Task to Stop" << std::endl;
    // Stop the Task
    task.stop();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    //Waiting for thread to join
    th.join();
    std::cout << "Thread Joined" << std::endl;
    std::cout << "Exiting Main Function" << std::endl;
    return 0;
}
```
