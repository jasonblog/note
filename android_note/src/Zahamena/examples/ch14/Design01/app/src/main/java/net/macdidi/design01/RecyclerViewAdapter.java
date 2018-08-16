package net.macdidi.design01;

import android.annotation.TargetApi;
import android.content.Context;
import android.os.Build;
import android.support.design.widget.Snackbar;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

public class RecyclerViewAdapter
    extends RecyclerView.Adapter<RecyclerViewAdapter.ViewHolder>
{

    private Context context;
    // 使用的畫面配置資源編號
    private int layoutId;

    public RecyclerViewAdapter(Context context, int layoutId)
    {
        this.context = context;
        this.layoutId = layoutId;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType)
    {
        View view = LayoutInflater.from(parent.getContext())
                    .inflate(layoutId, parent, false);
        return new ViewHolder(view);
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @Override
    public void onBindViewHolder(final ViewHolder holder,
                                 final int position)
    {
        final View view = holder.mView;
        // 註冊項目點擊事件
        view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view,
                              "Item position: " + position,
                              Snackbar.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public int getItemCount()
    {
        // 設定為固定7個項目
        return 7;
    }

    public static class ViewHolder extends RecyclerView.ViewHolder
    {
        public final View mView;

        public ViewHolder(View view)
        {
            super(view);
            mView = view;
        }
    }

}
