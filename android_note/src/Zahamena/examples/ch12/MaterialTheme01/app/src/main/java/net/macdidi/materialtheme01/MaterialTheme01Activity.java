package net.macdidi.materialtheme01;

import android.app.Activity;
import android.os.Bundle;

// 只可以繼承 Activity 類別，不可以繼支援套件的 AppCompatActivity 類別
public class MaterialTheme01Activity extends Activity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_material_theme01);
    }
}
