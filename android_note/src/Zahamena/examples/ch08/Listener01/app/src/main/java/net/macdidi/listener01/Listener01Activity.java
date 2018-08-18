package net.macdidi.listener01;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.MotionEvent;
import android.view.View;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

public class Listener01Activity extends AppCompatActivity
{

    private RelativeLayout relative;
    private TextView text, info;
    private EditText edit;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_listener01);

        processViews();
        processControllers();
    }

    private void processViews()
    {
        relative = (RelativeLayout) findViewById(R.id.relative);
        text = (TextView) findViewById(R.id.text);
        info = (TextView) findViewById(R.id.info);
        edit = (EditText) findViewById(R.id.edit);
    }

    private void processControllers()
    {
        // 註冊觸控事件
        relative.setOnTouchListener(new MyTouch01());
        // 註冊觸控事件
        text.setOnTouchListener(new MyTouch02());
        // 註冊焦點改變事件
        edit.setOnFocusChangeListener(new MyFocus());
    }

    // 觸控監聽類別
    private class MyTouch01 implements View.OnTouchListener
    {

        // 參數MotionEvent物件包裝觸控操作與資訊
        @Override
        public boolean onTouch(View v, MotionEvent event)
        {
            // 呼叫getAction方法取得觸控操作，
            //    MotionEvent.ACTION_DOWN是碰觸螢幕
            if (event.getAction() == MotionEvent.ACTION_DOWN) {
                Toast.makeText(Listener01Activity.this,
                               "LinearLayout DOWN",
                               Toast.LENGTH_SHORT).show();
            }

            // true表示事件不會往後傳遞
            // false表示事件會繼續傳遞給容器與Activity元件
            return false;
        }

    }

    // 觸控監聽類別
    private class MyTouch02 implements View.OnTouchListener
    {

        // 參數MotionEvent物件包裝觸控操作與資訊
        @Override
        public boolean onTouch(View v, MotionEvent event)
        {
            // 取得使用者操作種類
            int action = event.getAction();

            // 碰觸螢幕
            if (action == MotionEvent.ACTION_DOWN) {
                Toast.makeText(Listener01Activity.this,
                               "TextView DOWN",
                               Toast.LENGTH_SHORT).show();
            }
            // 移動
            else if (action == MotionEvent.ACTION_MOVE) {
                // 取得觸控數量
                final int pc = event.getPointerCount();

                StringBuffer sb = new StringBuffer();

                for (int i = 0; i < pc; i++) {
                    sb.append("point " + event.getPointerId(i) +
                              ": " + (int) event.getX(i) + "/" +
                              (int) event.getY(i));

                    if (i < pc - 1) {
                        sb.append("\n");
                    }
                }

                info.setText(sb.toString());
            }
            // 離開螢幕
            else if (action == MotionEvent.ACTION_UP) {
                Toast.makeText(Listener01Activity.this,
                               "TextView UP",
                               Toast.LENGTH_SHORT).show();
                info.setText("INFO");
            }

            // true表示事件不會往後傳遞
            // false表示事件會繼續傳遞給容器與Activity元件
            return true;
        }

    }

    // 焦點改變監聽類別
    private class MyFocus implements View.OnFocusChangeListener
    {

        // 參數boolean值表示取得（true）或離開（false）焦點
        @Override
        public void onFocusChange(View v, boolean hasFocus)
        {
            info.setText(hasFocus ? "EditText Focused" : "INFO");
        }

    }

}
