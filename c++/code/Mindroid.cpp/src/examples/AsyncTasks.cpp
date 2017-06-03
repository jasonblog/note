#include "mindroid/lang/Closure.h"
#include "mindroid/lang/Thread.h"
#include "mindroid/os/Looper.h"
#include "mindroid/os/Handler.h"
#include "mindroid/os/Message.h"
#include "mindroid/os/AsyncTask.h"
#include "mindroid/util/Log.h"

using namespace mindroid;

class ExampleAsyncTask : public AsyncTask<int32_t, int32_t, int32_t> {
public:
    virtual void onPreExecute() {
        Log::d("ExampleAsyncTask", "onPreExecute on thread %d", (int32_t) pthread_self());
    }

    virtual int32_t doInBackground(int32_t param) {
        Log::d("ExampleAsyncTask", "doInBackground on thread %d with param %d", (int32_t) pthread_self(), param);
        Thread::sleep(250);
        int32_t sum = 0;
        int32_t i;
        for (i = 0; i < param / 2; i++) {
            sum++;
        }
        publishProgress(sum);
        Thread::sleep(250);
        for (; i < param; i++) {
            sum++;
        }
        return sum;
    }

    virtual void onProgressUpdate(int32_t value) {
        Log::d("ExampleAsyncTask", "onProgressUpdate on thread %d with value %d", (int32_t) pthread_self(), value);
    }

    virtual void onPostExecute(int32_t result) {
        Log::d("ExampleAsyncTask", "onPostExecute on thread %d with result %d", (int32_t) pthread_self(), result);
    }

    virtual void onCancelled() {
        Log::d("ExampleAsyncTask", "onCancelled on thread %d", (int32_t) pthread_self());
    }
};

int main() {
    sp<Handler> handler;
    sp<ExampleAsyncTask> asyncTask1;
    sp<ExampleAsyncTask> asyncTask2;

    Looper::prepare();
    handler = new Handler(Looper::myLooper());
    handler->post([&] {
            asyncTask1 = new ExampleAsyncTask();
            asyncTask1->execute(1234567);

            asyncTask2 = new ExampleAsyncTask();
            asyncTask2->executeOnExecutor(AsyncTaskBase::THREAD_POOL_EXECUTOR, 123);

            handler->postDelayed([=] {
                    Looper::myLooper()->quit();
            }, 2000);
    });
    Looper::loop();

    return 0;
}
