#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_btn_comfirm_clicked();

    void on_btn_cancel_clicked();

private:
       QString m_uname;
       QString m_pwd;

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
