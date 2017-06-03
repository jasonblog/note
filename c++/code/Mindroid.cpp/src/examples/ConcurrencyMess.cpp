#include "mindroid/lang/Closure.h"
#include "mindroid/lang/Thread.h"
#include "mindroid/os/Looper.h"
#include "mindroid/os/Handler.h"
#include "mindroid/os/Message.h"
#include "mindroid/os/AsyncTask.h"
#include "mindroid/util/Log.h"
#include "mindroid/util/concurrent/Semaphore.h"

using namespace mindroid;

class Handler3;
class Handler4;
sp<Looper> sLooper1;
sp<Looper> sLooper2;
sp<Handler> sHandler1;
sp<Handler> sHandler2;
sp<Handler3> sHandler3;
sp<Handler4> sHandler4;
sp<Semaphore> sSemaphore1 = new Semaphore(0);
sp<Semaphore> sSemaphore2 = new Semaphore(0);

static const char* TAG = "Mindroid";

class Params :
    public Object {
public:
    static const uint32_t ARRAY_SIZE = 32;

    Params() {
        for (uint32_t i = 0; i < ARRAY_SIZE; i++) {
            values[i] = i;
        }
    }

    uint32_t values[ARRAY_SIZE];
};

class AsyncTask1 : public AsyncTask<sp<Params>, int32_t, int32_t> {
public:
    virtual void onPreExecute() {
        Log::d(TAG, "AsyncTask1::onPreExecute on thread %d", (int32_t) pthread_self());
    }

    virtual int32_t doInBackground(sp<Params> params) {
        Log::d(TAG, "AsyncTask1::doInBackground on thread %d with params %d", (int32_t) pthread_self(), params->values[0]);
        Thread::sleep(250);
        uint32_t sum = 0;
        uint32_t i;
        for (i = 0; i < Params::ARRAY_SIZE / 2; i++) {
            sum += params->values[i];
        }
        publishProgress(sum);
        Thread::sleep(250);
        for (; i < Params::ARRAY_SIZE; i++) {
            sum += params->values[i];
        }
        return sum;
    }

    virtual void onProgressUpdate(int32_t value) {
        Log::d(TAG, "AsyncTask1::onProgressUpdate on thread %d with value %d", (int32_t) pthread_self(), value);
    }

    virtual void onPostExecute(int32_t result) {
        Log::d(TAG, "AsyncTask1::onPostExecute on thread %d with result %d", (int32_t) pthread_self(), result);
    }

    virtual void onCancelled() {
        Log::d(TAG, "AsyncTask1::onCancelled on thread %d", (int32_t) pthread_self());
    }
};

class AsyncTask2 : public AsyncTask<int32_t, int32_t, int32_t> {
public:
    virtual void onPreExecute() {
        Log::d(TAG, "AsyncTask2::onPreExecute on thread %d", (int32_t) pthread_self());
    }

    virtual int32_t doInBackground(int32_t param) {
        Log::d(TAG, "AsyncTask2::doInBackground on thread %d with params %d", (int32_t) pthread_self(), param);
        Thread::sleep(100);
        return 42;
    }

    virtual void onPostExecute(int32_t result) {
        Log::d(TAG, "AsyncTask2::onPostExecute on thread %d with result %d", (int32_t) pthread_self(), result);
    }

    virtual void onCancelled() {
        Log::d(TAG, "AsyncTask2::onCancelled on thread %d", (int32_t) pthread_self());
    }
};

class Handler1 : public Handler {
public:
    static const uint32_t MSG_START_ASYNC_TASKS = 1;
    static const uint32_t MSG_PRINT_INFO = 2;

