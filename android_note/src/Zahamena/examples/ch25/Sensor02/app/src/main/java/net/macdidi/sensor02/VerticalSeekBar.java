package net.macdidi.sensor02;

import android.content.Context;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.SeekBar;

// 繼承自 SeekBar 類別，實作一個垂直樣式的 SeekBar 元件
public class VerticalSeekBar extends SeekBar
{

    public VerticalSeekBar(Context context)
    {
        super(context);
    }

    public VerticalSeekBar(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
    }

    public VerticalSeekBar(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }

    protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {
        super.onSizeChanged(h, w, oldh, oldw);
    }

    @Override
    protected synchronized void onMeasure(int widthMeasureSpec,
                                          int heightMeasureSpec)
    {
        super.onMeasure(heightMeasureSpec, widthMeasureSpec);
        setMeasuredDimension(getMeasuredHeight(), getMeasuredWidth());
    }

    protected void onDraw(Canvas canvas)
    {
        // 讓元件畫面向左旋轉90度
        canvas.rotate(-90);
        canvas.translate(-getHeight(), 0);

        super.onDraw(canvas);
    }

    @Override
    public synchronized void setProgress(int progress)
    {
        super.setProgress(progress);
        onSizeChanged(getWidth(), getHeight(), 0, 0);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        if (!isEnabled()) {
            return false;
        }

        switch (event.getAction()) {
        case MotionEvent.ACTION_DOWN:
        case MotionEvent.ACTION_MOVE:
        case MotionEvent.ACTION_UP:
            setProgress(getMax() -
                        (int)(getMax() * event.getY() / getHeight()));
            onSizeChanged(getWidth(), getHeight(), 0, 0);
            break;

        case MotionEvent.ACTION_CANCEL:
            break;
        }

        return true;
    }

}
