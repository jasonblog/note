package net.macdidi.audio01;

import android.Manifest;
import android.content.pm.PackageManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaRecorder;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;
import java.util.HashSet;
import java.util.Set;

public class Audio01Activity extends AppCompatActivity
{

    // 控制撥放音樂進度的元件
    private SeekBar control;

    // 多媒體撥放元件
    private MediaPlayer mediaPlayer;
    // 更新播放進度的AsyncTask物件
    private MyPlayTask myPlayTask;

    // 錄音按鈕、錄音狀態元件
    private ImageButton record_button;
    private ProgressBar record_volume;

    // 是否正在錄音
    private boolean isRecording = false;

    // 執行錄音工作的物件
    private MyRecorder myRecorder;

    // 寫入外部儲存與錄音設備設備授權請求代碼
    private static final int REQUEST_PERMISSION = 100;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio01);

        processViews();
        processControllers();

        // 建立指定資源的MediaPlayer物件
        mediaPlayer = MediaPlayer.create(this, R.raw.sound01);

        // 更新播放進度的AsyncTask物件
        myPlayTask = new MyPlayTask();
        // 啟動顯示播放進度的AsyncTask物件
        myPlayTask.execute();

        // 設定SeekBar元件的最大值為音樂的總時間（毫秒）
        control.setMax(mediaPlayer.getDuration());

        // 註冊播放完成監聽事件
        mediaPlayer.setOnCompletionListener(new OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer player) {
                // 回到開始的位置
                mediaPlayer.seekTo(0);
                control.setProgress(0);
            }
        });
    }

    public void onStop()
    {
        // 取消更新播放進度的AsyncTask物件
        myPlayTask.cancel(true);

        // 停止撥放
        if (mediaPlayer.isPlaying()) {
            mediaPlayer.stop();
        }

        // 清除資源
        mediaPlayer.release();

        super.onStop();
    }

    private void processViews()
    {
        control = (SeekBar) findViewById(R.id.control);
        record_button = (ImageButton) findViewById(R.id.record_button);
        record_volume = (ProgressBar) findViewById(R.id.record_volume);
    }

    private void processControllers()
    {
        // 註冊SeekBar元件進度改變事件
        control.setOnSeekBarChangeListener(
        new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar,
                                          int progress,
                                          boolean fromUser) {
                // 一定要判斷是使用者的操作，因為播放過程也會更改進度
                if (fromUser) {
                    // 移動音樂到指定的進度
                    mediaPlayer.seekTo(progress);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
    }

    public void clickButton01(View view)
    {
        // 建立播放應用程式音樂資源用的MediaPlayer物件
        MediaPlayer player = MediaPlayer.create(this, R.raw.sound02);
        // 註冊播放完成監聽事件
        player.setOnCompletionListener(new MyCompletion());
        // 開始播放
        player.start();
    }

    public void clickButton02(View view)
    {
        // 建立系統預設通知音效的Uri物件
        Uri uri = Settings.System.DEFAULT_NOTIFICATION_URI;
        // 建立播放Uri指定音樂用的MediaPlayer物件
        MediaPlayer player = MediaPlayer.create(this, uri);
        //        player.setLooping(false);
        // 註冊播放完成事件
        player.setOnCompletionListener(new MyCompletion());
        // 開始播放
        player.start();
    }

    public void clickPlay(View view)
    {
        // 開始播放
        mediaPlayer.start();
    }

    public void clickPause(View view)
    {
        // 暫停播放
        mediaPlayer.pause();
    }

    // 使用者選擇錄音按鈕
    public void clickRecord(View view)
    {
        // 處理授權
        requestPermission();
    }

    public void clickRecordPlay(View view)
    {
        // 取得外部儲存裝置路徑（SD Card）
        File extDir = Environment.getExternalStorageDirectory();
        File record = new File(extDir, "/Audio01Activity.mp3");

        // 如果錄音檔案存在
        if (record.exists()) {
            // 建立外部儲存裝置下的音樂檔案Uri物件
            Uri uri = Uri.parse(record.getAbsolutePath());
            // 建立播放Uri指定音樂用的MediaPlayer物件
            MediaPlayer player = MediaPlayer.create(this, uri);
            // 註冊播放完成監聽事件
            player.setOnCompletionListener(new MyCompletion());
            // 開始播放
            player.start();
        } else {
            Toast.makeText(this, record.getAbsolutePath() +
                           " not exist.", Toast.LENGTH_LONG).show();
        }
    }

    // 播放完成監聽類別
    private class MyCompletion implements OnCompletionListener
    {
        @Override
        public void onCompletion(MediaPlayer player)
        {
            // 清除MediaPlayer物件
            player.release();
        }
    }

    // 在播放過程中顯示播放進度
    private class MyPlayTask extends AsyncTask<Void, Void, Void>
    {

        @Override
        protected Void doInBackground(Void... args)
        {
            while (!isCancelled()) {

                publishProgress();

                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    Log.d("Audio01Activity", e.toString());
                }
            }

            return null;
        }

        @Override
        protected void onProgressUpdate(Void... values)
        {
            // 設定播放進度
            control.setProgress(mediaPlayer.getCurrentPosition());
        }

    }

    // 執行錄音工作的類別
    private class MyRecorder
    {

        private MediaRecorder recorder = null;
        private String output;

        // 建立錄音物件，參數為錄音儲存的位置與檔名
        MyRecorder(String output)
        {
            this.output = output;
        }

        // 開始錄音
        public void start()
        {
            if (recorder == null) {
                // 建立錄音用的MediaRecorder物件
                recorder = new MediaRecorder();
                // 設定錄音來源為麥克風，必須在setOutputFormat方法之前呼叫
                recorder.setAudioSource(MediaRecorder.AudioSource.MIC);
                // 設定輸出格式為3GP壓縮格式，必須在setAudioSource方法之後，
                // 在prepare方法之前呼叫
                recorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
                // 設定錄音的編碼方式，必須在setOutputFormat方法之後，
                // 在prepare方法之前呼叫
                recorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
                // 設定輸出的檔案名稱，必須在setOutputFormat方法之後，
                // 在prepare方法之前呼叫
                recorder.setOutputFile(output);

                try {
                    // 準備執行錄音工作，必須在所有設定之後呼叫
                    recorder.prepare();
                } catch (IOException e) {
                    Log.d("Audio01Activity", e.toString());
                }

                // 開始錄音
                recorder.start();
            }
        }

        // 停止錄音
        public void stop()
        {
            if (recorder != null) {
                // 停止錄音
                recorder.stop();
                // 清除錄音資源
                recorder.release();
                recorder = null;
            }
        }
    }

    private void requestPermission()
    {
        // 如果裝置版本是6.0（包含）以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 儲存授權名稱
            Set<String> permSet = new HashSet<>();

            // 讀取外部儲存與錄音設備的授權狀態
            int hasStoragePermission = checkSelfPermission(
                                           Manifest.permission.WRITE_EXTERNAL_STORAGE);
            int hasRecordPermission = checkSelfPermission(
                                          Manifest.permission.RECORD_AUDIO);

            // 如果還沒有外部儲存設備授權
            if (hasStoragePermission != PackageManager.PERMISSION_GRANTED) {
                permSet.add(Manifest.permission.WRITE_EXTERNAL_STORAGE);
            }

            // 如果還沒有錄音設備授權
            if (hasRecordPermission != PackageManager.PERMISSION_GRANTED) {
                permSet.add(Manifest.permission.RECORD_AUDIO);
            }

            // 如果需要執行授權
            if (permSet.size() > 0) {
                // 轉換需要的授權為字串陣列
                String[] permArray = permSet.toArray(new String[] {});
                // 請求授權
                //     第一個參數是請求授權的名稱
                //     第二個參數是請求代碼
                requestPermissions(permArray, REQUEST_PERMISSION);
                return;
            }
        }

        // 如果裝置版本是6.0以下，
        // 或是裝置版本是6.0（包含）以上，使用者已經授權，
        // 執行錄音
        processRecord();
    }

    // 覆寫請求授權後執行的方法
    //   第一個參數是請求代碼
    //   第二個參數是請求授權的名稱
    //   第三個參數是請求授權的結果，PERMISSION_GRANTED或PERMISSION_DENIED
    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String[] permissions,
                                           int[] grantResults)
    {
        // 如果是寫入外部儲存與錄音設備設備授權請求
        if (requestCode == REQUEST_PERMISSION) {
            // 如果在授權請求全部選擇「允許」
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED &&
                grantResults[1] == PackageManager.PERMISSION_GRANTED) {
                // 執行錄音
                processRecord();
            }
            // 如果在授權請求選擇「拒絕」
            else {
                // 顯示沒有授權的訊息
                Toast.makeText(this, "沒有授權",
                               Toast.LENGTH_SHORT).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    private void processRecord()
    {
        isRecording = !isRecording;

        // 錄音中
        if (isRecording) {
            // 設定按鈕圖示為錄音中
            record_button.setImageResource(R.drawable.ic_mic_off_black_48dp);
            // 取得外部儲存裝置路徑
            File file = Environment.getExternalStorageDirectory();
            // 建立錄音物件，儲存檔名為外部儲存裝置下的Audio01Activity.mp3
            myRecorder = new MyRecorder(file.getAbsolutePath() +
                                        "/Audio01Activity.mp3");
            // 開始錄音
            myRecorder.start();

            // 顯示開始錄音
            record_volume.setIndeterminate(true);
        }
        // 停止錄音
        else {
            // 設定按鈕圖示為停止錄音
            record_button.setImageResource(R.drawable.ic_mic_black_48dp);
            // 停止錄音
            myRecorder.stop();
            // 顯示停止錄音
            record_volume.setIndeterminate(false);
        }
    }

}
