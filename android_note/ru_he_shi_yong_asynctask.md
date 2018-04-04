# 如何使用AsyncTask


Asynctask是一種在背景執行緒運作的非同步任務, 
如果你使用它, 則必須覆寫它唯一的方法doInBackground。
```java
 class MyTask extends AsyncTask<Void, Void, Void>{

    @Override
    protected Void doInBackground(Void... params) {
        return null;
    }
}
```
當AsyncTask執行完以後, 就不能再被執行了, 跟Thread行為相同,
如果多次執行將會丟出illegalStateException,
AsyncTask除了提供背景執行功能外, 還可以把物件傳到背景Thread, 
透過AsyncTask你不必費心Hanlder傳送跟Message處理。

```java
 class MyTask extends AsyncTask<Params, Progress, Result>{
    protected void onPreExecute(){
       // in main thread

    }

    protected Result doInBackground(Params... params){
       // in background thread

    }

    protected void onProgressUpdate(Progress... progress){
       // in main thread

    }

    protected void onPostExecute(Result result){
       // in main thread

    }
 
    protected void onCancelled(Result result){
       // in main thread

    }

}
```

- Params 輸入背景任務的資料
- Progress 由Background thread向UI Thread報告進度
- Result Background thread向UI Thread報告結果

- 如果你要執行一個AsyncTask必須在Main Thread上面呼叫execute, 否則callback method將會傳不到。

如果你要取消一個AsyncTask可以這樣

```java
AsyncTask task = new MyTask();
task.exectue(/*參數*/);
task.cancel(true);
```

當你呼叫execute()方法時, 則會使用預設的ThreadPool, 而預設的ThreadPool只會有5個core Thread,
因此如果超過5個task將會被放進Queue等待, 
使用沒有參數的execute方法, 在3.0之後將採取循序完成,
如果想要調整, 可以選擇是否要採取循序。

```java
//循序
executeOnExecutor(AsyncTask.SERIAL_EXECUTOR, Object... objs);
//並行
executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR, Object... objs);
```

另外由於預設的ThreadPool是static, 
當app內其他功能使用asynctask, 則將會讓任務更晚完成。
如果使用cancel(true)方法則會發出InterruptException,
當收到取消請求時, 則不會呼叫onPostExecute方法, 改回呼onCancelled。

AsyncTask存在幾種狀態

- PENDING 當AsyncTask建立但是卻尚未執行。
- RUNNING 執行execute
- FINISHED onPostExecute或onCancelled被呼叫

```java
AsyncTask.getStatus();
```

可以透過上面的方法來觀察到哪個狀態。

以下兩種狀況不適合使用AsyncTask 不使用任何參數執行任務

```java
AsyncTask<Void, Void, Void>
```
沒有使用參數, 背景執行緒無法跟UI執行緒溝通, 沒有任何回傳結果。

```java
只實做doInBackground
```

沒有提供報告進度或結果, 就只是個背景任務。

如果有以上的情況, 請洽Thread或HandlerThread, 謝謝。

AsyncTask內塞入有Looper的Thread, 也無法傳遞訊息給該執行緒。
如有上述情況, 要注意不能任意更改該Thread的Looper, 否則會很不方便阻塞其他Thread, 
如果更換Looper也會跳出RuntimeException, 真的要用Looper還是乖乖使用HandlerThread吧!
AsyncTask塞入Runnable, 這樣只是把AsyncTask當普通的Thread在使用,
優點: 如果AsyncTask內部已經存在Thread, 則會讓資源有效利用。
缺點: AsyncTask是全域環境則會干擾其他Thread運作。

