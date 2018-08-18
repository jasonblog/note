package net.macdidi.checkbox01;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.CheckBox;
import android.widget.Toast;

public class CheckBox01Activity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_check_box01);
    }

    public void isReady(View view)
    {
        // View是所有畫面元件的父類別，可以把它轉型為CheckBox
        CheckBox ready = (CheckBox)view;
        // CheckBox的isChecked方法可以傳回boolean值，代表是否勾選
        String message = ready.isChecked() ? "Yes" : "No";
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

}
