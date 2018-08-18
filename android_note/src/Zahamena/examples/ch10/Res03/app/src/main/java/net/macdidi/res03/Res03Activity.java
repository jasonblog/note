package net.macdidi.res03;

import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.Spinner;

public class Res03Activity extends AppCompatActivity
{

    private Spinner spinner;
    private ImageView imageview;

    private TypedArray icons;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_res03);

        processViews();
        processControllers();

        // 建立一個項目為陣列資源的Adapter物件
        // 第一個參數設定「this」，表示這個Activity元件
        // 第二個參數設定陣列資源
        // 第三個參數設定畫面配置資源
        ArrayAdapter<CharSequence> aa =
            ArrayAdapter.createFromResource(
                this, R.array.android_icons,
                android.R.layout.simple_spinner_item);
        // 呼叫setAdapter方法指定使用的Adapter物件
        spinner.setAdapter(aa);

        // 取得資源物件
        Resources r = getResources();
        // 取得包裝圖形陣列資源的TypedArray物件
        icons = r.obtainTypedArray(R.array.android_icons);
    }

    // 控制顯示的圖形往前與往後一個
    public void clickButton(View view)
    {
        int id = view.getId();
        int now = spinner.getSelectedItemPosition();

        if (id == R.id.previous) {
            now--;
        } else if (id == R.id.next) {
            now++;
        }

        if (now >= spinner.getCount()) {
            now = 0;
        }

        if (now < 0) {
            now = spinner.getCount() - 1;
        }

        spinner.setSelection(now);
    }

    private void processViews()
    {
        spinner = (Spinner) findViewById(R.id.spinner);
        imageview = (ImageView) findViewById(R.id.imageview);
    }

    private void processControllers()
    {
        spinner.setOnItemSelectedListener(
        new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view,
                                       int position, long id) {
                // 讀取圖形陣列資源中指定位置的Drawable物件
                Drawable icon = icons.getDrawable(position);
                // 把Drawable物件設定給ImageView元件
                imageview.setImageDrawable(icon);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
    }

}
