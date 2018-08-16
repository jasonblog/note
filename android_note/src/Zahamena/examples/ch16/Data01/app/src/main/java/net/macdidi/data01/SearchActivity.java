package net.macdidi.data01;

import android.app.Activity;
import android.app.DatePickerDialog;
import android.app.DatePickerDialog.OnDateSetListener;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.DatePicker;
import android.widget.EditText;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class SearchActivity extends Activity
{

    private EditText date_search_edit;
    private SimpleDateFormat dateFormat =
        new SimpleDateFormat("yyyy-MM-dd");

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_search);

        processViews();
    }

    private void processViews()
    {
        date_search_edit = (EditText) findViewById(R.id.date_search_edit);

        // 設定為今天的日期
        Date date = new Date();
        date_search_edit.setText(dateFormat.format(date));
    }

    public void clickDateSearch(View view)
    {
        String dateValue = date_search_edit.getText().toString();
        Date date = new Date();

        try {
            // 轉換目前的日期為Date物件
            date = dateFormat.parse(dateValue);
        } catch (ParseException e) {

        }

        // 讀取日期物件中的年、月、日
        Calendar calendar = Calendar.getInstance();
        calendar.setTime(date);
        int year = calendar.get(Calendar.YEAR);
        int month = calendar.get(Calendar.MONTH);
        int day = calendar.get(Calendar.DAY_OF_MONTH);

        // 日期對話框設定監聽物件
        OnDateSetListener listener = new OnDateSetListener() {
            @Override
            public void onDateSet(DatePicker view,
                                  int year, int monthOfYear, int dayOfMonth) {
                // 設定畫面元件為設定的日期
                Calendar calendar = Calendar.getInstance();
                calendar.set(year, monthOfYear, dayOfMonth);
                date_search_edit.setText(dateFormat.format(calendar.getTime()));
            }
        };

        // 建立日期對話框物件
        DatePickerDialog dialog =
            new DatePickerDialog(this, listener, year, month, day);
        // 顯示日期對話框
        dialog.show();
    }

    public void clickOk(View view)
    {
        String dateValue = date_search_edit.getText().toString();

        Intent intent = getIntent();
        // 加入設定的日期資料
        intent.putExtra("dateValue", dateValue);
        // 設定回傳結果
        setResult(Activity.RESULT_OK, intent);
        // 結束
        finish();
    }

    public void clickCancel(View view)
    {
        // 結束
        finish();
    }

}