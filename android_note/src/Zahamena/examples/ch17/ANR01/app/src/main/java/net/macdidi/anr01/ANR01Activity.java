package net.macdidi.anr01;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class ANR01Activity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_anr01);
    }

    public void sayHello(final View view)
    {
        int id = view.getId();

        if (id == R.id.button01) {
            // 宣告與建立執行緒物件，並啟動它
            new Thread() {
                public void run() {
                    try {
                        sleep(3000);
                    } catch (InterruptedException e) {
                        Log.d("ANR01Activity", e.toString());
                    }
                }
            } .start();

            // 沒有在執行緒中更新畫面元件
            ((Button) view).setText("DONE");
        } else if (id == R.id.button02) {
            // 宣告與建立執行緒物件，並啟動它
            new Thread() {
                public void run() {
                    try {
                        sleep(3000);
                    } catch (InterruptedException e) {
                        Log.d("ANR01Activity", e.toString());
                    }

                    // 在執行緒中更新畫面元件
                    ((Button) view).setText("DONE");
                }
            } .start();
        }
    }

}
