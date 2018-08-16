package net.macdidi.design01;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

public class Page01Fragment extends Fragment
{

    private RecyclerView recyclerView;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState)
    {
        // 讀取 RecylerView 物件
        recyclerView = (RecyclerView) inflater.inflate(
                           R.layout.recyler_view_fragment, container, false);
        return recyclerView;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);

        // 設定畫面配置元件
        recyclerView.setLayoutManager(
            new LinearLayoutManager(recyclerView.getContext()));
        // 設定 RecylerView 使用的 Adapter 物件
        recyclerView.setAdapter(new RecyclerViewAdapter(
                                    getActivity(), R.layout.view_pager01));
    }

}
