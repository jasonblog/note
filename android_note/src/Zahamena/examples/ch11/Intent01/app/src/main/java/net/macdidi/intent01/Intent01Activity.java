package net.macdidi.intent01;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class Intent01Activity extends AppCompatActivity
{

    private EditText account_edit, password_edit, age_edit;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_intent01);

        processViews();
    }

    public void clickOk(View view)
    {
        // 讀取使用者輸入的帳號、密碼與年齡
        String accountValue = account_edit.getText().toString();
        String passwordValue = password_edit.getText().toString();
        String ageStr = age_edit.getText().toString();

        // 把輸入的年齡轉換為int整數
        int ageValue = 0;

        try {
            ageValue = Integer.parseInt(ageStr);
        } catch (NumberFormatException e) {
            Toast.makeText(this, "Age must be number!",
                           Toast.LENGTH_LONG).show();
            return;
        }

        // 建立啟動SecondActivity元件的Intent物件
        Intent intent = new Intent(this, SecondActivity.class);

        // 建立Bundle物件
        Bundle bundle = new Bundle();

        // 把帳號、密碼與年齡資料放到Bundle物件中
        bundle.putString("accountValue", accountValue);
        bundle.putString("passwordValue", passwordValue);
        bundle.putInt("ageValue", ageValue);

        // 加入Bundle物件到Intent物件中
        intent.putExtras(bundle);

        // 把帳號、密碼與年齡資料放到Intent物件中
        intent.putExtra("accountValue", accountValue);
        intent.putExtra("passwordValue", passwordValue);
        intent.putExtra("ageValue", ageValue);

        // 啟動Intent物件指定的Activity元件
        startActivity(intent);
    }

    private void processViews()
    {
        account_edit = (EditText) findViewById(R.id.account_edit);
        password_edit = (EditText) findViewById(R.id.password_edit);
        age_edit = (EditText) findViewById(R.id.age_edit);
    }

}
