package net.macdidi.activitylife001;

import android.app.Activity;
import android.os.Bundle;
import android.view.MotionEvent;

public class ThirdActivity extends Activity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_third);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        int action = event.getAction();

        if (action == MotionEvent.ACTION_DOWN) {
            finish();
        }

        return true;
    }

}
