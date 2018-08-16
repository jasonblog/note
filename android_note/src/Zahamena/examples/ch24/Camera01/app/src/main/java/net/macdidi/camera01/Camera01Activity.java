package net.macdidi.camera01;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.v4.content.FileProvider;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

public class Camera01Activity extends AppCompatActivity
{

    // 啟動照相應用程式用的請求代碼，全圖與縮圖
    private static final int TAKE_PICTURE_BIG = 0;
    private static final int TAKE_PICTURE_SMALL = 1;

    // 保存畫面照片物件的資料名稱
    private static final String BITMAP_KEY = "BITMAP_KEY";
    // 儲存顯示在畫面上的照片物件
    private Bitmap bitmap;

    // 全圖照片檔案的目錄與檔名
    private String bigPictureFileName;

    // 顯示照片用的ImageView元件
    private ImageView image_view;

    // 請求授權使用外部儲存設備的請求代碼
    private static final int REQUEST_STORAGE_PERMISSION = 100;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera01);

        image_view = (ImageView) findViewById(R.id.image_view);
    }

    @Override
    protected void onSaveInstanceState(Bundle bundle)
    {
        // 裝置改變方向後，結束元件前先儲存目前畫面上的圖形物件
        bundle.putParcelable(BITMAP_KEY, bitmap);
        super.onSaveInstanceState(bundle);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState)
    {
        super.onRestoreInstanceState(savedInstanceState);

        // 裝置改變方向後，重新啟動元件時先讀取之前儲存的圖形物件
        bitmap = savedInstanceState.getParcelable(BITMAP_KEY);
        // 把讀取的圖形物件設定給ImageView元件
        image_view.setImageBitmap(bitmap);
    }

    // 完成拍照工作
    @Override
    protected void onActivityResult(int requestCode, int resultCode,
                                    Intent data)
    {
        // 如果使用者在相機元件選擇確定
        if (resultCode == Activity.RESULT_OK) {
            switch (requestCode) {
            // 取得全圖
            case TAKE_PICTURE_BIG:
                if (bigPictureFileName != null) {
                    // 讀取與顯示照片檔案
                    pictureToImageView(bigPictureFileName, image_view);
                    // 加入系統相簿
                    addGallery(bigPictureFileName);
                    bigPictureFileName = null;
                }

                break;

            // 取得縮圖
            case TAKE_PICTURE_SMALL:
                // 讀取縮圖Bitmap物件
                Bundle extras = data.getExtras();
                bitmap = (Bitmap) extras.get("data");
                // 把縮圖設定給ImageView元件顯示
                image_view.setImageBitmap(bitmap);
                break;
            }
        }
    }

    // 啟動照相元件並取得全圖
    public void clickBigButton(View view)
    {
        // 請求授權
        requestStoragePermission();
    }

    // 啟動照相元件並取得縮圖
    public void clickSmallButton(View view)
    {
        // 建立啟動相機元件的Intent物件
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        // 啟動照相元件
        startActivityForResult(intent, TAKE_PICTURE_SMALL);
    }

    private File getImageFile()
    {
        File result = null;

        // 判斷儲存設備是否可以讀寫
        if (Environment.MEDIA_MOUNTED.equals(Environment
                                             .getExternalStorageState())) {

            File storage;

            // 判斷版本是否為Froyo
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.FROYO) {
                // 取得相片儲存目錄
                storage = Environment.getExternalStoragePublicDirectory(
                              Environment.DIRECTORY_PICTURES);
            }
            // 如果是Froyo以前的版本
            else {
                // 取得相片儲存目錄
                storage = new File(Environment.getExternalStorageDirectory()
                                   + "/dcim/");
            }

            // 使用年月日_時分秒格式為檔案名稱
            SimpleDateFormat sdf = new SimpleDateFormat(
                "yyyyMMdd_HHmmss", Locale.getDefault());
            String timeStamp = sdf.format(new Date());

            // 加入前置和後置檔名
            String fileName = "IMG_" + timeStamp + ".jpg";

            result = new File(storage, fileName);
        } else {
            Log.d("Camera01Activity", "External Storage NOT MOUNTED!");
        }

        return result;
    }

    // 通知系統加入參數指定的照片檔案
    private void addGallery(String fileName)
    {
        // 準備通知系統的Intent物件
        Intent intent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
        // 建立參數指定的File物件
        File file = new File(fileName);
        // 建立參數指定的Uri物件
        Uri uri = Uri.fromFile(file);
        // 設定資料
        intent.setData(uri);
        // 發出廣播事件，由系統接收後把指定的照片加入系統相簿
        sendBroadcast(intent);
    }

    // 讀取第一個參數指定的照片檔案，轉換為第二個ImageView元件的大小後，
    // 載入並顯示，這樣在顯示大型照片檔案的時候，可以節省很多資源
    private void pictureToImageView(String fileName, ImageView imageView)
    {
        // 取得ImageView元件在畫面上的寬與高
        int targetWidth = imageView.getWidth();
        int targetHeight = imageView.getHeight();

        // 建立調整照片用的Options物件
        BitmapFactory.Options options = new BitmapFactory.Options();
        // 設定為只讀取大小的資訊
        options.inJustDecodeBounds = true;
        // 讀取指定照片檔案的資訊到Options物件
        BitmapFactory.decodeFile(fileName, options);

        // 取得照片檔案的寬與高
        int pictureWidth = options.outWidth;
        int pictureHeight = options.outHeight;
        // 比較ImageView和照片的大小後計算縮小比例
        int scaleFactor = Math.min(pictureWidth / targetWidth,
                                   pictureHeight / targetHeight);

        // 取消只讀取大小的資訊的設定
        options.inJustDecodeBounds = false;
        // 設定縮小比例
        options.inSampleSize = scaleFactor;

        // 使用建立好的設定載入照片檔案
        Bitmap bitmap = BitmapFactory.decodeFile(fileName, options);
        // 設定給參數指定的ImageView元件
        imageView.setImageBitmap(bitmap);
    }

    private void requestStoragePermission()
    {
        // 如果裝置版本是6.0（包含）以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 取得授權狀態，參數是請求授權的名稱
            int hasPermission = checkSelfPermission(
                                    Manifest.permission.READ_EXTERNAL_STORAGE);

            // 如果未授權
            if (hasPermission != PackageManager.PERMISSION_GRANTED) {
                // 請求授權
                //     第一個參數是請求授權的名稱
                //     第二個參數是請求代碼
                requestPermissions(
                    new String[] {Manifest.permission.READ_EXTERNAL_STORAGE},
                    REQUEST_STORAGE_PERMISSION);
                return;
            }
        }

        // 如果裝置版本是6.0以下，
        // 或是裝置版本是6.0（包含）以上，使用者已經授權，
        // 執行全圖拍照
        processImages();
    }

    // 使用者完成授權的選擇以後，Android會呼叫這個方法
    //     第一個參數：請求授權代碼
    //     第二個參數：請求的授權名稱
    //     第三個參數：使用者選擇授權的結果
    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String[] permissions,
                                           int[] grantResults)
    {
        // 如果是使用儲存設備授權請求
        if (requestCode == REQUEST_STORAGE_PERMISSION) {
            // 如果在授權請求選擇「允許」
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // 執行全圖拍照
                processImages();
            }
            // 如果在授權請求選擇「拒絕」
            else {
                // 顯示沒有授權的訊息
                Toast.makeText(this, "沒有使用儲存設備授權",
                               Toast.LENGTH_SHORT).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions,
                                             grantResults);
        }
    }

    // 執行全圖拍照
    private void processImages()
    {
        // 建立啟動相機元件的Intent物件
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);

        // 取得檔案位置與名稱，參數指定相簿目錄名稱
        File file = getImageFile();
        // 設定全圖照片檔案名稱
        bigPictureFileName = file.getAbsolutePath();

        // 建立檔案儲存位置的Uri物件
        Uri uri = null;

        if (Build.VERSION.SDK_INT > 21) {
            uri = FileProvider.getUriForFile(this,
                                             BuildConfig.APPLICATION_ID + ".provider",
                                             file);
        } else {
            uri = Uri.fromFile(file);
        }

        // 設定全圖照片檔案儲存的位置
        intent.putExtra(MediaStore.EXTRA_OUTPUT, uri);

        // 啟動照相元件
        startActivityForResult(intent, TAKE_PICTURE_BIG);
    }

    // 轉換媒體資訊為實際檔案位置與名稱
    private String getUriPath(Uri uri)
    {
        String result = null;
        String scheme = uri.getScheme();

        if (scheme.equals("content")) {
            Cursor cursor = getContentResolver().query(uri, null, null,
                            null, null);

            if (cursor.moveToFirst()) {
                int column_index = cursor.getColumnIndexOrThrow(
                                       MediaStore.Images.Media.DATA);
                result = cursor.getString(column_index);
            }

            cursor.close();
        }

        return result;
    }

}
