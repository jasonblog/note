package net.macdidi.res01;

import android.content.DialogInterface;
import android.content.res.Resources;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class Res01Activity extends AppCompatActivity
{

    private Button button02;
    private Button button03;
    private Button button04;
    private Button button05;
    private Button button06;

    private String[] items;
    private boolean[] selection;
    private int choice = -1;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        // 設定畫面配置資源
        // 「R」是ADT自動產生的Java類別，裡面包含所有資源名稱
        // 「layout」是資源種類，layout表示放在layout目錄下的畫面配置資源
        // 「activity_res01」是資源名稱，也就是檔案名稱，不包含副檔名「.xml」
        setContentView(R.layout.activity_res01);

        processViews();

        // 取得資源物件
        Resources r = getResources();
        // 讀取陣列資源給對話框的選項使用
        items = r.getStringArray(R.array.dialog_item_array);
        // 建立多選對話框需要的boolean陣列，元素個數跟選項一樣
        selection = new boolean[items.length];
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        // 取得載入選單用的MenuInflater物件
        MenuInflater menuInflater = getMenuInflater();
        // 呼叫inflate方法載入指定的選單資源，第二個參數是這個方法的Menu物件
        menuInflater.inflate(R.menu.main, menu);
        // 回傳true選單才會顯示
        return true;
    }

    // 參數MenuItem是使用者選擇的選單項目物件
    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        Toast.makeText(this, item.getTitle(), Toast.LENGTH_SHORT).show();

        // 取得選單項目的資源編號
        int id = item.getItemId();

        switch (id) {
        case R.id.main_menu_add:
            // 執行這個選單項目的工作
            break;

        case R.id.main_menu_delete:
            //
            break;

        case R.id.main_menu_help:
            //
            break;
        }

        return super.onOptionsItemSelected(item);
    }

    private void processViews()
    {
        button02 = (Button) findViewById(R.id.button02);
        button03 = (Button) findViewById(R.id.button03);
        button04 = (Button) findViewById(R.id.button04);
        button05 = (Button) findViewById(R.id.button05);
        button06 = (Button) findViewById(R.id.button06);
    }

    public void clickButton01(View view)
    {
        // 建立對話框物件並指定樣式
        AlertDialog.Builder d = new AlertDialog.Builder(
            Res01Activity.this, android.R.style.Theme_Material_Light_Dialog_Alert);
        // 設定標題為文字資源
        d.setTitle(R.string.dialog01_title_text);
        // 設定訊息為文字資源
        d.setMessage(R.string.dialog01_message_text);
        // 顯示對話框
        d.show();
    }

    public void clickButton02(View view)
    {
        // 建立對話框物件
        AlertDialog.Builder d = new AlertDialog.Builder(Res01Activity.this);
        // 設定標題、訊息與不可取消
        d.setTitle(R.string.dialog02_title_text)
        .setMessage(R.string.dialog02_message_text)
        .setCancelable(false);

        // 加入按鈕
        d.setPositiveButton(R.string.dialog02_button_yes_text,
        new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                button02.setText(R.string.dialog02_button_yes_text);
            }
        });

        // 加入按鈕
        d.setNeutralButton(R.string.dialog02_button_maybe_text,
        new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                button02.setText(R.string.dialog02_button_maybe_text);
            }
        });

        // 加入按鈕
        d.setNegativeButton(R.string.dialog02_button_no_text,
        new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                button02.setText(R.string.dialog02_button_no_text);
            }
        });

        // 顯示對話框
        d.show();
    }

    public void clickButton03(View view)
    {
        // 建立對話框物件
        AlertDialog.Builder d = new AlertDialog.Builder(Res01Activity.this);
        // 設定標題與不可取消
        d.setTitle(R.string.select_text)
        .setCancelable(false);

        // 設定選擇項目
        // 第一個參數為提供選項的字串陣列
        // 第二個參數為選擇選項後的事件控制
        d.setItems(items, new DialogInterface.OnClickListener() {
            // int值參數為操作的項目編號
            public void onClick(DialogInterface dialog, int which) {
                button03.setText(items[which]);
            }
        });

        // 顯示對話框
        d.show();
    }

    public void clickButton04(View view)
    {
        // 建立對話框物件
        AlertDialog.Builder d = new AlertDialog.Builder(Res01Activity.this);
        // 設定標題
        d.setTitle(R.string.select_text);

        // 設定單選項目
        // 第一個參數為提供選項的字串陣列
        // 第二個參數為對畫框出現時的預設選項，-1為沒有選擇
        // 第三個參數為選擇選項後的事件控制
        d.setSingleChoiceItems(items, choice,
        new DialogInterface.OnClickListener() {
            // int值參數為操作的項目編號
            public void onClick(DialogInterface dialog, int which) {
                choice = which;
            }
        });

        // 加入確認選擇的按鈕
        d.setPositiveButton(android.R.string.ok,
        new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                // choice就是使用者選擇的項目編號，還是-1的話表示沒有選擇
                if (choice != -1) {
                    button04.setText(items[choice]);
                }
            }
        });

        // 加入按鈕，事件指定為null值，表示不作任何事情，只會關閉對話框
        d.setNegativeButton(android.R.string.cancel, null);

        // 顯示對話框
        d.show();
    }

    public void clickButton05(View view)
    {
        // 建立對話框物件
        AlertDialog.Builder d = new AlertDialog.Builder(Res01Activity.this);
        // 設定標題
        d.setTitle(R.string.select_text);

        // 設定多選項目
        // 第一個參數為提供選項的字串陣列
        // 第二個參數為對畫框出現時的預設選項
        // 第三個參數為選擇選項後的事件控制
        d.setMultiChoiceItems(items, selection,
        new DialogInterface.OnMultiChoiceClickListener() {
            // int值參數為操作的項目編號
            // boolean值參數為是否勾選
            public void onClick(DialogInterface dialog, int which,
                                boolean isChecked) {
                selection[which] = isChecked;
            }
        });

        // 加入確認按鈕
        d.setPositiveButton(android.R.string.ok,
        new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                StringBuilder sb = new StringBuilder();

                for (int i = 0; i < selection.length; i++) {
                    if (selection[i]) {
                        sb.append(items[i] + " ");
                    }
                }

                button05.setText(sb.toString());
            }
        });

        // 顯示對話框
        d.show();
    }

    public void clickButton06(View view)
    {
        // 取得載入畫面配置資源用的物件
        LayoutInflater inflater = getLayoutInflater();
        // 載入對話框使用的畫面配置資源
        final View dialogView =
            inflater.inflate(R.layout.dialog_signin, null);
        // 建立對話框物件
        AlertDialog.Builder d =
            new AlertDialog.Builder(Res01Activity.this);
        // 設定對話框使用的畫面配置資源與標題
        d.setView(dialogView)
        .setTitle(R.string.dialog06_title_text);

        // 加入登入按鈕
        d.setPositiveButton("Signin",
        new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                // 讀取帳號與密碼元件
                EditText username = (EditText)
                                    dialogView.findViewById(R.id.username);
                EditText password = (EditText)
                                    dialogView.findViewById(R.id.password);
                String usernameValue = username.getText().toString();
                String passwordValue = password.getText().toString();

                // 讀取文字資源
                String simonUsername = getString(R.string.simon_username);
                String simonPassword = getString(R.string.simon_password);

                // 取得Resources物件
                Resources r = getResources();

                if (!(usernameValue.equals(simonUsername) &&
                      passwordValue.equals(simonPassword))) {
                    // 讀取顏色資源
                    int color = ContextCompat.getColor(
                                    Res01Activity.this, R.color.failure_text_color);
                    button06.setTextColor(color);

                    // 讀取尺寸資源
                    float dimen = r.getDimension(R.dimen.failure_text_size);
                    button06.setTextSize(dimen);

                    button06.setText(R.string.signin_failure);
                } else {
                    // 讀取顏色資源
                    int color = ContextCompat.getColor(
                                    Res01Activity.this, R.color.buttons_text_color);
                    button06.setTextColor(color);

                    // 讀取尺寸資源
                    float dimen = r.getDimension(R.dimen.buttons_text_size);
                    button06.setTextSize(dimen);

                    button06.setText(R.string.signin_success);
                }
            }
        });

        // 加入取消按鈕
        d.setNegativeButton(android.R.string.cancel,
        new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                button06.setText(android.R.string.cancel);
            }
        });

        // 顯示對話框
        d.show();
    }

}
