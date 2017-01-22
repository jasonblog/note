#ifndef STOCKINFO_H
#define STOCKINFO_H
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include "stockdayinfo.h"
#include <qpainter.h>
using namespace std;

enum DrawStockType{
    POLYLINE,       //折线图
    COLUMN          //柱形图
};

class StockInfo
{
    private:
        list<StockDayInfo> stockList;
    public:
        void readData(string fileName);
        void display();//打印全部
                StockDayInfo getLastStockDayInfo();//最后一条
        void searchStocknum(string oneStocknum);
                string searchStockname(string filePath,string oneStockname);
        void range();//排序
                void clear();
                void display(QPaintDevice *widget);//折线图
                void displayAsColumn(QPaintDevice *widget);//K线图
                void display(QPaintDevice *widget, DrawStockType drawStockType);
                void displayTrade(QPaintDevice *widget);
};
#endif
