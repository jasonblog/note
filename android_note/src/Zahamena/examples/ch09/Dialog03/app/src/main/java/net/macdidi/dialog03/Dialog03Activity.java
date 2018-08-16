package net.macdidi.dialog03;

import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

import java.lang.ref.WeakReference;

public class Dialog03Activity extends AppCompatActivity
{

    private Button button01, button02;
    private ProgressDialog progressDialog;
    private static final int MAX = 100;

    // 建立一個測試進度對話框的Handler物件
    private Handler progressHandler = new MyHandler(this);

    // 宣告Handler類別，在後面的章節會詳細說明
    private static class MyHandler extends Handler
    {
        private final WeakReference<Dialog03Activity> dialog03Activity;

        public MyHandler(Dialog03Activity activity)
        {
            dialog03Activity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg)
        {
            Dialog03Activity activity = dialog03Activity.get();

            if (activity != null) {
                super.handleMessage(msg);

                if (activity.progressDialog.getProgress() >= MAX) {
                    // 已經處理完成，關閉對話框
                    activity.progressDialog.dismiss();
                    activity.button02.setText("DONE");
                } else {
                    // 增加進度對話框目前的進度
                    activity.progressDialog.incrementProgressBy(2);
                    // 0.1秒後再執行一次Handler
                    activity.progressHandler.sendEmptyMessageDelayed(0, 100);
                }
            }
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dialog03);

        processViews();
    }

    private void processViews()
    {
        button01 = (Button) findViewById(R.id.button01);
        button02 = (Button) findViewById(R.id.button02);
    }

    public void clickButton01(View view)
    {
        // 建立進度對話框物件
        ProgressDialog d = new ProgressDialog(Dialog03Activity.this);

        // 設定進度對話框的標題、訊息與可以取消
        d.setTitle("Download");
        d.setMessage("Please wait for download...");
        d.setCancelable(true);

        // 加入取消按鈕
        // 第一個參數是按鈕的種類
        // 第二個參數是按鈕的文字
        // 第一個參數是處理按鈕事件的監聽物件
        d.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel",
        new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                button01.setText("Cancel");
            }
        });

        // 顯示進度對話框
        d.show();
    }

    public void clickButton02(View view)
    {
        // 建立進度對話框物件
        progressDialog = new ProgressDialog(Dialog03Activity.this);

        // 設定進度對話框的標題、訊息與進度樣式
        progressDialog.setTitle("Download");
        progressDialog.setMessage("Please wait for download...");
        progressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);

        // 設定進度對話框的最大值為100
        progressDialog.setMax(MAX);
        // 設定進度對話框目前的進度
        progressDialog.setProgress(0);
        // 第一次執行Handler
        progressHandler.sendEmptyMessage(0);

        // 加入取消按鈕
        progressDialog.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel",
        new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                button02.setText("Cancel");
                progressHandler.removeMessages(0);
                progressDialog.setProgress(0);
            }
        });

        // 顯示進度對話框
        progressDialog.show();
    }

}
