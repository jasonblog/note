package net.macdidi.materialtran01;

import android.app.Activity;
import android.app.ActivityOptions;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

// 只可以繼承 Activity 類別，不可以繼支援套件的 AppCompatActivity 類別
public class MaterialTran01Activity extends Activity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_material_tran01);
    }

    public void clickButton(View view)
    {
        Intent intent = new Intent(this, ApiLevelActivity.class);
        // 使用動畫資源啟動元件
        startActivity(intent,
                      ActivityOptions.makeSceneTransitionAnimation(
                          MaterialTran01Activity.this).toBundle());
    }

}
