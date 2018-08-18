package net.macdidi.data01;

import java.text.SimpleDateFormat;
import java.util.Date;

// 位置類別，一個Place物件就代表一個位置資料
public class Place
{

    // 一定要包含這個固定的資料編號欄位變數
    private long id;

    // 把需要的資料包裝為欄位變數
    private double latitude;
    private double longitude;
    private double accuracy;
    private String datetime;
    private String note;

    // 提供一個沒有參數的建構子
    public Place()
    {

    }

    // 提供一個包含所有資料參數的建構子
    public Place(long id, double latitude, double longitude,
                 double accuracy, String datetime, String note)
    {
        this.id = id;
        this.latitude = latitude;
        this.longitude = longitude;
        this.accuracy = accuracy;
        this.datetime = datetime;
        this.note = note;
    }

    // 取得編號
    public long getId()
    {
        return id;
    }

    // 設定編號
    public void setId(long id)
    {
        this.id = id;
    }

    // 取得緯度
    public double getLatitude()
    {
        return latitude;
    }

    // 設定緯度
    public void setLatitude(double latitude)
    {
        this.latitude = latitude;
    }

    // 取得經度
    public double getLongitude()
    {
        return longitude;
    }

    // 設定經度
    public void setLongitude(double longitude)
    {
        this.longitude = longitude;
    }

    // 取得誤差
    public double getAccuracy()
    {
        return accuracy;
    }

    // 設定誤差
    public void setAccuracy(double accuracy)
    {
        this.accuracy = accuracy;
    }

    // 取得日期時間
    public String getDatetime()
    {
        return datetime;
    }

    // 設定日期時間
    public void setDatetime(String datetime)
    {
        this.datetime = datetime;
    }

    // 額外提供的設定日期時間方法，接收一個long值的參數
    // 把參數轉換為日期時間後在設定給日期時間欄位變數
    public void setDatetime(long now)
    {
        Date date = new Date(now);
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm");
        datetime = sdf.format(date);
    }

    // 取得說明
    public String getNote()
    {
        return note;
    }

    // 設定說明
    public void setNote(String note)
    {
        this.note = note;
    }

}
