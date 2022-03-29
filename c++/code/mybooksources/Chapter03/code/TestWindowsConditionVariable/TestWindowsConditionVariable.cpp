/** 
 * ��ʾ Windows ����������ʹ��
 * zhangyl 20191111
 */

#include <Windows.h>
#include <iostream>
#include <list>

class Task
{
public:
    Task(int taskID)
    {
        this->taskID = taskID;
    }

    void doTask()
    {
        std::cout << "handle a task, taskID: " << taskID << ", threadID: " << GetCurrentThreadId() << std::endl;
    }

private:
    int taskID;
};

CRITICAL_SECTION    myCriticalSection;
CONDITION_VARIABLE  myConditionVar;
std::list<Task*>    tasks;

DWORD WINAPI consumerThread(LPVOID param)
{
    Task* pTask = NULL;
    while (true)
    {
        //�����ٽ���
        EnterCriticalSection(&myCriticalSection);
        while (tasks.empty())
        {
            //���SleepConditionVariableCS���𣬹���ǰ���뿪�ٽ�����������ִ�С�
            //�������仯���������ʣ�SleepConditionVariableCS��ֱ�ӽ����ٽ�����
            SleepConditionVariableCS(&myConditionVar, &myCriticalSection, INFINITE);
        }

        pTask = tasks.front();
        tasks.pop_front();

        //SleepConditionVariableCS�����Ѻ�����ٽ�����
        //Ϊ���������߳��л������tasks��������Ҫ�ٴ��뿪�ٽ���
        LeaveCriticalSection(&myCriticalSection);

        if (pTask == NULL)
            continue;

        pTask->doTask();
        delete pTask;
        pTask = NULL;
    }

    return 0;
}

DWORD WINAPI producerThread(LPVOID param)
{
    int taskID = 0;
    Task* pTask = NULL;

    while (true)
    {
        pTask = new Task(taskID);

        //�����ٽ���
        EnterCriticalSection(&myCriticalSection);
        tasks.push_back(pTask);
        std::cout << "produce a task, taskID: " << taskID << ", threadID: " << GetCurrentThreadId() << std::endl;

        LeaveCriticalSection(&myCriticalSection);

        WakeConditionVariable(&myConditionVar);

        taskID++;

        //����1��
        Sleep(1000);
    }

    return 0;
}

int main()
{
    InitializeCriticalSection(&myCriticalSection);
    InitializeConditionVariable(&myConditionVar);

    //����5���������߳�
    HANDLE consumerThreadHandles[5];
    for (int i = 0; i < 5; ++i)
    {
        consumerThreadHandles[i] = CreateThread(NULL, 0, consumerThread, NULL, 0, NULL);
    }

    //����һ���������߳�
    HANDLE producerThreadHandle = CreateThread(NULL, 0, producerThread, NULL, 0, NULL);

    //�ȴ��������߳��˳�
    WaitForSingleObject(producerThreadHandle, INFINITE);

    //�ȴ��������߳��˳�
    for (int i = 0; i < 5; ++i)
    {
        WaitForSingleObject(consumerThreadHandles[i], INFINITE);
    }

    DeleteCriticalSection(&myCriticalSection);

    return 0;
}