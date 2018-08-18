package net.macdidi.listview04;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.List;

public class FruitAdapter extends ArrayAdapter<Fruit>
{

    // 項目使用的畫面配置資源
    private int layoutId;
    // 所有項目資料
    private List<Fruit> items;

    // 第一個參數Context物件，表示使用這個類別的Activity元件
    // 第二個參數是項目使用的畫面配置資源
    // 第三個參數是所有項目資料
    public FruitAdapter(Context context, int layoutId, List<Fruit> items)
    {
        // 呼叫父類別建構子，執行必要的設定
        super(context, layoutId, items);
        this.layoutId = layoutId;
        this.items = items;
    }

    // 至少要覆寫「getView」方法，在方法中製作與回傳代表項目畫面的View物件
    // 第一個參數是項目的編號，第一個項目是0
    // 第二個參數是代表項目的View物件
    // 第三個參數是ListView物件
    @Override
    public View getView(int position, View convertView, ViewGroup parent)
    {
        // 取得使用這個類別的Activity元件
        Context context = getContext();

        // 因為項目的畫面使用LinearLayout，所以宣告一個LinearLayout變數
        LinearLayout view;

        // ListView需要建立新的項目
        if (convertView == null) {
            // 建立一個新的View物件
            view = new LinearLayout(context);
            // 使用LayoutInflater把畫面配置檔轉換為View物件
            String inflater = Context.LAYOUT_INFLATER_SERVICE;
            LayoutInflater vi = (LayoutInflater)
                                context.getSystemService(inflater);
            vi.inflate(layoutId, view, true);
        }
        // ListView需要修改原有的項目
        else {
            view = (LinearLayout) convertView;
        }

        // 取得畫面配置檔中的畫面元件
        TextView id_text = (TextView) view.findViewById(R.id.id_text);
        TextView name_text = (TextView) view.findViewById(R.id.name_text);
        TextView content_text = (TextView) view.findViewById(R.id.content_text);
        CheckBox selected_check = (CheckBox) view.findViewById(R.id.selected_check);

        // 取得目前項目的Fruit物件
        Fruit fruit = items.get(position);

        // 設定畫面元件顯示的內容
        id_text.setText(Long.toString(fruit.getId()));
        name_text.setText(fruit.getName());
        content_text.setText(fruit.getContent());
        selected_check.setChecked(fruit.isSelected());

        return view;
    }

    // 增加一個修改項目資料的方法宣告
    // 參數是位置與修改項目資料的Fruit物件
    public void set(int index, Fruit fruit)
    {
        if (index >= 0 && index < items.size()) {
            // 修改指定位置的Fruit物件
            items.set(index, fruit);
            // 呼叫父類別ArrayAdapter的方法，通知項目資料已改變
            notifyDataSetChanged();
        }
    }

}
