package net.macdidi.intent01;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

public class SecondActivity extends AppCompatActivity
{

    private TextView account_text02, password_text02, age_text02;

    private String accountValue, passwordValue;
    private int ageValue;

    private Bundle bundle;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_second);

        processViews();

        // 取得Intent物件
        Intent intent = getIntent();

        // 讀取Intent物件中包裝所有資料的Bundle物件
        bundle = intent.getExtras();

        // 讀取在Intent物件中的帳號、密碼與年齡資料
        accountValue = intent.getStringExtra("accountValue");
        passwordValue = intent.getStringExtra("passwordValue");
        ageValue = intent.getIntExtra("ageValue", -1);

        account_text02.setText(accountValue);
        password_text02.setText(passwordValue);
        age_text02.setText(Integer.toString(ageValue));
    }

    public void clickOk(View view)
    {
        // 設定準備啟動ThirdActivity元件的Action名稱
        String action = "net.macdidi.intent01.ACTION.THIRD";

        // 建立指定Action名稱的Intent物件
        Intent intent = new Intent(action);

        // 把包裝所有資料的Bundle放到Intent物件中
        intent.putExtras(bundle);

        // 啟動Action名稱指定的元件
        startActivity(intent);
    }

    public void clickCancel(View view)
    {
        finish();
    }

    private void processViews()
    {
        account_text02 = (TextView) findViewById(R.id.account_text02);
        password_text02 = (TextView) findViewById(R.id.password_text02);
        age_text02 = (TextView) findViewById(R.id.age_text02);
    }
}
