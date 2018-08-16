package net.macdidi.pref02;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

import java.util.HashSet;
import java.util.Set;

public class Pref02Activity extends AppCompatActivity
{

    private EditText name_edit;
    private TextView amount_text;
    private EditText weeks_edit;
    private SeekBar amount_seekbar;
    private EditText ringtone_edit;
    private CheckBox alarm_check;

    public static final String KEY_NAME = "KEY_NAME";
    public static final String KEY_AMOUNT = "KEY_AMOUNT";
    public static final String KEY_WEEKS = "KEY_WEEKS";
    public static final String KEY_RINGTONE = "KEY_RINGTONE";
    public static final String KEY_ALARM = "KEY_ALARM";

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pref02);

        processViews();
    }

    @Override
    protected void onResume()
    {
        super.onResume();

        // 讀取資料與設定畫面元件
        restorePref();
    }

    public void clickPref(View view)
    {
        // 啟動設定元件
        Intent intent = new Intent(this, PrefActivity.class);
        startActivity(intent);
    }

    public void clickCancel(View view)
    {
        // 結束
        finish();
    }

    // 讀取資料與設定畫面元件
    private void restorePref()
    {
        // 建立準備讀取資料用的SharedPreferences物件
        SharedPreferences sp =
            PreferenceManager.getDefaultSharedPreferences(this);

        // 讀取名稱、數量、星期、VIP、音效與鬧鈴資料
        String name = sp.getString(KEY_NAME, "");
        String amountStr = sp.getString(KEY_AMOUNT, "0");
        int amount = Integer.parseInt(amountStr);
        Set<String> weeks = sp.getStringSet(KEY_WEEKS, new HashSet<String>());
        String ringtone = sp.getString(KEY_RINGTONE, "");
        boolean alarm = sp.getBoolean(KEY_ALARM, false);

        // 設定畫面元件
        name_edit.setText(name);
        amount_seekbar.setProgress(amount);
        amount_text.setText(amountStr);
        weeks_edit.setText(weeks.toString());
        ringtone_edit.setText(ringtone);
        alarm_check.setChecked(alarm);
    }

    private void processViews()
    {
        name_edit = (EditText) findViewById(R.id.name_edit);
        amount_seekbar = (SeekBar) findViewById(R.id.amount_seekbar);
        weeks_edit = (EditText) findViewById(R.id.weeks_edit);
        amount_text = (TextView) findViewById(R.id.amount_text);
        ringtone_edit = (EditText) findViewById(R.id.ringtone_edit);
        alarm_check = (CheckBox) findViewById(R.id.alarm_check);

        amount_seekbar.setEnabled(false);
    }

}
