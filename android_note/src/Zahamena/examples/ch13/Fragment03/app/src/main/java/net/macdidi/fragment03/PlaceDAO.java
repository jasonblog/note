package net.macdidi.fragment03;

import android.content.ContentValues;
import android.content.Context;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.preference.PreferenceManager;

import java.text.SimpleDateFormat;
import java.util.Date;

// 資料功能類別
public class PlaceDAO
{
    // 表格名稱
    public static final String TABLE_NAME = "place";

    // 編號表格欄位名稱，固定不變
    public static final String KEY_ID = "_id";

    // 其它需要的表格欄位名稱
    public static final String LATITUDE_COLUMN = "latitude";
    public static final String LONGITUDE_COLUMN = "longitude";
    public static final String ACCURACY_COLUMN = "accuracy";
    public static final String DATETIME_COLUMN = "datetime";
    public static final String NOTE_COLUMN = "note";

    // 所有欄位名稱陣列，把所有表格欄位名稱變數湊起來建立一個字串陣列
    public static final String[] COLUMNS = {
        KEY_ID, LATITUDE_COLUMN, LONGITUDE_COLUMN,
        ACCURACY_COLUMN, DATETIME_COLUMN, NOTE_COLUMN
    };

    // 顯示用欄位名稱陣列，
    // 在資料查詢畫面上希望顯示位置表格的編號、日期時間和說明，
    // 所以額外使用三個表格欄位名稱變數建立這個欄位名稱陣列
    public static final String[] SHOW_COLUMNS =
    {KEY_ID, DATETIME_COLUMN, NOTE_COLUMN};

    // 使用上面宣告的變數建立表格的SQL指令
    public static final String CREATE_TABLE =
        "CREATE TABLE " + TABLE_NAME + " (" +
        KEY_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
        LATITUDE_COLUMN + " REAL NOT NULL, " +
        LONGITUDE_COLUMN + " REAL NOT NULL, " +
        ACCURACY_COLUMN + " REAL NOT NULL, " +
        DATETIME_COLUMN + " TEXT NOT NULL, " +
        NOTE_COLUMN + " TEXT NOT NULL)";

    // 資料庫物件
    private SQLiteDatabase db;

    // 建構子，一般的應用都不需要修改
    public PlaceDAO(Context context)
    {
        db = MyDBHelper.getDatabase(context);
    }

    // 關閉資料庫，一般的應用都不需要修改
    public void close()
    {
        db.close();
    }

    // 新增參數指定的物件
    public Place insert(Place place)
    {
        // 建立準備新增資料的ContentValues物件
        ContentValues cv = new ContentValues();

        // 加入ContentValues物件包裝的新增資料
        // 第一個參數是欄位名稱， 第二個參數是欄位的資料
        cv.put(LATITUDE_COLUMN, place.getLatitude());
        cv.put(LONGITUDE_COLUMN, place.getLongitude());
        cv.put(ACCURACY_COLUMN, place.getAccuracy());
        cv.put(DATETIME_COLUMN, place.getDatetime());
        cv.put(NOTE_COLUMN, place.getNote());

        // 新增一筆資料並取得編號
        // 第一個參數是表格名稱
        // 第二個參數是沒有指定欄位值的預設值
        // 第三個參數是包裝新增資料的ContentValues物件
        long id = db.insert(TABLE_NAME, null, cv);

        // 設定編號
        place.setId(id);
        // 回傳結果
        return place;
    }

    // 修改參數指定的物件
    public boolean update(Place place)
    {
        // 建立準備修改資料的ContentValues物件
        ContentValues cv = new ContentValues();

        // 加入ContentValues物件包裝的修改資料
        // 第一個參數是欄位名稱， 第二個參數是欄位的資料
        cv.put(LATITUDE_COLUMN, place.getLatitude());
        cv.put(LONGITUDE_COLUMN, place.getLongitude());
        cv.put(ACCURACY_COLUMN, place.getAccuracy());
        cv.put(DATETIME_COLUMN, place.getDatetime());
        cv.put(NOTE_COLUMN, place.getNote());

        // 設定修改資料的條件為編號
        // 格式為「欄位名稱＝資料」
        String where = KEY_ID + "=" + place.getId();

        // 執行修改資料並回傳修改的資料數量是否成功
        return db.update(TABLE_NAME, cv, where, null) > 0;
    }

    // 刪除參數指定編號的資料
    public boolean delete (long id)
    {
        // 設定條件為編號，格式為「欄位名稱=資料」
        String where = KEY_ID + "=" + id;
        // 刪除指定編號資料並回傳刪除是否成功
        return db.delete(TABLE_NAME, where, null) > 0;
    }

    // 取得所有資料的Cursor物件
    public Cursor getAllCursor()
    {
        return db.query(TABLE_NAME, SHOW_COLUMNS,
                        null, null, null, null, null);
    }

    // 取得參數指定日期資料的Cursor物件
    public Cursor getSearchCursor(String date)
    {
        // 設定條件為查詢日期時間欄位的前十個字元，就是日期的部份，
        // 格式為「substr(欄位名稱,開始,個數)='資料'」
        // 字串資料必須在前後加上「'」，數字不用
        String where = "substr(datetime, 1, 10)='" + date + "'";
        // 查詢指定日期條件的資料
        return db.query(TABLE_NAME, SHOW_COLUMNS, where,
                        null, null, null, null);
    }

    // 取得指定編號的資料物件
    public Place get(long id)
    {
        // 準備回傳結果用的物件
        Place place = null;
        // 使用編號為查詢條件
        String where = KEY_ID + "=" + id;
        // 執行查詢
        Cursor result = db.query(TABLE_NAME, COLUMNS, where,
                                 null, null, null, null, null);

        // 如果有查詢結果
        if (result.moveToFirst()) {
            // 讀取包裝一筆資料的物件
            place = getRecord(result);
        }

        // 關閉Cursor物件
        result.close();
        // 回傳結果
        return place;
    }

    // 把Cursor目前的資料包裝為物件
    public Place getRecord(Cursor cursor)
    {
        // 準備回傳結果用的物件
        Place result = new Place();

        result.setId(cursor.getLong(0));
        result.setLatitude(cursor.getDouble(1));
        result.setLongitude(cursor.getDouble(2));
        result.setAccuracy(cursor.getDouble(3));
        result.setDatetime(cursor.getString(4));
        result.setNote(cursor.getString(5));

        // 回傳結果
        return result;
    }

    // 第一次執行應用程式新增一些範例資料
    public void sampleData(Context context)
    {
        SharedPreferences sp =
            PreferenceManager.getDefaultSharedPreferences(context);
        boolean firstTime = sp.getBoolean("FIRST_TIME", true);

        if (firstTime) {
            Place p01 = new Place(0, 25.04719, 121.516981, 10.0, "2011-12-31 08:30",
                                  "Hello!");
            Place p02 = new Place(0, 24.143033, 121.271982, 25.0, "2012-01-01 06:12",
                                  "Hi!");
            Place p03 = new Place(0, 25.200854, 121.646714, 55.0, "2012-02-12 16:50",
                                  "Awesome!");

            insert(p01);
            insert(p02);
            insert(p03);

            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm");

            for (int i = 0; i < 10; i++) {
                Date date = new Date(System.currentTimeMillis() + (i * 1000 * 60 * 60 * 24));
                Place place = new Place(0, 25.04719 + i, 121.516981 + i, i * 10,
                                        sdf.format(date), "Place: " + i);
                insert(place);
            }

            SharedPreferences.Editor editor = sp.edit();
            editor.putBoolean("FIRST_TIME", false);
            editor.commit();
        }
    }

}
