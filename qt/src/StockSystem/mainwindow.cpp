#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qvariant.h>
#include <qlineedit.h>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    allStocks=new list<StockInfo>();
    loginFlat=0;
    loginDataFlat=0;
    newStockList=new list<StockDayInfo>();
}

MainWindow::~MainWindow()
{
    delete allStocks;
    delete newStockList;
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_action_triggered()
{//菜单栏->文件->导入
    if(loginFlat==0 || login.flat==0)
    {
        login.show();
        loginFlat=1;
    }else{
        if(loginDataFlat==0)
        {
            QString qsDataFileName=QFileDialog::getOpenFileName(this,trUtf8("数据导入"),".",
                                          trUtf8("数据文件(stock.dat)"));
            if(qsDataFileName.length()==0)//按下取消按钮是弹出MessageBox消息框
            {
                 QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("您没选择任何文件"));
             }else{
            //字符串转换
            std::string dataFileName= std::string((const char *)qsDataFileName.toLocal8Bit());
            //把文件导入到allstock中
            ifstream dataFile;
            dataFile.open(dataFileName.c_str());
            string stockNum;
            int stocksNum=0;
            //cout<<stocksNum<<endl;
            while(getline(dataFile,stockNum))
            {
                //对于每只股票，构建StockInfo对象，加入到allStocks
                StockInfo stockInfo;
                //根据文件名获取文件路径，供读取每支股票信息时用
                QFileInfo fileInfo(qsDataFileName);
                //文件的绝对路径
                dataFilePath=std::string((const char *)fileInfo.absolutePath().toLocal8Bit());
                string fileName=dataFilePath+"/"+stockNum+".dat";
                //路径完成
                //加载文件开始
                stockInfo.readData(fileName);
                allStocks->push_back(stockInfo);
                //完成一个文件到加载
                stocksNum++;
             }
             //allstock的导入完成
             //显示allstock
             QStandardItemModel *model=new QStandardItemModel(stocksNum,8);
             initTabView(model);
             dataFile.close();
             loginDataFlat=1;
            }
         }else{
             QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("您已经导入数据了!"));
         }
    }
}

void MainWindow::initTabView(QStandardItemModel *model)
{
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置只读
    //表头
    model->setHeaderData(0,Qt::Horizontal,trUtf8("股票代码"));
    model->setHeaderData(1,Qt::Horizontal,trUtf8("股票名称"));
    model->setHeaderData(2,Qt::Horizontal,trUtf8("日期"));
    model->setHeaderData(3,Qt::Horizontal,trUtf8("开盘价"));
    model->setHeaderData(4,Qt::Horizontal,trUtf8(" 收盘价 "));
    model->setHeaderData(5,Qt::Horizontal,trUtf8("最高价"));
    model->setHeaderData(6,Qt::Horizontal,trUtf8("最低价"));
    model->setHeaderData(7,Qt::Horizontal,trUtf8(" 交易量 "));

    list<StockInfo>::iterator stockInfoIterator;
    int row=0;
    for(stockInfoIterator=allStocks->begin();stockInfoIterator!=allStocks->end();stockInfoIterator++,row++)
    {
           StockDayInfo stockDayInfo=stockInfoIterator->getLastStockDayInfo();
           //代号
           QModelIndex index=model->index(row,0,QModelIndex());
           model->setData(index,QVariant(stockDayInfo.getStockNum()));
           //名字
           index=model->index(row,1,QModelIndex());
           model->setData(index,QVariant(trUtf8(stockDayInfo.getStockName().c_str())));
           //...
           index=model->index(row,2,QModelIndex());
           model->setData(index,QVariant(trUtf8(stockDayInfo.getTradeDate().c_str())));

           index=model->index(row,3,QModelIndex());
           model->setData(index,QVariant(stockDayInfo.getOpenPrice()));

           index=model->index(row,4,QModelIndex());
           model->setData(index,QVariant(stockDayInfo.getClosingPrice()));

           index=model->index(row,5,QModelIndex());
           model->setData(index,QVariant(stockDayInfo.getHighestPrice()));

           index=model->index(row,6,QModelIndex());
           model->setData(index,QVariant(stockDayInfo.getLowestPrice()));

           index=model->index(row,7,QModelIndex());
           model->setData(index,QVariant(stockDayInfo.getTradeNum()));
    }

    //设置tableView的model，刷新tableView里面的数据
    ui->tableView->setModel(model);

}

