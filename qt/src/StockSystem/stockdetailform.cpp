#include "stockdetailform.h"
#include "ui_stockdetailform.h"

StockDetailForm::StockDetailForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StockDetailForm)
{
    ui->setupUi(this);
    stockInfo=new StockInfo();
}

StockDetailForm::~StockDetailForm()
{
    delete stockInfo;
    delete ui;
}

void StockDetailForm::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void StockDetailForm::readStockData(string fileName)
{
    stockInfo->clear();
    stockInfo->readData(fileName);
}

void StockDetailForm::paintEvent(QPaintEvent *e)//绘图
{
    QPainter painter(this);//绘图地方
    drawBackground(&painter);//绘坐标
    stockInfo->display(this);//显示
    stockInfo->display(this,COLUMN);
    QPainter painter2(this);//绘图地方
    drawBackground_2(&painter2);//绘坐标
    stockInfo->displayTrade(this);
    //开始绘交易量
}

void StockDetailForm::drawBackground(QPainter *painter)//绘坐标
{
    string stockName=stockInfo->getLastStockDayInfo().getStockName();
    int formHeight=this->height();
    int formWidth=this->width();
    painter->drawText(formWidth/2-50,20,QString(trUtf8(stockName.c_str())));//标题
    painter->drawLine(10,10,10,formHeight/2-10);
    painter->drawLine(10,formHeight/2-10,formWidth-10,formHeight/2-10);
}

void StockDetailForm::drawBackground_2(QPainter *painter)//绘交易量坐标
{
    string stockName=stockInfo->getLastStockDayInfo().getStockName();
    int formHeight=this->height();
    int formWidth=this->width();
    painter->drawText(formWidth/2-50,formHeight/2+50,QString(trUtf8(" 交易量 ")));//标题
    painter->drawLine(10,formHeight/2+50,10,formHeight-10);
    painter->drawLine(10,formHeight-10,formWidth-10,formHeight-10);
}
