package net.macdidi.video01;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.MediaController;
import android.widget.VideoView;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

public class Video01Activity extends AppCompatActivity
{

    // 播放影片用的元件
    private VideoView video_view;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video01);

        // 讀取播放影片用的元件
        video_view = (VideoView) findViewById(R.id.video_view);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode,
                                    Intent data)
    {
        super.onActivityResult(requestCode, resultCode, data);

        // 如果使用者在錄影元件選擇確定按鈕
        if (resultCode == Activity.RESULT_OK) {
            // 取得影片檔案儲存位置與檔名的Uri物件
            Uri uri = data.getData();

            // 設定播放影片元件來源
            video_view.setVideoURI(uri);
            // 設定播放影片的時候裝置不要休眠
            video_view.setKeepScreenOn(true);
            // 建立控制物件
            MediaController controller = new MediaController(this);
            // 設定控制物件使用的VideoView物件
            controller.setMediaPlayer(video_view);
            // 設定VideoView使用控制物件
            video_view.setMediaController(controller);
        }
    }

    // 使用者選擇錄影按鈕
    public void clickRecord(View view)
    {
        // 取得儲存影片檔案的Uri物件
        Uri uri = getOutputFile();

        // 建立啟動錄影元件的Intent物件
        Intent intent = new Intent(MediaStore.ACTION_VIDEO_CAPTURE);
        // 設定儲存影片檔案的Uri物件
        intent.putExtra(MediaStore.EXTRA_OUTPUT, uri);
        // 設定影片品質，0為較低品質，1為較高品質
        intent.putExtra(MediaStore.EXTRA_VIDEO_QUALITY, 1);
        // 啟動錄影元件
        startActivityForResult(intent, 0);
    }

    // 使用者選擇播放按鈕
    public void clickPlay(View view)
    {
        // 開始播放影片
        video_view.start();
    }

    // 傳回儲存影片檔案的Uri物件
    private Uri getOutputFile()
    {

        // 建立公用照片的AnkaranaWork目錄物件
        File mediaStorageDir = new File(
            Environment.getExternalStoragePublicDirectory(
                Environment.DIRECTORY_PICTURES), "AnkaranaWork");

        // 如果沒有目錄的話就建立
        if (!mediaStorageDir.exists()) {
            if (!mediaStorageDir.mkdirs()) {
                Log.d("AnkaranaWork", "failed to create directory");
                return null;
            }
        }

        // 取得年月日_時分秒格式的字串
        String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss")
        .format(new Date());
        // 儲存影片檔案的完整目錄與檔名
        File mediaFile = new File(mediaStorageDir.getPath() +
                                  File.separator + "VID_" + timeStamp + ".mp4");
        // 除回包裝成Uri物件的目錄與檔名
        return Uri.fromFile(mediaFile);
    }

}
