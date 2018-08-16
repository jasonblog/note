package net.macdidi.design01;

import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.NavigationView;
import android.support.design.widget.Snackbar;
import android.support.design.widget.TabLayout;
import android.support.v4.app.Fragment;
import android.support.v4.view.ViewPager;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class Design01Activity extends AppCompatActivity
{

    // DrawerLayout 物件
    private DrawerLayout drawerLayout;
    // 與 DrawerLayout 搭配使用的 ActionBar 控制物件
    private ActionBarDrawerToggle drawerToggle;
    // NavigationView 瀏覽物件
    private NavigationView navigationView;

    // ViewPager 分頁容器物件
    private ViewPager viewPager;
    // TabLayout 分頁物件
    private TabLayout tabLayout;

    // 浮動功能按鈕(FAB)物件
    private FloatingActionButton fab;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_design01);

        processViews();
        processControllers();

        // 設定 ViewPager 分頁容器物件
        configViewPager();
    }

    private void processViews()
    {
        drawerLayout = (DrawerLayout)
                       findViewById(R.id.design01_drawer_layout);
        navigationView = (NavigationView)
                         findViewById(R.id.drawer_navigation_view);

        if (navigationView != null) {
            // 設定 DrawerLayout 物件
            configDrawerContent(navigationView);
        }

        viewPager = (ViewPager) findViewById(R.id.view_pager);
        tabLayout = (TabLayout) findViewById(R.id.tab_layout);
        fab = (FloatingActionButton) findViewById(R.id.fab);

        // 建立與設定 Toolbar 物件
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        // 取得 ActionBar 物件
        ActionBar actionBar = getSupportActionBar();

        if (actionBar != null) {
            // 設定圖示
            actionBar.setHomeAsUpIndicator(R.drawable.ic_menu_white_36dp);
            actionBar.setDisplayHomeAsUpEnabled(true);
        }
    }

    private void processControllers()
    {
        // 註冊 FAB 點擊事件
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Refresh...", Snackbar.LENGTH_LONG)
                .setAction("Abort", new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        Toast.makeText(
                            Design01Activity.this,
                            "Abort",
                            Toast.LENGTH_SHORT).show();
                    }
                }).show();
            }
        });

        // 建立與註冊 ActionBarDrawerToggle 監聽物件
        drawerToggle = new ActionBarDrawerToggle(this, drawerLayout,
                R.string.drawer_open, R.string.drawer_close);
        drawerLayout.setDrawerListener(drawerToggle);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        // 讀取與設定功能表資源
        getMenuInflater().inflate(R.menu.menu_design01, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        // 如果是選擇 Drawer 的選單項目
        if (drawerToggle.onOptionsItemSelected(item)) {
            return true;
        }

        if (item.getItemId() == R.id.menu_shopping_cart) {
            // 啟動 ShoppingCartActivity 元件
            startActivity(new Intent(this, ShoppingCartActivity.class));

            return true;
        } else {
            return super.onOptionsItemSelected(item);
        }
    }

    // 設定 ViewPager 分頁容器物件
    private void configViewPager()
    {
        // 建立與設定儲存分頁名稱的 List 物件
        List<String> titles = new ArrayList<>();
        titles.add("Page 01");
        titles.add("Page 02");
        titles.add("Page 03");

        // 加入三個分頁
        tabLayout.addTab(tabLayout.newTab().setText(titles.get(0)));
        tabLayout.addTab(tabLayout.newTab().setText(titles.get(1)));
        tabLayout.addTab(tabLayout.newTab().setText(titles.get(2)));

        // 建立與設定儲存分頁 Fragment 的 List 物件
        List<Fragment> fragments = new ArrayList<>();
        fragments.add(new Page01Fragment());
        fragments.add(new Page02Fragment());
        fragments.add(new Page03Fragment());

        // 建立包含三個分頁的 PagerFragmentAdapter 物件
        PagerFragmentAdapter adapter = new PagerFragmentAdapter(
            getSupportFragmentManager(), fragments, titles);

        // 設定 ViewPager 使用的 PagerFragmentAdapter 物件
        viewPager.setAdapter(adapter);
        // 設定分頁物件使用的 ViewPager 物件
        tabLayout.setupWithViewPager(viewPager);
        tabLayout.setTabsFromPagerAdapter(adapter);
    }

    // 設定 DrawerLayout 物件
    private void configDrawerContent(NavigationView navigationView)
    {
        // 註冊 Drawer 選單項目點擊事件
        navigationView.setNavigationItemSelectedListener(
        new NavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(
                MenuItem menuItem) {
                menuItem.setChecked(true);
                // 關閉 Drawer
                drawerLayout.closeDrawers();
                return true;
            }
        });
    }

}
