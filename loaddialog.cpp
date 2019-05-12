#include "loaddialog.h"
#include "ui_loaddialog.h"

loadDialog::loadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loadDialog),
    m_audio_file(new QFile),
    m_lyric_file(new QFile),
    m_audio_load(new QNetworkAccessManager),
    m_lyric_load(new QNetworkAccessManager)
{
    ui->setupUi(this);
    this->setWindowTitle("TPO Listening Download Tool");
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    loadCombox();
    loadRemotePath();
    loadConstant();
}

loadDialog::~loadDialog()
{
    // add
    delete ui;
}

void loadDialog::loadCombox()
{
    // 初始化选中
    m_tpo_index = 0;
    m_section_index = 0;
    m_detail_index = 0;

    // 设置最大TPO序号
    const int TPO_size = 54;

    // 加载下拉菜单选项
    // 加载TPO序号
    for (int i = m_tpo_index+1;i != TPO_size+1;i++){
        ui->combox_TPO->addItem(QString("%1").arg(i));
    }
    ui->combox_TPO->setCurrentIndex(0);

    // 加载section
    QString section = "Listening,Speaking,Writing";
    m_section_list = section.split(",");
    for (int i = 0;i < m_section_list.size();i++){
        ui->combox_section->addItem(m_section_list[i]);
    }
    ui->combox_section->setCurrentIndex(0);

    // 加载听力section中得detail
    // 在选择的时候还会更进一步判断当前section有哪些detail
    QString detail = "C1,L1,L2,C2,L3,L4";
    m_detail_list = detail.split(",");
    for (int i = 0;i < m_detail_list.size();i++){
        ui->combox_detail->addItem(m_detail_list[i]);
    }
    ui->combox_detail->setCurrentIndex(0);
}

void loadDialog::loadConstant(){
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(false);

    // 设置下载目录
    m_saveDir = "TPO";
    createDir(m_saveDir);
}

void loadDialog::createDir(QString dir){
    QDir tempDir;
    if(!tempDir.exists(dir))
    {
        tempDir.mkpath(dir);
    }
    else {
        // ..
    }
}

void loadDialog::generateUrl()
{
       // 因为服务器上的tpo是按照序号划分的
        // 也就是说 /TPO/tpo序号/内容.mp3
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

       // 如果是Listeing部分，就下载听力文本
       QString detail = "C1,L1,L2,C2,L3,L4";
       m_detail_list = detail.split(",");
       if (m_section_index == 0){
           m_lyric_filename = QString("TPO%1%2").arg(m_tpo_index+1,2,10,QChar('0')).arg(m_detail_list[m_detail_index])+".txt";
       }
       else{
           m_lyric_filename = "";
       }
       qDebug() <<"m_lyric_filename:"<< m_lyric_filename;
}

void loadDialog::loadRemotePath()
{
    //  服务器IP
    // 防止IP被爬
    QString ip = "h#t#t#p:/#/3#9.#9#7.1#15.1#2#8";
    QString port = "8000";
    QString path = "/TPO/";
    ip.replace(QString("#"),QString(""));
    m_remotepath = ip+":"+port+path;
}

void loadDialog::on_combox_section_activated(int index)
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

void loadDialog::loadConnect(){
    // Connect audio operation
    connect((QObject *)m_audio_reply,SIGNAL(readyRead()), this,SLOT(on_loadBeg()));
    connect((QObject *)m_audio_reply,SIGNAL(downloadProgress(qint64 ,qint64)), this,SLOT(on_audioIng(qint64 ,qint64)));
    connect((QObject *)m_audio_reply,SIGNAL(finished()), this,SLOT(on_audioFin()));

    // Connect lyric operation
    connect((QObject *)m_lyric_reply,SIGNAL(readyRead()),this,SLOT(on_loadBeg()));
    connect((QObject *)m_lyric_reply,SIGNAL(downloadProgress(qint64 ,qint64)),this,SLOT(on_lyricIng(qint64 ,qint64)));
    connect((QObject *)m_lyric_reply,SIGNAL(finished()),this,SLOT(on_lyricFin()));
}

