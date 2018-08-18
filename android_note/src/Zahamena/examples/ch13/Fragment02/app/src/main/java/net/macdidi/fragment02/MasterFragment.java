package net.macdidi.fragment02;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

//繼承自Support Library的Fragment類別
public class MasterFragment extends Fragment
{

    // 顯示項目資料用的ListView元件
    private ListView fruit_list;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState)
    {
        // 載入指定的畫面配置資源
        View result = inflater.inflate(R.layout.fragment_master, container,
                                       false);
        // 讀取與設定畫面元件
        fruit_list = (ListView) result.findViewById(R.id.fruit_list);

        return result;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);

        // 建立ListView使用的Adapter物件，
        // 第一個參數需要Context物件，所以要呼叫getActivity()
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
            getActivity(), android.R.layout.simple_list_item_1,
            DataSet.rpis);

        // 設定ListView元件使用的Adapter物件
        fruit_list.setAdapter(adapter);

        // 註冊項目點擊事件
        fruit_list.setOnItemClickListener(new MyItemListener());
    }

    // ListView項目點擊事件類別
    private class MyItemListener implements AdapterView.OnItemClickListener
    {

        @Override
        public void onItemClick(AdapterView<?> parent, View view,
                                int position, long id)
        {
            // 取得使用這個Fragment的Activity元件
            Fragment02Activity fragmen02Activity =
                (Fragment02Activity) getActivity();
            // 通知Activity元件項目點擊的編號
            fragmen02Activity.onItemSelected(position);
        }

    }

    // 給Activity元件實作的介面
    public interface OnItemSelectedListener
    {
        // 呼叫這個方法，請Activity元件執行點擊項目後的工作，參數是項目編號
        public void onItemSelected(int position);
    }

}