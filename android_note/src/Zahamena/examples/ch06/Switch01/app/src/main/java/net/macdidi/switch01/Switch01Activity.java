package net.macdidi.switch01;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.Toast;

public class Switch01Activity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_switch01);
    }

    public void isOpen(View view)
    {
        // View是所有畫面元件的父類別，所以可以把它轉型為CompoundButton
        CompoundButton open = (CompoundButton)view;
        // CompoundButton的isChecked方法可以傳回boolean值，代表開或關
        String message = open.isChecked() ? "OPEN" : "CLOSE";
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }
}
