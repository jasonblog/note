package net.macdidi.pref01;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

public class Pref01Activity extends AppCompatActivity
{

    private EditText name_edit;
    private SeekBar amount_seekbar;
    private TextView amount_text;
    private CheckBox vip_check;

    // 宣告名稱、數量與VIP資料名稱變數
    private static final String KEY_NAME = "KEY_NAME";
    private static final String KEY_AMOUNT = "KEY_AMOUNT";
    private static final String KEY_VIP = "KEY_VIP";

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pref01);

        processViews();
        processControllers();

        // 讀取資料與設定畫面元件
        restorePref();
    }

    public void clickOk(View view)
    {
        // 儲存設定資料
        savePref();
        finish();
    }

    // 儲存設定資料
    private void savePref()
    {
        // 讀取畫面元件名稱、數量與VIP的資料
        String nameValue = name_edit.getText().toString();
        int amountValue = amount_seekbar.getProgress();
        boolean vipValue = vip_check.isChecked();

        // 建立準備儲存資料用的SharedPreferences物件
        SharedPreferences sp =
            PreferenceManager.getDefaultSharedPreferences(this);
        // 取得儲存資料用的SharedPreferences.Editor物件
        SharedPreferences.Editor editor = sp.edit();

        // 儲存名稱、數量與VIP資料
        editor.putString(KEY_NAME, nameValue);
        editor.putInt(KEY_AMOUNT, amountValue);
        editor.putBoolean(KEY_VIP, vipValue);

        // 寫入
        editor.apply();
    }

    // 讀取資料與設定畫面元件
    private void restorePref()
    {
        // 建立準備讀取資料用的SharedPreferences物件
        SharedPreferences sp =
            PreferenceManager.getDefaultSharedPreferences(this);
        // 讀取名稱、數量與VIP資料
        String name = sp.getString(KEY_NAME, "");
        int amount = sp.getInt(KEY_AMOUNT, 0);
        boolean vip = sp.getBoolean(KEY_VIP, false);

        // 設定畫面元件
        name_edit.setText(name);
        amount_seekbar.setProgress(amount);
        vip_check.setChecked(vip);
    }

    private void processViews()
    {
        name_edit = (EditText) findViewById(R.id.name_edit);
        amount_seekbar = (SeekBar) findViewById(R.id.amount_seekbar);
        amount_text = (TextView) findViewById(R.id.amount_text);
        vip_check = (CheckBox) findViewById(R.id.vip_check);

        amount_seekbar.setProgress(0);
    }

    private void processControllers()
    {
        // 顯示設定數量
        amount_seekbar.setOnSeekBarChangeListener(
        new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(SeekBar seekBar,
                                          int progress, boolean fromUser) {
                amount_text.setText(Integer.toString(progress));
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