void loadDialog::on_loadBeg()
{
    ui->progressBar->setVisible(true);
}

void loadDialog::on_audioIng(qint64 bytesRead, qint64 bytesTotal)
{
    qDebug() << bytesRead << ":"<<bytesTotal;
    m_audio_file->write(m_audio_reply->readAll());
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesRead);
}

void loadDialog::on_audioFin()
{
    //qDebug() << "download finished";
    ui->progressBar->setVisible(true);
    m_audio_reply->deleteLater();
    m_audio_load->deleteLater();
    if (m_audio_file->isOpen()){
        m_audio_file->close();
    }
    // QString path=QDir::currentPath() +"\\"+m_saveDir;
    // path.replace("/","\\");
    // QProcess::startDetached("explorer "+path);
}

void loadDialog::on_lyricIng(qint64 bytesRead, qint64 bytesTotal)
{
    qDebug() << bytesRead << ":"<<bytesTotal;
    m_lyric_file->write(m_lyric_reply->readAll());
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesRead);
}

void loadDialog::on_lyricFin()
{
    ui->progressBar->setVisible(true);
    m_lyric_reply->deleteLater();
    m_lyric_load->deleteLater();
    if (m_lyric_file->isOpen()){
        m_lyric_file->close();
    }
}

void loadDialog::on_btn_comfirm_clicked()
{
    m_tpo_index = ui->combox_TPO->currentIndex();
    m_section_index = ui->combox_section->currentIndex();
    m_detail_index = ui->combox_detail->currentIndex();

    // 生成m_filename
    // 生成远程获取url
    generateUrl();
    QString lyric_url = m_remotepath+ "tpo_lyric/" + m_lyric_filename;
    QString pathstr = m_remotepath+m_filename+".mp3";
    qDebug()<<"download from:"<<pathstr;
    qDebug()<<"download from:"<<lyric_url;

    /*
     *  需要判断url是否有效
    */

    // 获取本地保存路径以及保存名称
    createDir(QString("TPO\\TPO%1").arg(m_tpo_index+1));
    QString local_save;
    local_save = QString("TPO\\TPO%1\\TPO_%2_%3_%4").
            arg(m_tpo_index+1).arg(m_tpo_index+1).
            arg(m_section_list[m_section_index]).arg(m_detail_list[m_detail_index]);
    local_save += ".mp3";

    m_savePath = QFileDialog::getSaveFileName(this,("Save"),local_save,tr("*.mp3"));

    // 用户没有选择路径
    if(m_savePath.isEmpty()){
        return;
    }

    // 打开指定文件准备写入
    // 1. 音频文件
    m_audio_file= new QFile(m_savePath);
    m_audio_file->open(QIODevice::WriteOnly | QIODevice::Truncate);
    // 2. 文本文件
    m_lyric_file = new QFile(m_lyric_filename);
    m_lyric_file->open(QIODevice::WriteOnly | QIODevice::Truncate);

    // 开始下载
    // 1. 下载音频文件
    QUrl url = QUrl(pathstr);
    m_audio_load = new QNetworkAccessManager(this);
    m_audio_reply=m_audio_load->get(QNetworkRequest(url));
    // 2. 下载文本文件
    url = QUrl(lyric_url);
    m_lyric_load = new QNetworkAccessManager(this);
    m_lyric_reply=m_lyric_load->get(QNetworkRequest(url));

    // 映射下载操作
    loadConnect();
}

void loadDialog::on_combox_TPO_activated(int index)
{
    m_tpo_index = index;
}

void loadDialog::on_combox_detail_activated(int index)
{
    m_detail_index = index;
}

void loadDialog::on_btn_cancel_clicked()
{
    if (m_audio_file->isOpen()){
        m_audio_file->close();
    }
    if (m_lyric_file->isOpen()){
        m_lyric_file->close();
    }
    this->reject();
}

void loadDialog::closeEvent(QCloseEvent *event)
{
    if (m_audio_file->isOpen()){
        m_audio_file->close();
    }
    if (m_lyric_file->isOpen()){
        m_lyric_file->close();
    }
    this->reject();
}

