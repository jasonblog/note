package com.demo.asus.fileobserver;

import android.app.Activity;

import android.os.Bundle;

import android.util.Log;
import android.view.View;
import android.widget.Button;


/**
 * Created by ASUS on 2016/5/23.
 */
public class MonitorActivity extends Activity
{

    //public static String MONITOR_TAG = "monitor_debug";
    private String path = null;
    private Button btn_stop = null;
    private RecursiveFileObserver observer = null;

    ;
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        Bundle extras = getIntent().getExtras();

        if (extras != null) {
            path = extras.getString(MainActivity.EXTAR_PATH);
        } else {
            Log.d("C&C", "observer path is null");
        }

        btn_stop = (Button)findViewById(R.id.btn_stop);

        if (btn_stop == null) {

        }

        btn_stop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                observer.stopWatching();
            }
        });


        observer = new RecursiveFileObserver(path);
        observer.startWatching();

    }

    /*
    public void log(String msg) {
        Log.d(MONITOR_TAG, msg);
        if (Environment.getExternalStorageState()
                .equals(Environment.MEDIA_MOUNTED)) {
            //存在sd卡
            String sd_path = Environment.getExternalStorageDirectory().getPath();
            Calendar c = Calendar.getInstance();
            SimpleDateFormat time = new SimpleDateFormat("yyyy-MM-dd");
            String strDay = time.format(c.getTime());
            String logFileName = sd_path + "/FileObserverDemo/log/"+strDay+".txt";
            MyLog log = new MyLog(logFileName);
            /*
            try {
                File f = new File(logFileName);
                if (f.exists()) {


                } else {//文件不存在
                    if (f.getParentFile().mkdir()) {
                        //创建父目录
                        if (f.createNewFile()) {
                            Log.d(MONITOR_TAG, "CCCC");
                        }
                    } else {
                        Log.d(MONITOR_TAG, "CCCC");
                    }

                }

            } catch (Exception e) {
                e.printStackTrace();
            }

        } else {

        }
    }
    */

}
