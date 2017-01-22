#ifndef USERLOGIN_H
#define USERLOGIN_H

#include <QDialog>
#include <iostream>
#include <string>
namespace Ui {
    class UserLogin;
}

class UserLogin : public QDialog {
    Q_OBJECT
public:
    UserLogin(QWidget *parent = 0);
    ~UserLogin();
    int flat;



protected:
    void changeEvent(QEvent *e);

private:
    Ui::UserLogin *ui;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // USERLOGIN_H
