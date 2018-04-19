package com.tengmuz.jnidemo;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    static{
        //System.loadLibrary("库文件名称不带前缀，后缀名");
        System.loadLibrary("hello-jni");
    }

    public native String hellojni();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        String str = hellojni();
        Toast.makeText(this,str,Toast.LENGTH_SHORT).show();

    }

}
