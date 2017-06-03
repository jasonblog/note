/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (C) 2011 Daniel Himmelein
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDROID_ASYNCTASK_H_
#define MINDROID_ASYNCTASK_H_

#include "mindroid/os/Handler.h"
#include "mindroid/util/concurrent/SerialExecutor.h"
#include "mindroid/util/concurrent/ThreadPoolExecutor.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"

namespace mindroid {

class AsyncTaskBase :
        public Object {
public:
    static sp<SerialExecutor> SERIAL_EXECUTOR;
    static sp<ThreadPoolExecutor> THREAD_POOL_EXECUTOR;

protected:
    static sp<ReentrantLock> sLock;

private:
    static const uint32_t THREAD_POOL_SIZE = 4;
};

/**
 * <p>
 * AsyncTask enables proper and easy use of the UI thread. This class allows to perform background
 * operations and publish results on the UI thread without having to manipulate threads and/or
 * handlers.
 * </p>
 *
 * <p>
 * AsyncTask is designed to be a helper class around {@link Thread} and {@link Handler} and does not
 * constitute a generic threading framework. AsyncTasks should ideally be used for short operations
 * (a few seconds at the most.) If you need to keep threads running for long periods of time, it is
 * highly recommended you use the various APIs provided by the <code>java.util.concurrent</code>
 * pacakge such as {@link Executor}, {@link ThreadPoolExecutor}.
 * </p>
 *
 * <p>
 * An asynchronous task is defined by a computation that runs on a background thread and whose
 * result is published on the UI thread. An asynchronous task is defined by 3 generic types, called
 * <code>Params</code>, <code>Progress</code> and <code>Result</code>, and 4 steps, called
 * <code>onPreExecute</code>, <code>doInBackground</code>, <code>onProgressUpdate</code> and
 * <code>onPostExecute</code>.
 * </p>
 *
 * <div class="special reference"> <h3>Developer Guides</h3>
 * <p>
 * For more information about using tasks and threads, read the <a href="{@docRoot}
 * guide/topics/fundamentals/processes-and-threads.html">Processes and Threads</a> developer guide.
 * </p>
 * </div>
 *
 * <h2>Usage</h2>
 * <p>
 * AsyncTask must be subclassed to be used. The subclass will override at least one method (
 * {@link #doInBackground}), and most often will override a second one ({@link #onPostExecute}.)
 * </p>
 *
 * <p>
 * Here is an example of subclassing:
 * </p>
 *
 * <pre class="prettyprint">
 * private class DownloadFilesTask extends AsyncTask&lt;URL, Integer, Long&gt; {
 *     protected Long doInBackground(Object urls) {
 *         int count = urls.length;
 *         long totalSize = 0;
 *         for (int i = 0; i &lt; count; i++) {
 *             totalSize += Downloader.downloadFile(urls[i]);
 *             publishProgress((int) ((i / (float) count) * 100));
 *             // Escape early if cancel() is called
 *             if (isCancelled()) break;
 *         }
 *         return totalSize;
 *     }
 *
 *     protected void onProgressUpdate(Integer... progress) {
 *         setProgressPercent(progress[0]);
 *     }
 *
 *     protected void onPostExecute(Long result) {
 *         showDialog(&quot;Downloaded &quot; + result + &quot; bytes&quot;);
 *     }
 * }
 * </pre>
 *
 * <p>
 * Once created, a task is executed very simply:
 * </p>
 *
 * <pre class="prettyprint">
 * new DownloadFilesTask().execute(url1, url2, url3);
 * </pre>
 *
 * <h2>AsyncTask's generic types</h2>
 * <p>
 * The three types used by an asynchronous task are the following:
 * </p>
 * <ol>
 * <li><code>Params</code>, the type of the parameters sent to the task upon execution.</li>
 * <li><code>Progress</code>, the type of the progress units published during the background
 * computation.</li>
 * <li><code>Result</code>, the type of the result of the background computation.</li>
 * </ol>
 * <p>
 * Not all types are always used by an asynchronous task. To mark a type as unused, simply use the
 * type {@link Void}:
 * </p>
 *
 * <pre>
 * private class MyTask extends AsyncTask&lt;Void, Void, Void&gt; { ... }
 * </pre>
 *
 * <h2>The 4 steps</h2>
 * <p>
 * When an asynchronous task is executed, the task goes through 4 steps:
 * </p>
 * <ol>
 * <li>{@link #onPreExecute()}, invoked on the UI thread immediately after the task is executed.
 * This step is normally used to setup the task, for instance by showing a progress bar in the user
 * interface.</li>
 * <li>{@link #doInBackground}, invoked on the background thread immediately after
 * {@link #onPreExecute()} finishes executing. This step is used to perform background computation
 * that can take a long time. The parameters of the asynchronous task are passed to this step. The
 * result of the computation must be returned by this step and will be passed back to the last step.
 * This step can also use {@link #publishProgress} to publish one or more units of progress. These
 * values are published on the UI thread, in the {@link #onProgressUpdate} step.</li>
 * <li>{@link #onProgressUpdate}, invoked on the UI thread after a call to {@link #publishProgress}.
 * The timing of the execution is undefined. This method is used to display any form of progress in
 * the user interface while the background computation is still executing. For instance, it can be
 * used to animate a progress bar or show logs in a text field.</li>
 * <li>{@link #onPostExecute}, invoked on the UI thread after the background computation finishes.
 * The result of the background computation is passed to this step as a parameter.</li>
 * </ol>
 *
 * <h2>Cancelling a task</h2>
 * <p>
 * A task can be cancelled at any time by invoking {@link #cancel(boolean)}. Invoking this method
 * will cause subsequent calls to {@link #isCancelled()} to return true. After invoking this method,
 * {@link #onCancelled(Object)}, instead of {@link #onPostExecute(Object)} will be invoked after
 * {@link #doInBackground(Object)} returns. To ensure that a task is cancelled as quickly as
 * possible, you should always check the return value of {@link #isCancelled()} periodically from
 * {@link #doInBackground(Object)}, if possible (inside a loop for instance.)
 * </p>
 *
 * <h2>Threading rules</h2>
 * <p>
 * There are a few threading rules that must be followed for this class to work properly:
 * </p>
 * <ul>
 * <li>The AsyncTask class must be loaded on the main thread.</li>
 * <li>The task instance must be created on the main thread.</li>
 * <li>{@link #execute} must be invoked on the main thread.</li>
 * <li>Do not call {@link #onPreExecute()}, {@link #onPostExecute}, {@link #doInBackground},
 * {@link #onProgressUpdate} manually.</li>
 * <li>The task can be executed only once (an exception will be thrown if a second execution is
 * attempted.)</li>
 * </ul>
 *
 * <h2>Memory observability</h2>
 * <p>
 * AsyncTask guarantees that all callback calls are synchronized in such a way that the following
 * operations are safe without explicit synchronizations.
 * </p>
 * <ul>
 * <li>Set member fields in the constructor or {@link #onPreExecute}, and refer to them in
 * {@link #doInBackground}.
 * <li>Set member fields in {@link #doInBackground}, and refer to them in {@link #onProgressUpdate}
 * and {@link #onPostExecute}.
 * </ul>
 */
template<typename Params, typename Progress, typename Result>
class AsyncTask :
        public AsyncTaskBase {
public:
    AsyncTask() :
            mExecutor(nullptr),
            mCancelled(false) {
        mHandler = new InternalHandler();
        mWorkerRunnable = new WorkerRunnable(this);
    }

    virtual ~AsyncTask() = default;

    AsyncTask(const AsyncTask&) = delete;
    AsyncTask& operator=(const AsyncTask&) = delete;

    sp<AsyncTask<Params, Progress, Result>> execute(Params params) {
        if (mExecutor == nullptr) {
            mExecutor = SERIAL_EXECUTOR;
            onPreExecute();
            mWorkerRunnable->mParams = params;
            mExecutor->execute(mWorkerRunnable);
            return this;
        } else {
            return nullptr;
        }
    }

    sp<AsyncTask<Params, Progress, Result>> executeOnExecutor(const sp<Executor>& executor, Params params) {
        if (mExecutor == nullptr) {
            mExecutor = executor;
            onPreExecute();
            mWorkerRunnable->mParams = params;
            mExecutor->execute(mWorkerRunnable);
            return this;
        } else {
            return nullptr;
        }
    }

    /**
     * <p>
     * Attempts to cancel execution of this task. This attempt will fail if the task has already
     * completed, already been cancelled, or could not be cancelled for some other reason. If
     * successful, and this task has not started when <tt>cancel</tt> is called, this task should
     * never run. If the task has already started, then the <tt>mayInterruptIfRunning</tt> parameter
     * determines whether the thread executing this task should be interrupted in an attempt to stop
     * the task.
     * </p>
     *
     * <p>
     * Calling this method will result in {@link #onCancelled(Object)} being invoked on the UI
     * thread after {@link #doInBackground(Object)} returns. Calling this method guarantees that
     * {@link #onPostExecute(Object)} is never invoked. After invoking this method, you should check
     * the value returned by {@link #isCancelled()} periodically from
     * {@link #doInBackground(Object)} to finish the task as early as possible.
     * </p>
     *
     * @param mayInterruptIfRunning <tt>true</tt> if the thread executing this task should be
     * interrupted; otherwise, in-progress tasks are allowed to complete.
     *
     * @return <tt>false</tt> if the task could not be cancelled, typically because it has already
     * completed normally; <tt>true</tt> otherwise
     *
     * @see #isCancelled()
     * @see #onCancelled(Object)
     */
    bool cancel() {
        bool isAlreadyCancelled = isCancelled();
        AutoLock autoLock(sLock);
        if (mExecutor != nullptr && !isAlreadyCancelled) {
            bool result = mExecutor->cancel(mWorkerRunnable);
            if (result) {
                mCancelled = true;
                sp<Message> message = mHandler->obtainMessage(ON_TASK_CANCELLED_MESSAGE);
                message->obj = mWorkerRunnable;
                message->sendToTarget();
            }
            return result;
        } else {
            return false;
        }
    }

    /**
     * Returns <tt>true</tt> if this task was cancelled before it completed normally. If you are
     * calling {@link #cancel(boolean)} on the task, the value returned by this method should be
     * checked periodically from {@link #doInBackground(Object)} to end the task as soon as
     * possible.
     *
     * @return <tt>true</tt> if task was cancelled before it completed
     *
     * @see #cancel(boolean)
     */
    bool isCancelled() {
        AutoLock autoLock(sLock);
        return mCancelled;
    }

protected:
    /**
     * Override this method to perform a computation on a background thread. The specified
     * parameters are the parameters passed to {@link #execute} by the caller of this task.
     *
     * This method can call {@link #publishProgress} to publish updates on the Handler thread.
     *
     * @param params The parameters of the task.
     *
     * @return A result, defined by the subclass of this task.
     *
     * @see #onPreExecute()
     * @see #onPostExecute
     * @see #publishProgress
     */
    virtual Result doInBackground(Params params) = 0;

    /**
     * Runs on the Handler thread before {@link #doInBackground}.
     *
     * @see #onPostExecute
     * @see #doInBackground
     */
    virtual void onPreExecute() {
    }

    /**
     * <p>
     * Runs on the Handler thread after {@link #doInBackground}. The specified result is the value
     * returned by {@link #doInBackground}.
     * </p>
     *
     * <p>
     * This method won't be invoked if the task was cancelled.
     * </p>
     *
     * @param result The result of the operation computed by {@link #doInBackground}.
     *
     * @see #onPreExecute
     * @see #doInBackground
     * @see #onCancelled(Object)
     */
    virtual void onPostExecute(Result result) {
    }

    /**
     * Runs on the Handler thread after {@link #publishProgress} is invoked. The specified values are the
     * values passed to {@link #publishProgress}.
     *
     * @param values The values indicating progress.
     *
     * @see #publishProgress
     * @see #doInBackground
     */
    virtual void onProgressUpdate(Progress values) {
    }

    /**
     * <p>
     * Applications should preferably override {@link #onCancelled(Object)}. This method is invoked
     * by the default implementation of {@link #onCancelled(Object)}.
     * </p>
     *
     * <p>
     * Runs on the Handler thread after {@link #cancel(boolean)} is invoked and
     * {@link #doInBackground(Object)} has finished.
     * </p>
     *
     * @see #onCancelled(Object)
     * @see #cancel(boolean)
     * @see #isCancelled()
     */
    virtual void onCancelled() {
    }

    /**
     * This method can be invoked from {@link #doInBackground} to publish updates on the UI thread
     * while the background computation is still running. Each call to this method will trigger the
     * execution of {@link #onProgressUpdate} on the UI thread.
     *
     * {@link #onProgressUpdate} will note be called if the task has been canceled.
     *
     * @param values The progress values to update the UI with.
     *
     * @see #onProgressUpdate
     * @see #doInBackground
     */
    void publishProgress(Progress values) {
        if (!isCancelled()) {
            sp<Message> message = mHandler->obtainMessage(ON_PROGRESS_UPDATE_MESSAGE);
            message->obj = object_cast<AsyncTaskResult>(new AsyncTaskResult(this, values));
            message->sendToTarget();
        }
    }

    Params params() {
        return mWorkerRunnable->mParams;
    }

    Result result() {
        return mWorkerRunnable->mResult;
    }

private:
    class InternalHandler :
            public Handler {
    public:
        virtual void handleMessage(const sp<Message>& message) {
            switch (message->what) {
            case ON_POST_EXECUTE_MESSAGE: {
                sp<WorkerRunnable> runnable = object_cast<WorkerRunnable>(message->obj);
                runnable->mTask->onPostExecute(runnable->mResult);
                break;
            }
            case ON_PROGRESS_UPDATE_MESSAGE: {
                sp<AsyncTaskResult> result = object_cast<AsyncTaskResult>(message->obj);
                result->mTask->onProgressUpdate(result->mValue);
                break;
            }
            case ON_TASK_CANCELLED_MESSAGE: {
                sp<WorkerRunnable> runnable = object_cast<WorkerRunnable>(message->obj);
                runnable->mTask->onCancelled();
                break;
            }
            }
        }
    };

    class WorkerRunnable :
            public Runnable {
    public:
        WorkerRunnable(const sp<AsyncTask<Params, Progress, Result>>& task) :
                mTask(task) {
        }

        virtual void run() {
            mResult = mTask->doInBackground(mParams);
            if (!mTask->isCancelled()) {
                sp<Message> message = mTask->mHandler->obtainMessage(ON_POST_EXECUTE_MESSAGE);
                message->obj = object_cast<WorkerRunnable>(this);
                message->sendToTarget();
            }
        }

        Params mParams;
        Result mResult;

    private:
        sp<AsyncTask<Params, Progress, Result>> mTask;

        friend class InternalHandler;
    };

    class AsyncTaskResult :
            public Object {
    public:
        AsyncTaskResult(const sp<AsyncTask<Params, Progress, Result>>& task, Progress& value) :
                mTask(task), mValue(value) {
        }

    private:
        sp<AsyncTask<Params, Progress, Result>> mTask;
        Progress mValue;

        friend class InternalHandler;
    };

    sp<Executor> mExecutor;
    sp<InternalHandler> mHandler;
    sp<WorkerRunnable> mWorkerRunnable;
    bool mCancelled;

    static const int32_t ON_POST_EXECUTE_MESSAGE = 1;
    static const int32_t ON_PROGRESS_UPDATE_MESSAGE = 2;
    static const int32_t ON_TASK_CANCELLED_MESSAGE = 3;
};

} /* namespace mindroid */

#endif /* MINDROID_ASYNCTASK_H_ */
