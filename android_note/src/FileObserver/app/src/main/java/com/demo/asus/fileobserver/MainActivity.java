package com.demo.asus.fileobserver;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Process;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import java.io.File;

public class MainActivity extends Activity
{

    public static String EXTAR_PATH = "path_to_monitor";

    private Button btnStart = null;
    private EditText editPath = null;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btnStart = (Button) findViewById(R.id.btn_start);
        editPath = (EditText) findViewById(R.id.text_path);

        if (editPath != null) {
            /*一次将/proc 目录下全加上会崩溃，文件太多*/
            // editPath.setText("/proc/"+getPid());
            // editPath.setText("/tmp/1"); //test
            editPath.setText("/proc/" + getPid() + "/status");
        }

        if (btnStart != null) {
            btnStart.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    String path = editPath.getText().toString();
                    handlePath(path);
                }
            });
        }

    }


    public void handlePath(String path)
    {
        if (path.isEmpty()) {
            Toast.makeText(this, "the path is empty", Toast.LENGTH_SHORT).show();
            return;
        }

        try {
            File f = new File(path);

            if (f.exists()) {
                int requestCode = 0;
                Intent i = new Intent("android.intent.action.MONITOR");
                i.putExtra(EXTAR_PATH, path);
                startActivityForResult(i, requestCode);
            }
        } catch (RuntimeException e) {
            Toast.makeText(this, "the file is not exist", Toast.LENGTH_SHORT).show();
            return;
        }
    }


    private static String getPid()
    {
        return String.format("%d", Process.myPid());
    }
}
