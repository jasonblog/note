package net.macdidi.fragment03;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;

public class Fragment03Activity extends AppCompatActivity
{

    private PlaceFragment place_fragment;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fragment03);

        // 為了支援Support Library，
        // 所以呼叫getSupportFragmentManager方法，
        // 取得FragmentManager物件
        FragmentManager manager = getSupportFragmentManager();
        // 取得PlaceFragment物件
        place_fragment = (PlaceFragment)
                         manager.findFragmentById(R.id.place_fragment);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode,
                                    Intent data)
    {
        // 如果執行確定
        if (resultCode == Activity.RESULT_OK) {
            // 呼叫Fragment物件的onActivityResult方法
            place_fragment.onActivityResult(requestCode, resultCode, data);
        }
    }

}
