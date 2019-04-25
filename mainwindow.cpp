#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_dialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dialog(new Ui::Dialog),
    m_music()
{
    this->setWindowTitle("MarkSentence");

    ui->setupUi(this);

    // 初始化一些常量
    loadConstant();

    // 加载界面中的下拉菜单
    loadComboxItems();

    // 初始化槽连接
    loadConnect();

    // 初始化界面控制
    loadControl(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_save_clicked()
{
    log_write(QString("Saving:Segment ["+GetFormatTime(m_beg_point)+"-"+GetFormatTime(m_end_point)+"] NoteLength:%1 TypeLength:%2 Selected %3,%4").
              arg(m_note_content.size()).arg(m_type_content.size()).
              arg(m_level_index).arg(m_sort_index));
    // 保存到内存，不是文件
    createOneMark();
    // 清除界面数据
    ui->text_type->clear();
    ui->text_note->clear();
    // 保存到文件
    saveMarkIntoFile();
    // 刷新界面
    updateListWidget(false);
    log_write("Saved");
}

QString MainWindow::GetPlainContent(QPlainTextEdit * plainText)
{
    QTextDocument * doc = plainText->document();
    QTextBlock block = doc->firstBlock();
    QString text;

    for(int i = 0; i < doc->blockCount();i++,block=block.next()){
        text = text + block.text() + '\n';
    }
    return text;
}

void MainWindow::SetPlainContent(){
    ui->text_type->setPlainText(m_type_content);
    ui->text_note->setPlainText(m_note_content);
}


QString MainWindow::GetFormatTime(qint64 time)
{
    int h,m,s;
    time /= 1000;
    h = time/3600;
    m = (time-h*3600)/60;
    s = time-h*3600-m*60;
    return QString("%1:%2").
            arg(m,2,10,QChar('0')).
            arg(s,2,10,QChar('0'));
}

QString MainWindow::GetFormatMark(const QJsonObject &json)
{
    QString mark;
    // 获取起止时间
    QString beg_point = GetFormatTime(json["beg_point"].toInt()),
            end_point = GetFormatTime(json["end_point"].toInt());
    mark = "["+beg_point +"-"+end_point+"]\t";
    // 追加难度和种类
    mark += m_level_value[json["level_index"].toInt()] + "\t";
    mark += m_sort_value[json["sort_index"].toInt()];
    return mark;
}

void MainWindow::loadConstant()
{
    // 初始化下拉菜单的选项
    QString level_value = "Easy,Midd,Hard",
            sort_value = "None,Unfamiliar Words,Too Long,Garmmar";
    m_level_value =level_value.split(",");
    m_sort_value = sort_value.split(",");
    m_level_index = 0;
    m_sort_index = 0;

    // 初始化音频播放相关数据
    m_play = false;
    m_beg_point = 0;
    m_end_point = 0;
    m_total_time = 0;
    m_step_millSecond = 3000; // 3s

    // 初始化标记数据
    m_mark_index = -1;
    m_mark_vec.clear();

    // 用于文件读写
    m_file_saveDir = "Json";
    loadFileDir();
    m_log_name = "log";
    loadFileLog();
}

void MainWindow::fileOpenIsFailed(QString title,QString content){
    QMessageBox::about(NULL, title, content);
}

void MainWindow::log_write(QString info){
    if (!m_log_object.isOpen()){
        return;
    }
    m_log_object.write((info+'\n').toStdString().c_str());
}

void MainWindow::loadFileLog(){
    m_log_object.setFileName(m_log_name);
    if (!m_log_object.open(QIODevice::WriteOnly | QIODevice::Text)){
        fileOpenIsFailed("文件打开失败","详细信息请查阅log文件或反馈");
    }
}
void MainWindow::loadControl(bool status)
{
    ui->text_type->clear();
    ui->text_note->clear();
    if (!status){
        ui->button_play->setEnabled(false);
        ui->button_stop->setEnabled(false);
        ui->button_back->setEnabled(false);
        ui->button_forward->setEnabled(false);
        ui->button_set_end->setEnabled(false);
        ui->button_set_start->setEnabled(false);
        ui->button_clear_beg_point->setEnabled(false);
        ui->button_clear_end_point->setEnabled(false);
        ui->button_save->setEnabled(false);
        ui->horizon_music->setEnabled(false);
        ui->button_delete->setEnabled(false);
    }
    else{
        ui->button_play->setEnabled(true);
        ui->button_stop->setEnabled(true);
        ui->button_back->setEnabled(true);
        ui->button_forward->setEnabled(true);
        ui->button_set_end->setEnabled(true);
        ui->button_set_start->setEnabled(true);
        ui->button_clear_beg_point->setEnabled(true);
        ui->button_clear_end_point->setEnabled(true);
        ui->button_save->setEnabled(true);
        ui->horizon_music->setEnabled(true);
    }
}


void MainWindow::createOneMark()
{
    QJsonObject curr_seg;

    m_type_content = GetPlainContent(ui->text_type);
    m_note_content = GetPlainContent(ui->text_note);

    curr_seg.insert("type", m_type_content);
    curr_seg.insert("note", m_note_content);
    curr_seg.insert("beg_point", m_beg_point);
    curr_seg.insert("end_point", m_end_point);
    curr_seg.insert("level_index", m_level_index);
    curr_seg.insert("sort_index", m_sort_index);

    m_mark_vec.push_back(curr_seg);
}

void MainWindow::saveMarkIntoFile()
{
    // 创建json列表用于保存
    // "序号":"标记内容(json)"
    QJsonObject save_json;
    int index = 0;
    for (QVector<QJsonObject>::iterator it =  m_mark_vec.begin(); it !=  m_mark_vec.end();++it,++index){
        save_json.insert(QString(index),*it);
    }
    QJsonDocument jsonDoc(save_json);

    // 将文件清空
    if(m_file_object.isOpen()){
        m_file_object.close();
    }
    m_file_object.open(QIODevice::ReadWrite|QIODevice::Truncate);
    // 写入文件
    m_file_object.write(jsonDoc.toJson());
    m_file_object.flush();
}

void MainWindow::loadJsonContent()
{
    QByteArray data = m_file_object.readAll();

    // qDebug() << data;
    QJsonParseError e;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &e);

    if(e.error == QJsonParseError::NoError && !jsonDoc.isNull())
    {
        //qDebug() <<"doc=\n"<< jsonDoc;
    }
    QJsonObject json = jsonDoc.object();

    QJsonObject curr_json_seg;
    for (QJsonObject::iterator it=json.begin();it != json.end();++it){
        curr_json_seg = it.value().toObject();
        // qDebug() << it.value().toObject();
        m_mark_vec.push_back(curr_json_seg);
    }
}

void MainWindow::updateListWidget(bool add,const QJsonObject &json)
{
    if (add == false){
        ui->listWidget->clear();
        for (QVector<QJsonObject>::iterator it=m_mark_vec.begin();
             it != m_mark_vec.end();++it){
            ui->listWidget->addItem(new QListWidgetItem(GetFormatMark(*it)));
        }
    }
    else{
        ui->listWidget->addItem(new QListWidgetItem(GetFormatMark(json)));
    }
}

void MainWindow::music_after_stop()
{
    m_play = false;
    m_music.setPosition(0);
    ui->label_beg_point->clear();
    ui->label_end_point->clear();
    ui->button_play->setText("Play");
}

void MainWindow::loadComboxItems()
{
    // Load Current Select Value
    for (int i = 0;i < m_level_value.size();i++){
        ui->combox_level->addItem(m_level_value[i]);
    }
    ui->combox_level->setCurrentIndex(0);

    for (int i = 0;i < m_sort_value.size();i++){
        ui->combox_sort->addItem(m_sort_value[i]);
    }
    ui->combox_sort->setCurrentIndex(0);

    // Load Sort Select Value
    for (int i = 0;i < m_level_value.size();i++){
        ui->combox_sort_level->addItem(m_level_value[i]);
    }
    ui->combox_sort_level->setCurrentIndex(0);

    for (int i = 0;i < m_sort_value.size();i++){
        ui->combox_sort_sort->addItem(m_sort_value[i]);
    }
    ui->combox_sort_sort->setCurrentIndex(0);
}

void MainWindow::on_actionopen_triggered()
{
    if(m_file_object.isOpen()){
        m_file_object.close();
    }
    QString temp = QFileDialog::getOpenFileName(this,"Open File",QString(),"*mp3 ");
    if (temp.indexOf(".mp3") == -1){
        return;
    }
    log_write("Loading file:"+temp);
    m_filepath = temp;
    m_filebase = QFileInfo(m_filepath).baseName();
    ui->label_info->setText(m_filebase);

    // 1. 清除与设置界面显示信息
    // 2. 清除之前文件信息
    // 3. 载入音频文件的json文件
    // 4. 刷新界面

    // 1
    ui->label_beg_point->clear();
    ui->label_end_point->clear();
    ui->combox_level->setCurrentIndex(0);
    ui->combox_sort->setCurrentIndex(0);
    ui->combox_sort_level->setCurrentIndex(0);
    ui->combox_sort_sort->setCurrentIndex(0);

    // 2
    m_mark_vec.clear();
    m_filepath = temp;
    m_beg_point = 0;
    m_play = false;
    m_type_content.clear();
    m_note_content.clear();
    m_level_index = 0;
    m_sort_index = 0;
    m_mark_index = -1;

    // 3
    m_music.setMedia(QUrl::fromLocalFile(m_filepath));
    loadJsonFile();

    // 4
    updateListWidget(false);
    loadControl(true);

    log_write(QString("Loaded[%1]").arg(m_beg_point));
}
void MainWindow::loadJsonFile()
{
    m_file_name = "./"+m_filebase.trimmed()+".json";
    m_file_object.setFileName(m_file_name);

    if (!m_file_object.exists()){
        log_write("File(json) Created");
        m_file_object.open(QIODevice::ReadWrite | QIODevice::Text);
    }
    else{
        log_write("FILE(json) Existed");
        m_file_object.open(QIODevice::ReadWrite | QIODevice::Text);
        loadJsonContent();
    }
}

void MainWindow::loadFileDir()
{
    QDir dir;
    if (!dir.exists(m_file_saveDir)){
        dir.mkdir(m_file_saveDir);
    }
    // 设置全局工作目录
    dir.setCurrent(m_file_saveDir);
}

void MainWindow::on_button_play_clicked()
{
    if (m_play == true){
        m_music.pause();
        m_play = false;
        ui->button_play->setText("Play");
    }else{
        m_music.play();
        m_play = true;
        ui->button_play->setText("Pause");
    }
}

void MainWindow::on_button_stop_clicked()
{
    m_music.stop();
    music_after_stop();
}

void MainWindow::on_media_updatePosition(qint64 position)
{
    // qDebug()<<(position);
    ui->horizon_music->setValue(position);
    ui->label_elapse_time->setText(GetFormatTime(position));
    // qDebug() << QString("%1:%2").arg(m_music.position()).arg(m_end_point);
    if (m_music.position() >= m_end_point+1/*+1防止在结束时返回的技巧*/){
        m_music.setPosition(m_beg_point);
    }
}

void MainWindow::on_media_updateDuration(qint64 duration)
{
    //qDebug()<<(duration);
    ui->horizon_music->setRange(0,duration);
    ui->horizon_music->setEnabled(duration>0);
    ui->horizon_music->setPageStep(duration/10);
    m_total_time = duration;
    m_end_point = m_total_time;
    ui->label_totle_time->setText("| "+GetFormatTime(m_total_time));
}

void MainWindow::on_media_stateChanged()
{
    if (QMediaPlayer::State::StoppedState == m_music.state()){
        music_after_stop();
    }
}

void MainWindow::set_media_position(qint64 duration)
{
    m_music.setPosition(duration);
}

void MainWindow::loadConnect()
{
    connect(&m_music,&QMediaPlayer::positionChanged,this,&MainWindow::on_media_updatePosition);
    connect(&m_music,&QMediaPlayer::durationChanged,this,&MainWindow::on_media_updateDuration);
    connect(&m_music,&QMediaPlayer::stateChanged,this,&MainWindow::on_media_stateChanged);

    connect(ui->horizon_music,&QSlider::sliderMoved,this,&MainWindow::set_media_position);
}

void MainWindow::on_button_back_clicked()
{
    m_music.setPosition(m_music.position() - m_step_millSecond > 0 ? m_music.position() - m_step_millSecond:0);
}

void MainWindow::on_button_forward_clicked()
{
    m_music.setPosition(m_music.position() + m_step_millSecond < m_total_time ? m_music.position() + m_step_millSecond:m_total_time);
}

void MainWindow::on_button_set_start_clicked()
{
    m_beg_point = m_music.position();
    ui->label_beg_point->setText(GetFormatTime(m_beg_point));
    // 设置开始的时候要清空结束点
    m_end_point = m_total_time;
    ui->label_end_point->clear();
}

void MainWindow::on_button_set_end_clicked()
{
    m_end_point = m_music.position();
    ui->label_end_point->setText(GetFormatTime(m_end_point));
    // 回到最开始的位置播放
    m_music.setPosition(m_beg_point);

}

void MainWindow::on_button_clear_beg_point_clicked()
{
    m_beg_point = 0;
    ui->label_beg_point->clear();
}

void MainWindow::on_button_clear_end_point_clicked()
{
    m_end_point = m_total_time;
    ui->label_end_point->clear();
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    // 载入选中item信息
    m_mark_index = ui->listWidget->currentRow();
    QJsonObject &select_json = m_mark_vec[m_mark_index];
    m_beg_point = select_json["beg_point"].toInt();
    m_end_point = select_json["end_point"].toInt();
    m_type_content = select_json["type"].toString();
    m_note_content = select_json["note"].toString();
    m_level_index= select_json["level_index"].toInt();
    m_sort_index= select_json["sort_index"].toInt();

    // 设置断点信息
    ui->label_beg_point->setText(GetFormatTime(m_beg_point));
    ui->label_end_point->setText(GetFormatTime(m_end_point));

    // 设置难度以及种类
    ui->combox_level->setCurrentIndex(m_level_index);
    ui->combox_sort->setCurrentIndex(m_sort_index);

    // 填充Text内容
    SetPlainContent();

    // 开始在断点起点播放
    music_play(m_beg_point);
}

void MainWindow::music_play(qint64 point){
    m_music.pause();
    m_music.setPosition(point);
    m_music.play();
    m_play = true;
    ui->button_play->setText("Pause");
}

void MainWindow::on_button_delete_clicked()
{
    log_write(QString("Deleting:Index:%1 MarksSize:%2").arg(m_mark_index).arg(m_mark_vec.size()));
    // 确保index安全
    if (m_mark_index < 0 && m_mark_index >= m_mark_vec.size()){
        return ;
    }
    // 在内存中删除指定item
    m_mark_vec.erase(m_mark_vec.begin() + m_mark_index);

    // 将结果保存至文件
    saveMarkIntoFile();

    // 确保index不会误指
    m_mark_index = -1;

    // 刷新listWidget
    updateListWidget(false);
    ui->button_delete->setEnabled(false);
    log_write(QString("Deleted:MarksSize:%1").arg(m_mark_vec.size()));
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    m_mark_index = ui->listWidget->currentRow();
    ui->button_delete->setEnabled(true);
}

void MainWindow::on_combox_sort_level_activated(int index)
{
    QVector<int> later_show;
    size_t i = 0;
    ui->listWidget->clear();
    // 加载满足条件的item
    for (QVector<QJsonObject>::iterator it=m_mark_vec.begin(); it!=m_mark_vec.end();++it,++i){
        if ((*it)["level_index"].toInt() == index){
            updateListWidget(true,*it);
        }else{
            later_show.push_back(i);
        }
    }
    // 加载剩余不满足条件的item
    for (QVector<int>::iterator it=later_show.begin(); it!=later_show.end();++it){
        updateListWidget(true,m_mark_vec.at(*it));
    }
}

void MainWindow::on_combox_sort_sort_activated(int index)
{
    QVector<int> later_show;
    size_t i = 0;
    ui->listWidget->clear();
    // 加载满足条件的item
    for (QVector<QJsonObject>::iterator it=m_mark_vec.begin(); it!=m_mark_vec.end();++it,++i){
        if ((*it)["sort_index"].toInt() == index){
            updateListWidget(true,*it);
        }else{
            later_show.push_back(i);
        }
    }
    // 加载剩余不满足条件的item
    for (QVector<int>::iterator it=later_show.begin(); it!=later_show.end();++it){
        updateListWidget(true,m_mark_vec.at(*it));
    }
}

void MainWindow::on_howToUse_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/Gltina/MarkSencent"));
}

void MainWindow::on_about_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/Gltina/MarkSencent"));
}

void MainWindow::on_combox_level_activated(int index)
{
    m_level_index = index;
}

void MainWindow::on_combox_sort_activated(int index)
{
    m_sort_index = index;
}

void MainWindow::on_exit_triggered()
{
    if (m_file_object.isOpen()){
        m_file_object.close();
    }
    if(m_log_object.isOpen()){
        m_log_object.close();
    }
    this->destroy();
    QApplication::exit();
}

void MainWindow::closeEvent(QCloseEvent *event){
    on_exit_triggered();
    QApplication::exit();
}

void MainWindow::on_onlineFile_triggered()
{
    pDislog = new Dialog(this);
    pDislog->setParent(this);
    pDislog->show();
}