    virtual void handleMessage(const sp<Message>& message) {
        switch (message->what) {
        case MSG_START_ASYNC_TASKS: {
            mAsyncTask1 = new AsyncTask1();
            mAsyncTask1->execute(new Params());
            mAsyncTask2 = new AsyncTask2();
            mAsyncTask2->executeOnExecutor(AsyncTaskBase::THREAD_POOL_EXECUTOR, 1234567);
            mAsyncTask3 = new AsyncTask2();
            mAsyncTask3->executeOnExecutor(AsyncTaskBase::THREAD_POOL_EXECUTOR, 7654321);
            break;
        }
        case MSG_PRINT_INFO:
            Log::i(TAG, "Handler1::handleMessage %p with ID %d by Looper %p",
                    message.getPointer(), message->what, Looper::myLooper().getPointer());
            break;
        default:
            Log::i(TAG, "Handler1::handleMessage: Oops, this message ID is not valid");
            break;
        }
    }

private:
    sp<AsyncTask1> mAsyncTask1;
    sp<AsyncTask2> mAsyncTask2;
    sp<AsyncTask2> mAsyncTask3;
};

class Handler2 : public Handler {
public:
    virtual void handleMessage(const sp<Message>& message) {
        Log::d(TAG, "Handler2::handleMessage %p with ID %d by Looper %p",
                message.getPointer(), message->what, Looper::myLooper().getPointer());
    }
};

class Handler3 : public Handler {
public:
    virtual void handleMessage(const sp<Message>& message) {
        Log::d(TAG, "Handler3::handleMessage %p with ID %d by Looper %p",
                message.getPointer(), message->what, Looper::myLooper().getPointer());
    }

    void test() {
        sendMessage(obtainMessage(3));
    }
};

class Handler4 : public Handler {
public:
    Handler4(const sp<Looper>& looper) : Handler(looper) {}

    virtual void handleMessage(const sp<Message>& message) {
        Log::d(TAG, "Handler4::handleMessage %p with ID %d by Looper %p",
                message.getPointer(), message->what, Looper::myLooper().getPointer());
    }

    void test() {
        sendMessageDelayed(obtainMessage(4), 1000);
    }
};

class Thread1 : public Thread {
public:
    virtual void run() {
        Looper::prepare();
        sLooper1 = Looper::myLooper();
        sHandler1 = new Handler1();
        sHandler3 = new Handler3();
        Log::d(TAG, "Looper 1 uses thread %d", (int32_t) pthread_self());
        sSemaphore1->release();
        Looper::loop();
    }
};

class Thread2 : public Thread {
public:
    virtual void run() {
        Looper::prepare();
        sLooper2 = Looper::myLooper();
        sHandler2 = new Handler2();
        Log::d(TAG, "Looper 2 uses thread %d", (int32_t) pthread_self());
        sSemaphore2->release();
        Looper::loop();
    }
};

class Runnable1 : public Runnable {
public:
    virtual void run() {
        Log::d(TAG, "Runnable1::run by Looper %p", Looper::myLooper().getPointer());
        assert(false);
    }
};

class TestClosure1 {
public:
    void test(int32_t value) {
        Log::d(TAG, "TestClosure1::test with value %d by Looper %p",
                value, Looper::myLooper().getPointer());
    }
};

sp<Thread1> sThread1 = new Thread1();
sp<Thread2> sThread2 = new Thread2();
TestClosure1 sTestClosure1;

int main() {
    sThread1->start();
    sThread2->start();

    sSemaphore1->acquire();
    sSemaphore2->acquire();

    sHandler4 = new Handler4(sLooper2);

    int turns = 10;
    while (turns >= 0) {
        turns--;
        sHandler1->sendMessage(sHandler1->obtainMessage(Handler1::MSG_PRINT_INFO));

        sHandler1->obtainMessage(Handler1::MSG_START_ASYNC_TASKS)->sendToTarget();

        sp<Runnable1> runnable1 = new Runnable1();
        sHandler2->postDelayed(runnable1, 100);
        sHandler2->removeCallbacks(runnable1);
        sHandler2->postDelayed(obtainClosure(sTestClosure1, &TestClosure1::test, 42), 500);

        sHandler3->test();
        sHandler4->test();

        Thread::sleep(2000);
    }

    sLooper1->quit();
    sThread1->join();
    sLooper2->quit();
    sThread2->join();

    return 0;
}
