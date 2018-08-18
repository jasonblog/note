package net.macdidi.radiobutton01;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.RadioButton;
import android.widget.Toast;

public class RadioButton01Activity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_radio_button01);
    }

    public void selectMovie(View view)
    {
        // View是所有畫面元件的父類別，所以可以把它轉型為RadioButton
        RadioButton checkedRadio = (RadioButton)view;
        // 呼叫元件的getText方法可以傳回String值，代表元件的文字
        Toast.makeText(this,
                       checkedRadio.getText(), Toast.LENGTH_SHORT).show();
    }

}
