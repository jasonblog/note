package net.macdidi.broadcast01;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

public class Broadcast01Activity extends AppCompatActivity
{

    private EditText name_edit;
    private SeekBar age_seekbar;
    private TextView age_text;

    // 發送廣播事件用的Action名稱
    public static final String BROADCAST_ACTION =
        "net.macdidi.broadcast01.action.MYBROADCAST01";

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_broadcast01);

        processViews();
        processControllers();
    }

    public void clickBroadcastButton(View view)
    {
        // 讀取畫面元件輸入與設定的資料
        String nameValue = name_edit.getText().toString();
        int ageValue = age_seekbar.getProgress();

        // 建立準備發送廣播事件的Intent物件
        Intent intent = new Intent(BROADCAST_ACTION);
        // 設定資料到Intent物件
        intent.putExtra("name", nameValue);
        intent.putExtra("age", ageValue);
        // 發送廣播事件
        sendBroadcast(intent);
    }

    private void processViews()
    {
        name_edit = (EditText) findViewById(R.id.name_edit);
        age_seekbar = (SeekBar) findViewById(R.id.age_seekbar);
        age_text = (TextView) findViewById(R.id.age_text);
    }

    private void processControllers()
    {
        age_seekbar.setOnSeekBarChangeListener(
        new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(SeekBar seekBar,
                                          int progress,
                                          boolean fromUser) {
                age_text.setText(Integer.toString(progress));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }

        });
    }

}
