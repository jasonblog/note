package net.macdidi.intent01;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class ThirdActivity extends Activity
{

    private TextView account_text03, password_text03, age_text03;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_third);

        processViews();

        // 取得Intent物件
        Intent intent = getIntent();

        // 讀取Intent物件中包裝所有資料的Bundle物件
        Bundle bundle = intent.getExtras();

        // 如果Intent物件中有包含資料
        if (bundle != null) {
            // 讀取在Bundle物件中的帳號、密碼與年齡資料
            String accountValue = bundle.getString("accountValue");
            String passwordValue = bundle.getString("passwordValue");
            int ageValue = bundle.getInt("ageValue");

            // 把帳號、密碼與年齡資料設定給畫面元件
            account_text03.setText(accountValue);
            password_text03.setText(passwordValue);
            age_text03.setText(Integer.toString(ageValue));
        }
    }

    public void clickButton(View view)
    {
        finish();
    }

    private void processViews()
    {
        account_text03 = (TextView) findViewById(R.id.account_text03);
        password_text03 = (TextView) findViewById(R.id.password_text03);
        age_text03 = (TextView) findViewById(R.id.age_text03);
    }

}