void MainWindow::on_action_2_triggered()
{//菜单栏->文件->退出系统
    if((QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8(" 是否退出 "),
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::No))==16384)
    {
        QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8(" 您已安全退出 "));
        QApplication::exit();
    }
}

void MainWindow::on_action_3_triggered()
{//菜单栏->文件->帮助
    help.show();
}

void MainWindow::on_pushButton_clicked()
{//开始使用按钮
    if(loginDataFlat==0)
    {
        if(loginFlat==0 || login.flat==0)
        {
            login.show();
            loginFlat=1;
        }
        if(login.flat==1)
        {
            loginFlat=1;
            on_action_triggered();
        }
    }else{
        QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("您已经导入数据了!"));
    }
}

void MainWindow::on_action_4_triggered()
{//菜单栏->帮助->打开帮助
    on_action_3_triggered();
}

void MainWindow::on_pushButton_2_clicked()
{//查询
    if(loginFlat==0 || login.flat==0)
    {
        QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("您还没有登录!"));
    }else{
        if(loginDataFlat==0)
        {
            QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("您还没有导入数据!"));
        }else{
            QString seachStockName=ui->lineEdit->text();
            //将QT中的字符串类型转换为标准的string
            string searchStockStr=string((const char *)seachStockName.toLocal8Bit());
            string searchFlat=searchStockStr.substr(0,1);
            StockInfo stock;
            if(searchFlat>="0" && searchFlat<="9")//代号查询
            {
                delete newStockList;
                newStockList=new list<StockDayInfo>();
                string searchFileName=dataFilePath+"/"+searchStockStr+".dat";
                ifstream inFile;
                inFile.open(searchFileName.c_str());
                string info;
                int stockNum=0;
                while(getline(inFile,info))
                {
                        StockDayInfo searchStockDayInfo;
                        searchStockDayInfo.readData(info);
                        newStockList->push_back(searchStockDayInfo);
                        stockNum++;
                }
                //刷图
                QStandardItemModel *modelStock=new QStandardItemModel(stockNum,8);
                //QStandardItemModel *model=new QStandardItemModel(stockNum,7);
                seachStock(modelStock);
                inFile.close();
            }else{
                //名字查询searchStockname
                delete newStockList;
                newStockList=new list<StockDayInfo>();
                string searchStockFileName;//代号
                string stockPath=dataFilePath+"/stock.dat";
                ifstream inFile_1;
                inFile_1.open(stockPath.c_str());
                string stockNum;
                while(getline(inFile_1,stockNum))
                {
                        string filePath=dataFilePath+"/"+stockNum+".dat";
                        ifstream stockFile;
                        stockFile.open(filePath.c_str());
                        string searStockname,searName,num;
                        getline(stockFile,searStockname);
                        istringstream istr(searStockname);
                        istr>>num>>searName;
                        if(searchStockStr == searName)
                        {
                            searchStockFileName=num;
                            break;
                        }
                        stockFile.close();
                }
                inFile_1.close();
                string searchFileName=dataFilePath+"/"+searchStockFileName+".dat";
                ifstream inFile_2;
                inFile_2.open(searchFileName.c_str());
                string info;
                int stockNum_2=0;
                while(getline(inFile_2,info))
                {
                        StockDayInfo searchStockDayInfo;
                        searchStockDayInfo.readData(info);
                        newStockList->push_back(searchStockDayInfo);
                        stockNum_2++;
                }
                //刷图
                QStandardItemModel *modelStock=new QStandardItemModel(stockNum_2,8);
                seachStock(modelStock);
                inFile_2.close();
            }
        }
    }
}

void MainWindow::on_tableView_doubleClicked(QModelIndex index)
{//双击
    if(loginFlat==0 || login.flat==0)
    {
        QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("您还没有登录!"));
    }else{
        //获取tableView中双击行的第一列的index
        QModelIndex stockNumIndex=ui->tableView->model()->index(index.row(),0,QModelIndex());
        //根据index获取tableView中的数据
        QString tempStr=ui->tableView->model()->data(stockNumIndex).toString();
        //将QT中的字符串类型转换为标准的string
        string stockNum=string((const char *)tempStr.toLocal8Bit());
        string fileName=dataFilePath+"/"+stockNum+".dat";
        stockDetailForm.readStockData(fileName);
        stockDetailForm.show();
    }
}

