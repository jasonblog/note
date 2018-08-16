package net.macdidi.broadcast02;

import android.app.Service;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

// 繼承自Service的服務元件，在裝置開機完成後就要啟動它
public class BootCompletedService extends Service
{

    // 倒數用的Handler物件
    private CountDownTask countDownTask;

    // 倒數次數
    private int countDown = 10;

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    @Override
    public void onCreate()
    {
        super.onCreate();

        // 建立倒數用的AsyncTask物件
        countDownTask = new CountDownTask();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        // 啟動AsyncTask工作
        countDownTask.execute();
        return Service.START_STICKY;
    }

    @Override
    public void onDestroy()
    {
        // 停止服務的時後取消AsyncTask工作
        countDownTask.cancel(true);
        super.onDestroy();
    }

    // 宣告倒數用的AsyncTask類別
    private class CountDownTask extends AsyncTask<Void, Integer, Void>
    {

        @Override
        protected Void doInBackground(Void... args)
        {
            for (int i = countDown; i >= 0; i--) {
                // 判斷是否取消工作
                if (isCancelled()) {
                    break;
                }

                // 處理進度
                publishProgress(i);

                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    Log.d("BootCompletedService", e.toString());
                }
            }

            return null;
        }

        @Override
        protected void onProgressUpdate(Integer... values)
        {
            // 顯示倒數訊息框
            makeText("BootService: " + values[0]);
        }

        @Override
        protected void onPostExecute(Void result)
        {
            // 顯示結束訊息框
            makeText("BootService: End");
            // 停止服務
            stopSelf();
        }

        @Override
        protected void onCancelled(Void result)
        {
            // 顯示取消訊息框
            makeText("BootService: Cancel");
        }

        private void makeText(String message)
        {
            Toast.makeText(getApplicationContext(),
                           message, Toast.LENGTH_SHORT).show();
        }
    }

}
