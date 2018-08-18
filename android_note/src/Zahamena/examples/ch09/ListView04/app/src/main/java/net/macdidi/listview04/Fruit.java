package net.macdidi.listview04;

public class Fruit
{

    // 宣告每個項目資料需要的欄位變數
    private long id;
    private String name;
    private String content;
    private boolean selected;

    // 建構子
    public Fruit(long id, String name, String content, boolean selected)
    {
        this.id = id;
        this.name = name;
        this.content = content;
        this.selected = selected;
    }

    // 讀取與設定欄位變數的方法
    public long getId()
    {
        return id;
    }

    public void setId(long id)
    {
        this.id = id;
    }

    public String getName()
    {
        return name;
    }

    public void setName(String name)
    {
        this.name = name;
    }

    public String getContent()
    {
        return content;
    }

    public void setContent(String content)
    {
        this.content = content;
    }

    public boolean isSelected()
    {
        return selected;
    }

    public void setSelected(boolean selected)
    {
        this.selected = selected;
    }

}
