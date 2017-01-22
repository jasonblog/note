#include "stockinfo.h"

void StockInfo::readData(string fileName)
{
    ifstream inFile;
    inFile.open(fileName.c_str());
    string info;
    while(getline(inFile,info))
    {
        StockDayInfo stockDayInfo;
        stockDayInfo.readData(info);
        stockList.push_back(stockDayInfo);
    }
    inFile.close();
}

void StockInfo::display()
{
    list<StockDayInfo>::iterator newLine;
    for(newLine=stockList.begin();newLine!=stockList.end();newLine++)
    {
        newLine->display();
    }
}

StockDayInfo StockInfo::getLastStockDayInfo()
{
    list<StockDayInfo>::iterator newLine;
    newLine=stockList.end();
    newLine--;
        return *newLine;
}

void StockInfo::searchStocknum(string oneStocknum)
{
    int flat=0;
    ifstream inFile;
    inFile.open("data/stock.dat");
    string stockNum;
    while(getline(inFile,stockNum))
    {
        if(oneStocknum==stockNum)
        {
            flat=1;
            break;
        }
    }
    inFile.close();
    if(flat==1)
    {
        oneStocknum="./data/"+oneStocknum+".dat";
        this->readData(oneStocknum);
        cout<<"代号"<<"\t"<<"名字"<<"\t"<<"日期"<<"\t"<<"\t"<<"开盘价"<<"\t";
        cout<<" 收盘价 "<<"\t"<<"最高价"<<"\t"<<"最低价"<<endl;
        this->display();
    }else{
        cout<<"没有找到"<<endl;
    }
}
string StockInfo::searchStockname(string filePath,string oneStockname)
{
    int flat=0;
    string stockPath=filePath+"stock.dat";
    ifstream inFile;
    inFile.open(filePath.c_str());
    string stockNum;
    while(getline(inFile,stockNum))
    {
            string filePath=filePath+stockNum+".dat";
            ifstream stockFile;
            stockFile.open(filePath.c_str());
            string searStockname,searName,num;
            getline(stockFile,searStockname);
            istringstream istr(searStockname);
            istr>>num>>searName;
            if(oneStockname == searName)
            {
                    return stockNum;
            }
            stockFile.close();
    }
    inFile.close();
}


void StockInfo::range()
{
    list<StockDayInfo>::iterator newLine;
    for(newLine=stockList.begin();newLine!=stockList.end();newLine++)
    {
        newLine->getTradeDate();
    }

}

void StockInfo::clear()
{
    stockList.clear();
}

void StockInfo::display(QPaintDevice *widget)//画折线图
{
    QPainter painter(widget);
    list<StockDayInfo>::iterator stockDayInfoIterator;
    QPointF* points=new QPointF[stockList.size()];//节点数

    float minPrice=stockList.begin()->getClosingPrice();
    float maxPrice=stockList.begin()->getClosingPrice();
    for(stockDayInfoIterator=stockList.begin();
        stockDayInfoIterator!=stockList.end();
        stockDayInfoIterator++)
    {
        if(minPrice>stockDayInfoIterator->getClosingPrice())
            minPrice=stockDayInfoIterator->getClosingPrice();//最低收盘价
        if(maxPrice<stockDayInfoIterator->getClosingPrice())
            maxPrice=stockDayInfoIterator->getClosingPrice();
    }

    int i=0;
    int axesHeight=widget->height()/2-50;
    float mulriple=axesHeight/(maxPrice-minPrice);//股票放大
    for(stockDayInfoIterator=stockList.begin();
            stockDayInfoIterator!=stockList.end();
            stockDayInfoIterator++){//将节点转化为坐标
        points[i].setX(20*i+20);
        points[i].setY(axesHeight+25-(stockDayInfoIterator->getClosingPrice()-minPrice)*mulriple);
        i++;
    }
    painter.drawPolyline(points,stockList.size());//划线
    delete[] points;
}

