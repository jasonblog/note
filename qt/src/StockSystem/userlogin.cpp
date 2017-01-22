#include "userlogin.h"
#include "ui_userlogin.h"
#include <qmessagebox.h>
#include <string>
#include <qlineedit.h>

using namespace std;

UserLogin::UserLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserLogin)
{
    ui->setupUi(this);
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
}

UserLogin::~UserLogin()
{
    delete ui;
}

void UserLogin::changeEvent(QEvent *e)
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

void UserLogin::on_pushButton_2_clicked()
{//登录返回
    flat=0;
    close();
}

void UserLogin::on_pushButton_clicked()
{//登录
    QString seachStock=ui->lineEdit->text();
    //将QT中的字符串类型转换为标准的string
    string userCoum=string((const char *)seachStock.toLocal8Bit());
    QString seachStock_2=ui->lineEdit_2->text();
    //将QT中的字符串类型转换为标准的string
    string userPasswd=string((const char *)seachStock_2.toLocal8Bit());
    if(userCoum=="admin" && userPasswd=="admin")
    {
        flat=1;
        close();
    }else{
        QMessageBox::information(NULL,trUtf8("系统提示"),trUtf8("帐号密码错误！请重新输入!"));
        flat=0;
    }

}
