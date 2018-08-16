package net.macdidi.hellofriends;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

// 提供資料給 RecylerView 使用的 Adapter 物件
//   繼承 RecyclerView.Adapter 類別
public class FriendAdapter
    extends RecyclerView.Adapter<FriendAdapter.ViewHolder>
{

    // 儲存項目資料的 List 物件
    private List<Friend> items;

    private Context context;

    public FriendAdapter(Context context)
    {
        items = new ArrayList<>();
        this.context = context;
    }

    @Override
    public ViewHolder onCreateViewHolder(
        ViewGroup parent, int viewType)
    {
        // 建立項目使用的畫面配置元件
        View v = LayoutInflater.from(parent.getContext()).inflate(
                     R.layout.cardview_item, parent, false);
        // 建立與回傳包裝好的畫面配置元件
        ViewHolder viewHolder = new ViewHolder(v);
        return viewHolder;
    }

    // 使用 ViewHolder 包裝項目使用的畫面元件
    public class ViewHolder extends RecyclerView.ViewHolder
    {

        // 郵件帳號與是否分享
        protected TextView email_text;
        protected ImageView online_image;

        // 包裝元件
        protected View rootView;

        public ViewHolder(View view)
        {
            super(view);

            // 使用父類別 ViewHolder 宣告的「itemView」欄位變數，
            //   取得郵件帳號與是否分享元件
            email_text = (TextView) itemView.findViewById(R.id.email_text);
            online_image = (ImageView) itemView.findViewById(R.id.online_image);

            // 設定包裝元件
            rootView = view;
        }

    }

    @Override
    public void onBindViewHolder(final ViewHolder holder,
                                 final int position)
    {
        // 讀取目前位置的項目物件
        final Friend friend = items.get(position);

        // 設定郵件帳號與是否分享
        holder.email_text.setText(friend.getEmail());
        holder.online_image.setImageResource(
            friend.isOnline() ? R.drawable.ic_star_black_32dp
            : R.drawable.ic_star_border_black_32dp);
    }

    @Override
    public int getItemCount()
    {
        return items.size();
    }

    // 新增一個項目
    public void add(Friend friend)
    {
        items.add(friend);
        // 通知資料項目已經新增
        notifyItemInserted(items.size());
    }

    // 刪除一個項目
    public void remove(int position)
    {
        items.remove(position);
        // 通知資料項目已經刪除
        notifyItemRemoved(position);
    }

    // 刪除一個項目
    public void remove(Friend friend)
    {
        int position = items.indexOf(friend);

        if (position != -1) {
            remove(position);
        }
    }

    public void update(Friend friend)
    {
        int position = items.indexOf(friend);

        if (position != -1) {
            items.set(position, friend);
            notifyItemChanged(position);
        }
    }

    public Friend get(int position)
    {
        return items.get(position);
    }

}
