#include "mydialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setWindowTitle("TPO listening download tool");

    loadCombox();
    loadRemotePath();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::loadCombox()
{
    // 初始化选中
    m_tpo_index = 50;
    m_section_index = 0;
    m_detail_index = 0;

    // 设置TPO序号
    const int TPO_size = 54;

    for (int i = m_tpo_index;i != TPO_size+1;i++){
        ui->combox_TPO->addItem(QString("%1").arg(i));
    }
    ui->combox_TPO->setCurrentIndex(0);

    // 设置section
    QString section = "Listening,Speaking,Writing";
    m_section_list = section.split(",");
    for (int i = 0;i < m_section_list.size();i++){
        ui->combox_section->addItem(m_section_list[i]);
    }
    ui->combox_section->setCurrentIndex(0);

    // 设置section下的听力内容
    QString detail = "C1,L1,L2,C2,L3,L4";
    m_detail_list = detail.split(",");
    for (int i = 0;i < m_detail_list.size();i++){
        ui->combox_detail->addItem(m_detail_list[i]);
    }
    ui->combox_detail->setCurrentIndex(0);
}

void Dialog::generateUrl()
{
       QString publicTitle = QString("tpo%1_").arg(m_tpo_index);
       QString postfix;
       if(m_section_index == 0){
           postfix = QString("listening%1_passage").arg(m_detail_index+1);
       }
       else if(m_section_index == 1){
           postfix = QString("speaking%1_question%2_dialog").
                   arg(m_detail_index+3).arg(m_detail_index+3);
       }
       else if (m_section_index == 2){
           postfix = QString("writing1_passage");
       }
       m_filename = publicTitle+postfix;

}

void Dialog::loadRemotePath()
{
    // 写入远端服务器IP
    // python -m SimpleHTTPServer
    m_remotepath = "http://39.97.115.128:8000/";
}

void Dialog::on_combox_section_activated(int index)
{
    m_detail_list.clear();
    ui->combox_detail->clear();
    m_section_index = index;
    QString detail;

    // Listening
    if(m_section_index ==0){
        detail = "C1,L1,L2,C2,L3,L4";
        m_detail_list = detail.split(",");
    }
    // Speaking
    else if (m_section_index == 1){
        detail = "Task3,Task4,Task5,Task6";
        m_detail_list = detail.split(",");
    }
    // Writing
    else if(m_section_index == 2){
        detail = "Integrated";
        m_detail_list.append(detail);
    }
    for (int i = 0;i < m_detail_list.size();i++){
        ui->combox_detail->addItem(m_detail_list[i]);
    }
    ui->combox_detail->setCurrentIndex(0);
}

void Dialog::on_btn_comfirm_clicked()
{
    generateUrl();
    m_savePath = QFileDialog::getSaveFileName(this,tr("Save Image"),m_filename+".mp3",tr("*.mp3"));

    if (!m_savePath.isEmpty()){
        QProcess pro;
        QString strPath = "../getpo.exe";
        QStringList  arguments;

        arguments.push_back(m_remotepath+m_filename+".mp3");
        arguments.push_back(m_savePath);

        qDebug()<<m_remotepath+m_filename+".mp3";
        qDebug()<<m_savePath;

        pro.startDetached(strPath,arguments);
    }

    //this->destroy();
}

void Dialog::on_combox_TPO_activated(int index)
{
    m_tpo_index = m_tpo_index + index;
}

void Dialog::on_combox_detail_activated(int index)
{
    m_detail_index = index;
}

void Dialog::on_btn_cancel_clicked()
{
    this->destroy();
}
