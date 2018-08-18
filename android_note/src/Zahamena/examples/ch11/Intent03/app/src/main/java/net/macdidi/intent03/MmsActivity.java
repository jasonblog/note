package net.macdidi.intent03;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class MmsActivity extends AppCompatActivity
{

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_intent03);

        // 取得ListView物件
        ListView listview = (ListView)findViewById(R.id.listview);

        // 建立指定陣列資源的ArrayAdapter物件
        ArrayAdapter<CharSequence> adapterActions =
            ArrayAdapter.createFromResource(
                this, R.array.mms_menu_array,
                android.R.layout.simple_list_item_1);

        // 設定ArrayAdapter物件給ListView物件
        listview.setAdapter(adapterActions);

        // 項目選擇事件
        listview.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                                    int position, long id) {

                Intent intent = new Intent();

                switch (position) {
                case 0:
                    // 建立傳送簡訊的Uri物件，包含電話號碼
                    Uri uri = Uri.parse("smsto:0933111222");

                    // 設定Uri物件
                    intent.setData(uri);

                    // 設定訊息內容
                    intent.putExtra("sms_body", "This is SMS test.");

                    // 設定Action名稱為傳送簡訊
                    intent.setAction(Intent.ACTION_SENDTO);

                    // 啟動發送簡訊元件
                    startActivity(intent);
                    break;

                case 1:
                    // 收件人、主旨與內容
                    String[] emailAddrs = {"ali@gmail.com"};
                    String subject = "EMail test";
                    String content = "This is EMail test";

                    // 設定Action名稱為傳送郵件
                    intent.setAction(Intent.ACTION_SEND);

                    // 設定郵件內容格式、收件人、主旨與內容
                    intent.setType("plain/text");
                    intent.putExtra(Intent.EXTRA_EMAIL, emailAddrs);
                    intent.putExtra(Intent.EXTRA_SUBJECT, subject);
                    intent.putExtra(Intent.EXTRA_TEXT, content);

                    // 啟動發送郵件元件
                    startActivity(Intent.createChooser(intent, "Send EMail"));
                    break;
                }
            }

        });
    }

}