void MainWindow::on_action_5_triggered()
{//注销用户
    if(loginFlat==0 || login.flat==0)
    {
        QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("您还没有登录!"));
    }else{
        if((QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("是否注销"),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No))==16384)
        {
            loginFlat=0;
            login.flat=0;
            loginDataFlat=0;
            //设置tableView的model，刷新tableView里面的数据
            QStandardItemModel *model=new QStandardItemModel(0,0);
            ui->tableView->setModel(model);
            QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("注销成功，谢谢使用！"));
        }
    }
}

void MainWindow::on_action_6_triggered()
{
    //退出->注销
    on_action_5_triggered();
}

void MainWindow::on_action_7_triggered()
{
    //退出->退出系统
    if((QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8(" 是否退出 "),
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::No))==16384)
    {
        QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("您已安全退出!"));
        QApplication::exit();
    }
}

void MainWindow::seachStock(QStandardItemModel *model)
{
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置只读
    //表头
    model->setHeaderData(0,Qt::Horizontal,trUtf8("股票代码"));
    model->setHeaderData(1,Qt::Horizontal,trUtf8("股票名称"));
    model->setHeaderData(2,Qt::Horizontal,trUtf8("日期"));
    model->setHeaderData(3,Qt::Horizontal,trUtf8("开盘价"));
    model->setHeaderData(4,Qt::Horizontal,trUtf8(" 收盘价 "));
    model->setHeaderData(5,Qt::Horizontal,trUtf8("最高价"));
    model->setHeaderData(6,Qt::Horizontal,trUtf8("最低价"));
    model->setHeaderData(7,Qt::Horizontal,trUtf8(" 交易量 "));

    list<StockDayInfo>::iterator iter;
    int row=0;
    for(iter=newStockList->begin();iter!=newStockList->end();iter++,row++)
    {
           //代号
           QModelIndex index=model->index(row,0,QModelIndex());
           model->setData(index,QVariant(iter->getStockNum()));
           //名字
           index=model->index(row,1,QModelIndex());
           model->setData(index,QVariant(trUtf8(iter->getStockName().c_str())));
           //...
           index=model->index(row,2,QModelIndex());
           model->setData(index,QVariant(trUtf8(iter->getTradeDate().c_str())));

           index=model->index(row,3,QModelIndex());
           model->setData(index,QVariant(iter->getOpenPrice()));

           index=model->index(row,4,QModelIndex());
           model->setData(index,QVariant(iter->getClosingPrice()));

           index=model->index(row,5,QModelIndex());
           model->setData(index,QVariant(iter->getHighestPrice()));

           index=model->index(row,6,QModelIndex());
           model->setData(index,QVariant(iter->getLowestPrice()));

           index=model->index(row,7,QModelIndex());
           model->setData(index,QVariant(iter->getTradeNum()));
    }
    //设置tableView的model，刷新tableView里面的数据
    ui->tableView->setModel(model);

}

void MainWindow::on_pushButton_3_clicked()
{
    //首页按钮
    if(loginFlat==0 || login.flat==0)
    {
        QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("您还没有登录!"));
    }else{
        if(loginDataFlat==0)
        {
            QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("您还没有导入数据!"));
        }else{
            delete allStocks;
            allStocks=new list<StockInfo>();
            ifstream dataFile;
            string fileName=dataFilePath+"/"+"stock"+".dat";
            dataFile.open(fileName.c_str());
            string stockNum;
            int stocksNum=0;
            while(getline(dataFile,stockNum))
            {
                string filePath=dataFilePath+"/"+stockNum+".dat";
                StockInfo stockInfo;
                stockInfo.readData(filePath);
                allStocks->push_back(stockInfo);
                stocksNum++;
            }
            QStandardItemModel *model=new QStandardItemModel(stocksNum,8);
            initTabView(model);
            dataFile.close();
        }
    }
}
