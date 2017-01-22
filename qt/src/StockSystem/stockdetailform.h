#ifndef STOCKDETAILFORM_H
#define STOCKDETAILFORM_H

#include <QDialog>
#include "stockinfo.h"
namespace Ui {
    class StockDetailForm;
}

class StockDetailForm : public QDialog {
    Q_OBJECT
public:
    StockDetailForm(QWidget *parent = 0);
    ~StockDetailForm();
    void readStockData(string fileName);

protected:
    void changeEvent(QEvent *e);
    void paintEvent(QPaintEvent *);//绘图

private:
    Ui::StockDetailForm *ui;
    StockInfo* stockInfo;
    void drawBackground(QPainter* painter);
    void drawBackground_2(QPainter* painter);
};

#endif // STOCKDETAILFORM_H
