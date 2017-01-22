#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qstandarditemmodel.h>
#include <iostream>
#include <string>
#include "stockdayinfo.h"
#include "stockinfo.h"
#include "help.h"
#include "userlogin.h"
#include "stockdetailform.h"
using namespace std;


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    std::string dataFilePath;//股票文件路径
    list<StockInfo> *allStocks;
    Help help;
    UserLogin login;
    StockDetailForm stockDetailForm;
    int loginFlat;
    int loginDataFlat;
    list<StockDayInfo> *newStockList;

private slots:
    void on_pushButton_3_clicked();
    void on_action_7_triggered();
    void on_action_6_triggered();
    void on_action_5_triggered();
    void on_tableView_doubleClicked(QModelIndex index);
    void on_pushButton_2_clicked();
    void on_action_4_triggered();
    void on_pushButton_clicked();
    void on_action_3_triggered();
    void on_action_2_triggered();
    void on_action_triggered();
    void initTabView(QStandardItemModel *model);
    void seachStock(QStandardItemModel *model);
};

#endif // MAINWINDOW_H
