#ifndef HELP_H
#define HELP_H

#include <QDialog>

namespace Ui {
    class Help;
}

class Help : public QDialog {
    Q_OBJECT
public:
    Help(QWidget *parent = 0);
    ~Help();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Help *ui;

private slots:
    void on_pushButton_clicked();
};

#endif // HELP_H
