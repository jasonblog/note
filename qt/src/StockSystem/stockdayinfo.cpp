#include "stockdayinfo.h"

StockDayInfo::StockDayInfo()
{

}

StockDayInfo::~StockDayInfo()
{
}

void StockDayInfo::display()
{
cout<<stockNum<<"\t"<<stockName<<"\t"<<tradeDate<<"\t"
   <<openPrice<<"\t"<<closingPrice<<"\t"<<highestPrice<<"\t"<<lowestPrice<<"\t"<<endl;
}

void StockDayInfo::readData(string infoStr)
{
    istringstream istr(infoStr);

istr>>stockNum>>stockName>>tradeDate>>openPrice>>closingPrice>>highestPrice>>lowestPrice>>tradeNum;
}

int StockDayInfo::getStockNum()
{
        return stockNum;
}

string StockDayInfo::getStockName()
{
        return stockName;
}

string StockDayInfo::getTradeDate()
{
        return tradeDate;
}

float StockDayInfo::getOpenPrice()
{
    return openPrice;
}

float StockDayInfo::getClosingPrice()
{
    return closingPrice;
}

float StockDayInfo::getHighestPrice()
{
    return highestPrice;
}

float StockDayInfo::getLowestPrice()
{
    return lowestPrice;
}
int StockDayInfo::getTradeNum()
{
        return tradeNum;
}

