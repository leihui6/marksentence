#include "inc/settingdialog.h"
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

    this->ui->lineEdit_textPath->setReadOnly(true);
    this->ui->lineEdit_audioPath->setReadOnly(true);

    initValueOnWidget();
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::initValueOnWidget(){
    ui->spinBox_preSecond->setValue(G_preSecond/1000);
    ui->spinBox_stepSecond->setValue(G_stepSecond/1000);
}

void SettingDialog::on_btn_save_clicked()
{
    if(ui->spinBox_preSecond->text().size() == 0)
    {
        G_preSecond = 3;
    }
    else
    {
        G_preSecond = ui->spinBox_preSecond->text().toInt() * 1000;
    }

    if(ui->spinBox_stepSecond->text().size() == 0)
    {
        G_stepSecond = 3;
    }
    else
    {
       G_stepSecond = ui->spinBox_stepSecond->text().toInt()*1000;
    }

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

void SettingDialog::on_pushButton_clicked()
{
    const QString curr_path = QDir::currentPath();
    //qDebug() <<curr_path<<endl;
    QString temp = QFileDialog::getOpenFileName(this,"Open Audio File",curr_path,"*mp3 ");
    if (temp.indexOf(".mp3") == -1)
    {
        return;
    }
    G_audioFilename = temp;
    QString target_file_name = QFileInfo(G_audioFilename).baseName();
    this->ui->lineEdit_audioPath->setText(target_file_name);
    QDir::setCurrent(curr_path);
}

void SettingDialog::on_pushButton_2_clicked()
{
    const QString curr_path = QDir::currentPath();
    //qDebug() <<curr_path<<endl;
    QString temp = QFileDialog::getOpenFileName(this,"Open Lyric File",curr_path,"*txt ");
    if (temp.indexOf(".txt") == -1)
    {
        return;
    }
    G_textFilename = temp;
    QString target_file_name = QFileInfo(G_textFilename).baseName();
    this->ui->lineEdit_textPath->setText(target_file_name);
    QDir::setCurrent(curr_path);
}
