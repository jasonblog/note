package net.macdidi.listener02;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

public class Listener02Activity extends AppCompatActivity
{

    private TextView info;
    private CheckBox check01, check02, check03;
    private RadioGroup radio_panel;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_listener02);

        processViews();
        processControllers();
    }

    private void processViews()
    {
        info = (TextView) findViewById(R.id.info);
        check01 = (CheckBox) findViewById(R.id.check01);
        check02 = (CheckBox) findViewById(R.id.check02);
        check03 = (CheckBox) findViewById(R.id.check03);

        radio_panel = (RadioGroup) findViewById(R.id.radio_panel);
    }

    private void processControllers()
    {
        // 建立選擇監聽物件
        MyChecked myChecked = new MyChecked();
        // 註冊監聽物件
        check01.setOnCheckedChangeListener(myChecked);
        check02.setOnCheckedChangeListener(myChecked);
        check03.setOnCheckedChangeListener(myChecked);

        // 註冊監聽物件
        radio_panel.setOnCheckedChangeListener(new MyRadio());
    }

    // 選擇監聽類別
    private class MyChecked
        implements CompoundButton.OnCheckedChangeListener
    {

        // CompoundButton參數是使用者操作的元件
        // boolean參數是選擇狀態
        @Override
        public void onCheckedChanged(CompoundButton buttonView,
                                     boolean isChecked)
        {
            if (isChecked) {
                info.setText(buttonView.getText() + ": checked");
            } else {
                info.setText(buttonView.getText() + ": unchecked");
            }
        }

    }

    // RadioGroup選擇監聽類別
    private class MyRadio implements RadioGroup.OnCheckedChangeListener
    {

        // RadioGroup參數是使用者操作的群組
        // int參數是使用者操作的RadioButton元件資源編號
        @Override
        public void onCheckedChanged(RadioGroup group, int checkedId)
        {
            // 取得RadioButton元件物件
            RadioButton rb = (RadioButton) findViewById(checkedId);
            info.setText(rb.getText() + ": checked");
        }

    }

}
