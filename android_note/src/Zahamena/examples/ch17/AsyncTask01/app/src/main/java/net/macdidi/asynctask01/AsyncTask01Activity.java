package net.macdidi.asynctask01;

import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;

import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.List;

public class AsyncTask01Activity extends AppCompatActivity
{

    private ImageView imageview;
    private LinearLayout op_panel;

    private Bitmap[] images;
    // 目前畫面顯示的圖片編號
    private int position = 0;

    // 顯示下載進度用的進度對話框
    private ProgressDialog progressDialog;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_async_task01);

        processViews();
    }

    public void clickDownLoadButton(View view)
    {
        // 讓下載按鈕消失
        findViewById(R.id.download_btn).setVisibility(View.INVISIBLE);

        // 建立下載圖片的AsyncTask物件
        final DownloadImageTask downloadImageTask = new DownloadImageTask();

        // 建立與設定進度對話框
        progressDialog = new ProgressDialog(this);
        progressDialog.setTitle("Download");
        progressDialog.setMessage("Please wait for download...");
        progressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
        progressDialog.setProgress(0);
        progressDialog.setCancelable(false);

        // 加入取消工作的按鈕
        progressDialog.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel",
        new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                // 取消AsyncTask的工作，參數指定為true，表示取消正在執行的工作
                // 呼叫這個方法的AsyncTask物件，在結束doInBackground方法後，
                // 不會呼叫onPostExecute方法，改為呼叫onCancelled方法
                downloadImageTask.cancel(true);
            }
        });

        // 顯示進度對話框
        progressDialog.show();

        // 啟動AsyncTask物件
        downloadImageTask.execute();
    }

    // 宣告下載圖片的AsyncTask類別，AsyncTask泛型的指定：
    // 第一個型態是啟動工作資料，doInBackground方法的參數型態
    // 第二個型態是處理進度資料，onProgressUpdate方法的參數型態
    // 第三個型態是結束工作回傳資料，onPostExecute方法的參數型態
    private class DownloadImageTask extends AsyncTask<Void, Integer, Void>
    {

        // 儲存下載圖片的List物件
        private List<Bitmap> downloads = new ArrayList<>();

        // 呼叫這個物件的execute方法後就會執行這個方法
        // 參數是AsyncTask泛型指定的第一個型態的陣列
        @Override
        protected Void doInBackground(Void... args)
        {
            // 讀取陣列資源，下載的圖片名稱
            Resources res = getResources();
            String[] imageNames =
                res.getStringArray(R.array.android_icons_array);

            // 讀取儲存圖片的網路位置
            String baseUrl = getString(R.string.base_url);

            // 設定進度對話框的最大進度數量
            progressDialog.setMax(imageNames.length);

            for (int i = 0; i < imageNames.length; i++) {
                // 判斷是否取消工作
                if (isCancelled()) {
                    break;
                }

                // 下載圖片的完整網址
                String url = baseUrl + imageNames[i];
                // 從網際網路下載圖片
                downloads.add(loadBitmap(url));
                // 執行進度處理，參數會傳送給onProgressUpdate方法
                publishProgress(i + 1);
            }

            // 如果已經下載圖片
            if (downloads.size() > 0) {
                // 建立儲存圖片的Bitmap陣列
                images = new Bitmap[downloads.size()];
                // 把List物件轉換為陣列
                downloads.toArray(images);
            }

            return null;
        }

        // 執行進度處理，參數是AsyncTask泛型指定的第二個型態的陣列
        // 在doInBackground方法中，呼叫publishProgress方法後，
        // 就會執行這個方法並接收放在publishProgress方法中的參數
        @Override
        protected void onProgressUpdate(Integer... values)
        {
            // 設定進度對話框的進度
            progressDialog.setProgress(values[0]);
            // 設定ImageView使用的ImageView物件
            imageview.setImageBitmap(downloads.get(downloads.size() - 1));
        }

        // doInBackground方法結束後就會執行這個方法
        // 參數是AsyncTask泛型指定的第三個型態的陣列
        // 接收doInBackground方法的回傳值
        @Override
        protected void onPostExecute(Void result)
        {
            // 結束進度對話框
            progressDialog.dismiss();
            // 設定ImageView使用的ImageView物件
            imageview.setImageBitmap(images[0]);
            // 顯示上下張圖片的操作按鈕
            op_panel.setVisibility(View.VISIBLE);
        }

        // 呼叫cancel方法後執行這個方法
        // 參數是AsyncTask泛型指定的第三個型態的陣列
        // 接收doInBackground方法的回傳值
        @Override
        protected void onCancelled(Void result)
        {
            if (images != null) {
                // 設定ImageView使用的ImageView物件
                imageview.setImageBitmap(images[0]);
                // 顯示上下張圖片的操作按鈕
                op_panel.setVisibility(View.VISIBLE);
            }
        }
    }

    // 控制顯示的圖片往前與往後一個
    public void clickMoveButton(View view)
    {
        int id = view.getId();

        if (id == R.id.previous) {
            position--;
        } else if (id == R.id.next) {
            position++;
        }

        if (position >= images.length) {
            position = 0;
        }

        if (position < 0) {
            position = images.length - 1;
        }

        imageview.setImageBitmap(images[position]);
    }

    private void processViews()
    {
        imageview = (ImageView) findViewById(R.id.imageview);
        op_panel = (LinearLayout) findViewById(R.id.op_panel);
    }

    // 從參數指定的網址下載一個圖片並傳回Bitmap物件
    private Bitmap loadBitmap(String URL)
    {
        Bitmap bitmap = null;
        InputStream in = null;

        try {
            // 取得指定網址的InputStream物件
            in = openHttpConnection(URL);
            // 從InputStream物件讀取圖片轉換為Bitmap物件
            bitmap = BitmapFactory.decodeStream(in);
        } catch (IOException e) {
            Log.d("Thread01Activity", e.toString());
        }

        finally {
            try
            {
                if (in != null) {
                    in.close();
                }
            } catch (IOException e)
            {
                Log.d("Thread01Activity", e.toString());
            }
        }

        return bitmap;
    }

    // 取得參數指定網址的InputStream物件
    private InputStream openHttpConnection(String strURL)
    throws IOException {
        InputStream inputStream = null;
        // 建立參數指定網址的URL物件
        URL url = new URL(strURL);
        // 建立參數指定網址的URLConnection物件
        URLConnection conn = url.openConnection();

        try
        {
            // 把URLConnection轉型為HttpURLConnection
            HttpURLConnection httpConn = (HttpURLConnection) conn;
            // 設定為HTTP GET請求方式
            httpConn.setRequestMethod("GET");
            // 連線
            httpConn.connect();

            // 如果連線成功
            if (httpConn.getResponseCode() == HttpURLConnection.HTTP_OK) {
                // 取得連線的InputStream物件
                inputStream = httpConn.getInputStream();
            }
        } catch (Exception e)
        {
            Log.d("Thread01Activity", e.toString());
        }

        return inputStream;
    }

}