void StockInfo::displayAsColumn(QPaintDevice *widget)//画出K线图
{
    QPainter painter(widget);
    list<StockDayInfo>::iterator stockDayInfoIterator;
    //QPointF* points=new QPointF[stockInfo.size()];


    float minPrice=stockList.begin()->getLowestPrice();
    float maxPrice=stockList.begin()->getHighestPrice();
    for(stockDayInfoIterator=stockList.begin();
        stockDayInfoIterator!=stockList.end();
        stockDayInfoIterator++)
    {
        if(minPrice>stockDayInfoIterator->getLowestPrice())
            minPrice=stockDayInfoIterator->getLowestPrice();
        if(maxPrice<stockDayInfoIterator->getHighestPrice())
            maxPrice=stockDayInfoIterator->getHighestPrice();
    }
    int i=0;
    int axesHeight=widget->height()/2-50;
    float mulriple=axesHeight/(maxPrice-minPrice);

    for(stockDayInfoIterator=stockList.begin();
            stockDayInfoIterator!=stockList.end();
            stockDayInfoIterator++){
        //如果收盘价低于开盘价，画绿色图形
        if(stockDayInfoIterator->getClosingPrice()<stockDayInfoIterator->getOpenPrice())
        {
            painter.setPen(Qt::green);
            painter.setBrush(Qt::green);
        }
        else    //否则画红色图形
        {
            painter.setPen(Qt::red);
            painter.setBrush(Qt::red);
        }
        int centerX=20*i+20;
        //画出矩形中的影线getHighestPrice
        painter.drawLine(centerX,axesHeight+25-(stockDayInfoIterator->getLowestPrice()-
minPrice)*mulriple
                         ,centerX,axesHeight+25-(stockDayInfoIterator->getHighestPrice()-
minPrice)*mulriple);

        //画出矩形
        painter.drawRect(centerX-5,axesHeight+25-(stockDayInfoIterator->getOpenPrice()-
minPrice)*mulriple,10,abs((stockDayInfoIterator->getClosingPrice()
                           -stockDayInfoIterator->getOpenPrice())*mulriple));
        i++;
    }
}

void StockInfo::display(QPaintDevice *widget, DrawStockType drawStockType)
{
    switch(drawStockType)
    {
    case POLYLINE:
        display(widget);
        break;
    case COLUMN:
        displayAsColumn(widget);
        break;
    }
}

void StockInfo::displayTrade(QPaintDevice *widget)
{
    QPainter painter(widget);
    list<StockDayInfo>::iterator stockDayInfoIterator;
    int formHeight=widget->height();
    float minPrice=stockList.begin()->getLowestPrice();
    float maxPrice=stockList.begin()->getHighestPrice();
    for(stockDayInfoIterator=stockList.begin();
        stockDayInfoIterator!=stockList.end();
        stockDayInfoIterator++)
    {
        if(minPrice>stockDayInfoIterator->getLowestPrice())
            minPrice=stockDayInfoIterator->getLowestPrice();
        if(maxPrice<stockDayInfoIterator->getHighestPrice())
            maxPrice=stockDayInfoIterator->getHighestPrice();
    }
    int i=0;
    for(stockDayInfoIterator=stockList.begin();
            stockDayInfoIterator!=stockList.end();
            stockDayInfoIterator++){
        //如果收盘价低于开盘价，画绿色图形
        if(stockDayInfoIterator->getClosingPrice()<stockDayInfoIterator->getOpenPrice())
        {
            painter.setPen(Qt::green);
            painter.setBrush(Qt::green);
        }
        else    //否则画红色图形
        {
            painter.setPen(Qt::red);
            painter.setBrush(Qt::red);
        }
        int centerX=20*i+20;
        painter.drawRect(centerX,formHeight-12,10,-stockDayInfoIterator->getTradeNum()/10);
        i++;
    }
}
