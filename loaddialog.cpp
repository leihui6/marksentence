#include "loaddialog.h"
#include "ui_loaddialog.h"

loadDialog::loadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loadDialog),
    m_save_object(new QFile),
    m_download_object(new QNetworkAccessManager)
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
    connect((QObject *)m_download_reply,SIGNAL(readyRead()),
            this,SLOT(on_downloadBeg()));
    connect((QObject *)m_download_reply,SIGNAL(downloadProgress(qint64 ,qint64)),
            this,SLOT(on_downloadIng(qint64 ,qint64)));
    connect((QObject *)m_download_reply,SIGNAL(finished()),
            this,SLOT(on_downloadFin()));
}

void loadDialog::on_downloadBeg()
{
    ui->progressBar->setVisible(true);
}

void loadDialog::on_downloadIng(qint64 bytesRead, qint64 bytesTotal)
{
    qDebug() << bytesRead << ":"<<bytesTotal;
    m_save_object->write(m_download_reply->readAll());
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesRead);
}

void loadDialog::on_downloadFin()
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

void loadDialog::on_btn_comfirm_clicked()
{
    m_tpo_index = ui->combox_TPO->currentIndex();
    m_section_index = ui->combox_section->currentIndex();
    m_detail_index = ui->combox_detail->currentIndex();
    // 生成m_filename
    // 生成远程获取url
    generateUrl();
    QString pathstr = m_remotepath+m_filename+".mp3";
    // qDebug()<<"download from:"<<pathstr;

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
    //qDebug() << local_save;
    m_savePath = QFileDialog::getSaveFileName(this,("Save"),local_save,tr("*.mp3"));

    // 用户没有选择路径
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
    if (m_save_object->isOpen()){
        m_save_object->close();
    }
    this->reject();
}

void loadDialog::closeEvent(QCloseEvent *event)
{
    if (m_save_object->isOpen()){
        m_save_object->close();
    }
    this->reject();
}

