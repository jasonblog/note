package net.macdidi.material02;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.CheckBox;
import android.widget.TextView;

import java.util.Collections;
import java.util.List;

// 提供資料給 RecyclerView 使用的 Adapter 物件
//   繼承 RecyclerView.Adapter 類別，指定泛型為 FruidAdapter.ViewHolder
//   ItemTouchHelperAdapter: 資料項目操作介面
public class FruitAdapter
    extends RecyclerView.Adapter<FruitAdapter.ViewHolder>
    implements ItemTouchHelperAdapter
{

    // 儲存項目資料的 List 物件
    private List<Fruit> items;

    // 儲存最後一個項目的位置
    private int lastPosition = -1;
    private Context context;

    public FruitAdapter(List<Fruit> items, Context context)
    {
        this.items = items;
        this.context = context;
    }

    @Override
    public ViewHolder onCreateViewHolder(
        ViewGroup parent, int viewType)
    {
        // 建立項目使用的畫面配置元件
        View v = LayoutInflater.from(parent.getContext()).inflate(
                     R.layout.cardview_item01, parent, false);
        // 建立與回傳包裝好的畫面配置元件
        ViewHolder viewHolder = new ViewHolder(v);
        return viewHolder;
    }

    // 使用 ViewHolder 包裝項目使用的畫面元件
    public class ViewHolder extends RecyclerView.ViewHolder
    {

        // 編號、名稱、說明與是否選擇
        protected TextView id_text;
        protected TextView name_text;
        protected TextView content_text;
        protected CheckBox selected_check;

        // 包裝元件
        protected View rootView;

        public ViewHolder(View view)
        {
            super(view);

            // 使用父類別 ViewHolder 宣告的「itemView」欄位變數，
            //   取得編號、名稱、說明與是否選擇元件
            id_text = (TextView) itemView.findViewById(R.id.id_text);
            name_text = (TextView) itemView.findViewById(R.id.name_text);
            content_text = (TextView)
                           itemView.findViewById(R.id.content_text);
            selected_check = (CheckBox)
                             itemView.findViewById(R.id.selected_check);

            // 設定包裝元件
            rootView = view;
        }

    }

    @Override
    public void onBindViewHolder(final ViewHolder holder,
                                 final int position)
    {
        // 讀取目前位置的項目物件
        final Fruit fruit = items.get(position);

        // 設定編號、名稱、說明與是否選擇
        holder.id_text.setText(Long.toString(fruit.getId()));
        holder.name_text.setText(fruit.getName());
        holder.content_text.setText(fruit.getContent());
        holder.selected_check.setChecked(fruit.isSelected());

        // 設定動畫效果
        setAnimation(holder.rootView, position);

        // 建立與註冊是否選擇元件的點擊監聽物件
        holder.selected_check.setOnClickListener(
        new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // 設定目前位置項目物件的是否選擇狀態
                fruit.setSelected(holder.selected_check.isChecked());
            }
        }
        );
    }

    // 設定動畫效果
    private void setAnimation(View view, int position)
    {
        // 如果是最後一個項目
        if (position > lastPosition) {
            // 建立 Animation 動畫物件, 指定效果為由左方滑入
            Animation animation = AnimationUtils.loadAnimation(
                                      context, android.R.anim.slide_in_left);
            // 啟動動畫效果
            view.startAnimation(animation);
            // 儲存最後一個項目的位置
            lastPosition = position;
        }
    }

    @Override
    public int getItemCount()
    {
        return items.size();
    }

    // 新增一個項目
    public void add(Fruit fruit)
    {
        items.add(fruit);
        // 通知資料項目已經新增
        notifyItemInserted(items.size());
    }

    // 刪除一個項目
    public void remove(int position)
    {
        items.remove(position);
        // 通知資料項目已經刪除
        notifyItemRemoved(position);
        notifyItemRangeChanged(position, items.size());
    }

    // 實作 ItemTouchHelperAdapter 介面的方法
    // 左右滑動項目
    @Override
    public void onItemDismiss(int position)
    {
        // 刪除項目
        remove(position);
    }

    // 實作 ItemTouchHelperAdapter 介面的方法
    // 長按並移動項目
    @Override
    public void onItemMove(int fromPosition, int toPosition)
    {
        if (fromPosition < toPosition) {
            // 如果是往下拖拉
            for (int i = fromPosition; i < toPosition; i++) {
                Collections.swap(items, i, i + 1);
            }
        } else {
            // 如果是往上拖拉
            for (int i = fromPosition; i > toPosition; i--) {
                Collections.swap(items, i, i - 1);
            }
        }

        // 通知資料項目已經移動
        notifyItemMoved(fromPosition, toPosition);
    }

}
