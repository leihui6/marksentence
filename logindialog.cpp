#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_btn_comfirm_clicked()
{
    m_uname = ui->input_username->text();
    m_pwd = ui->input_password->text();
}

void LoginDialog::on_btn_cancel_clicked()
{
    this->destroy();
    this->reject();
}
