#include "mydialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    m_save_object(new QFile),
    m_download_object(new QNetworkAccessManager)

{
    ui->setupUi(this);
    this->setWindowTitle("TPO Listening Download Tool");

    loadCombox();
    loadRemotePath();
    loadConstant();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::loadCombox()
{
    // 初始化选中
    m_tpo_index = 0;
    m_section_index = 0;
    m_detail_index = 0;

    // 设置TPO序号
    const int TPO_size = 54;

    for (int i = m_tpo_index+1;i != TPO_size+1;i++){
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

void Dialog::loadConstant(){
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(false);

    m_saveDir = "TPO";
    QDir tempDir;
    if(!tempDir.exists(m_saveDir))
    {
        tempDir.mkpath(m_saveDir);
    }
}

void Dialog::createDir(QString dir){
    QDir tempDir;
    if(!tempDir.exists(dir))
    {
        tempDir.mkpath(dir);
    }
}

void Dialog::generateUrl()
{
       // 因为服务器上的tpo是分文件夹的
       QString publicTitle = QString("tpo%1/tpo%2_").arg(m_tpo_index+1).arg(m_tpo_index+1);
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
    m_remotepath = "http://39.97.115.128:8000/TPO/";
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
    //qDebug() << m_detail_list.size();
    for (int i = 0;i < m_detail_list.size();i++){
        ui->combox_detail->addItem(m_detail_list[i]);
    }
    ui->combox_detail->setCurrentIndex(0);
}

void Dialog::loadConnect(){
    connect((QObject *)m_download_reply,SIGNAL(readyRead()),
            this,SLOT(downloadBeg()));
    connect((QObject *)m_download_reply,SIGNAL(downloadProgress(qint64 ,qint64)),
            this,SLOT(downloadIng(qint64 ,qint64)));
    connect((QObject *)m_download_reply,SIGNAL(finished()),
            this,SLOT(downloadFin()));
}

void Dialog::downloadBeg()
{
    //qDebug() << "start to download";
    ui->progressBar->setVisible(true);
}

void Dialog::downloadIng(qint64 bytesRead, qint64 bytesTotal)
{
    //qDebug() << bytesRead << ":"<<bytesTotal;
    m_save_object->write(m_download_reply->readAll());
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesRead);
}

void Dialog::downloadFin()
{
    //qDebug() << "download finished";
    ui->progressBar->setVisible(true);
    m_download_reply->deleteLater();
    m_download_object->deleteLater();
    if (m_save_object->isOpen()){
        m_save_object->close();
    }
    QString path=QDir::currentPath() +"\\"+m_saveDir;
    path.replace("/","\\");
    QProcess::startDetached("explorer "+path);
}

void Dialog::on_btn_comfirm_clicked()
{
    m_tpo_index = ui->combox_TPO->currentIndex();
    m_section_index = ui->combox_section->currentIndex();
    m_detail_index = ui->combox_detail->currentIndex();
    // 生成m_filename
    // 生成远程获取路径
    generateUrl();
    QString pathstr = m_remotepath+m_filename+".mp3";

    // 获取本地保存路径
    createDir(QString("TPO\\TPO%1").arg(m_tpo_index+1));
    QString local_save;
    local_save = QString("TPO\\TPO%1\\TPO_%2_%3_%4").
            arg(m_tpo_index+1).arg(m_tpo_index+1).
            arg(m_section_list[m_section_index]).arg(m_detail_list[m_detail_index]);
    local_save += ".mp3";
    //qDebug() << local_save;
    m_savePath = QFileDialog::getSaveFileName(this,("Save"),local_save,tr("*.mp3"));

    if(m_savePath.isEmpty()){
        return;
    }

    // 打开指定文件准备写入
    m_save_object = new QFile(m_savePath);
    m_save_object->open(QIODevice::WriteOnly | QIODevice::Truncate);

    // 开始下载
    QUrl download_url = QUrl(pathstr);
    //qDebug() <<m_savePath;
    //qDebug() << download_url;
    m_download_object = new QNetworkAccessManager(this);
    m_download_reply=m_download_object->get(QNetworkRequest(download_url));

    // 映射下载操作
    loadConnect();
}

void Dialog::on_combox_TPO_activated(int index)
{
    m_tpo_index = index;
}

void Dialog::on_combox_detail_activated(int index)
{
    m_detail_index = index;

}

void Dialog::on_btn_cancel_clicked()
{
    if (m_save_object->isOpen()){
        m_save_object->close();
    }

    this->reject();
}
