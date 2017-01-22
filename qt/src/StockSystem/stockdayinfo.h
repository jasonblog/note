#ifndef STOCKDAYINFO_H
#define STOCKDAYINFO_H


#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class StockDayInfo
{
    private:
        int stockNum;
        string stockName;
        string tradeDate;
        float openPrice;
        float closingPrice;
        float highestPrice;
        float lowestPrice;
                int tradeNum;

    public:
        StockDayInfo();
        void readData(string infoStr);
        void display();
                int getStockNum();
                string getStockName();
        string getTradeDate();
        float getOpenPrice();
        float getClosingPrice();
        float getHighestPrice();
        float getLowestPrice();
                int getTradeNum();
        ~StockDayInfo();
};
#endif
