package net.macdidi.data01;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class InsertActivity extends Activity
{

    private EditText latitude_edit;
    private EditText longitude_edit;
    private EditText accuracy_edit;
    private EditText note_edit;

    private PlaceDAO placeDAO;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_insert);

        processViews();

        // 取得資料庫物件
        placeDAO = new PlaceDAO(this);
    }

    private void processViews()
    {
        latitude_edit = (EditText) findViewById(R.id.latitude_edit);
        longitude_edit = (EditText) findViewById(R.id.longitude_edit);
        accuracy_edit = (EditText) findViewById(R.id.accuracy_edit);
        note_edit = (EditText) findViewById(R.id.note_edit);
    }

    public void clickOk(View view)
    {
        // 讀取使用者輸入的資料
        double latitudeValue = Double.parseDouble(latitude_edit.getText().toString());
        double longitudeValue = Double.parseDouble(longitude_edit.getText().toString());
        double accuracyValue = Double.parseDouble(accuracy_edit.getText().toString());
        String note = note_edit.getText().toString();

        // 建立準備新增資料的物件
        Place place = new Place();

        // 把讀取的資料設定給物件
        place.setLatitude(latitudeValue);
        place.setLongitude(longitudeValue);
        place.setAccuracy(accuracyValue);
        place.setDatetime(System.currentTimeMillis());
        place.setNote(note);

        // 新增
        place = placeDAO.insert(place);

        // 顯示新增成功
        Toast.makeText(this, "Insert success!", Toast.LENGTH_SHORT).show();

        Intent intent = getIntent();
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