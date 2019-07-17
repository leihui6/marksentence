#include "settingdialog.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Settings");
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setAttribute(Qt::WA_DeleteOnClose,true);

    initValueOnWidget();
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::initValueOnWidget(){
    ui->lineEdit_remoteIP->setText(G_remote_ip);
    ui->spinBox_preSecond->setValue(G_preSecond/1000);
    ui->spinBox_stepSecond->setValue(G_stepSecond/1000);
    ui->checkBox_closeAfterdl->setChecked(G_closeAfterDownload);
    ui->checkBox_playAfterdl->setChecked(G_playAfterDownload);
    ui->checkBox_dowloadWithoutN->setChecked(G_downloadWithoutNotifiction);
}

void SettingDialog::on_btn_save_clicked()
{
    G_remote_ip = ui->lineEdit_remoteIP->text();
    G_preSecond = ui->spinBox_preSecond->text().toInt() * 1000;
    G_stepSecond = ui->spinBox_stepSecond->text().toInt()*1000;
    G_playAfterDownload = ui->checkBox_playAfterdl->isChecked();
    G_closeAfterDownload = ui->checkBox_closeAfterdl->isChecked();
    G_downloadWithoutNotifiction = ui->checkBox_dowloadWithoutN->isChecked();

#ifdef  DLDEBUG
    qDebug()
            <<"[loadDialog]"
           <<"Remote IP:" <<G_remote_ip<<endl
          <<"preSecond:" <<G_preSecond<<endl
         <<"stepSecond"<<G_stepSecond<<endl
        <<"PlayAfterDownload:"<<G_playAfterDownload<<endl
       <<"CloseAfterDownload:"<<G_closeAfterDownload<<endl
          <<"DownloadWithoutNotifiction:"<<G_downloadWithoutNotifiction<<endl;
#endif
    this->accept();
}

void SettingDialog::on_btn_cane_clicked()
{
    this->reject();
}
