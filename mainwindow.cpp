#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_loadDialog(Q_NULLPTR)
{
    this->setWindowTitle("MarkSentence");
    ui->setupUi(this);

    // 初始化数据
    loadConstant();
    loadTableWidget();

    // 加载界面中的下拉菜单
    loadComboxItems();

    // 初始化槽连接
    loadConnect();

    // 初始化界面控制
    loadControl(false);

}

MainWindow::~MainWindow()
{
    if (m_json_object.isOpen()){
        m_json_object.close();
    }
    if (m_log_object.isOpen()){
        m_log_object.close();
    }
    delete ui;
}

void MainWindow::clearText(){
    ui->text_type->clear();
    ui->text_type->setPlainText("");
    ui->text_note->clear();
    ui->text_note->setPlainText("");
    ui->combox_level->setCurrentIndex(0);
    ui->combox_sort->setCurrentIndex(0);

}

bool MainWindow::generateLyricFileName()
{
    // 目前只针对听力文件有用
    if (m_filebase.contains("Listening",Qt::CaseSensitive)){
        // 获取"_"的索引
        QVector<int>indexvec;
        for (int i=0;i<m_filebase.size();i++) {
            if (m_filebase[i] == '_')
                indexvec.push_back(i);
        }
        // 正确的是三个
        if(indexvec.size() == 3){
            int id = m_filebase.mid(indexvec[0]+1,indexvec[1]-indexvec[0]-1).toInt();
            QString detail = m_filebase.mid(indexvec[2]+1);
            // qDebug() << id << " "<<detail;
            m_lyric_name = QString("TPO%1%2.txt").arg(id,2,10,QChar('0')).arg(detail);
            // qDebug()<< m_lyric_name;
        }
        logWrite(QString("Loading lyric file:%1").arg(m_lyric_dir+ m_lyric_name));
        return true;
    }
    return false;
}

void MainWindow::loadLyric2Map()
{
    // qDebug() << QDir::currentPath();

    m_lyric_object.setFileName(m_lyric_dir + m_lyric_name);

    if (!m_lyric_object.exists()){
         logWrite(QString("No lyric file:%1").arg(m_lyric_object.fileName()));
        m_exist_lyric = false;
        return;
    }
    m_lyric_object.open(QIODevice::ReadOnly);
    if (!m_lyric_object.isOpen()){
        m_exist_lyric = false;
        logWrite(QString("Cannot Load lyric file:%1").arg(m_lyric_object.fileName()));
        return ;
    }
    logWrite(QString("Loaded lyric file:%1").arg(m_lyric_object.fileName()));
    m_exist_lyric = true;
    QByteArray t ;
    double sentence_time;
    QString thisSentence;
    while(!m_lyric_object.atEnd())
    {
        t = m_lyric_object.readLine();
        thisSentence = QString(t);

        for (int i=0;i<thisSentence.size();i++){
            if (thisSentence[i] == ' '){
                // qDebug() << thisSentence.mid(0,i);
                sentence_time = thisSentence.mid(0,i).toDouble() * 1000;
                m_lyric_map.insert(sentence_time,thisSentence.mid(i+1).replace("\r\n",""));
                break;
            }// end if
        }// end for
    }// end while
    //ui->text_r->setText(QString(t));
    // qDebug() << m_lyric_map;
    m_lyric_object.close();
    logWrite(QString("Loaded lyric file, size:%1").arg(m_lyric_map.size()));
}

void MainWindow::showLyric()
{
    if (m_lyric_map.isEmpty()) {
        if (m_exist_lyric == false){
            ui->textBrowser_2->setText("No Lyric");
        }
        return ;
    }
    // 用index来辅助计数
    int index = 0;
    for (QMap<int,QString>::iterator it = m_lyric_map.begin();
         it != m_lyric_map.end(); ++it,++index) {

        if (m_music.position() < it.key()){
            ui->textBrowser_1->setText("");
            ui->textBrowser_2->setText("");
            ui->textBrowser_3->setText("");
            // 设置第一句
            if (index-2 >= 0) {
                ui->textBrowser_1->setText((it-2).value());
            }
            // 设置中间的lyric
            if (index-1 >= 0) {
                ui->textBrowser_2->setText((it-1).value());
            }
            // 设置第三句
            if (index < m_lyric_map.size()) {
                ui->textBrowser_3->setText((it).value());
            }
            break;
        }// end if
    }// end for
}

void MainWindow::on_button_save_clicked()
{
    logWrite(QString("Saving:Segment ["+getFormatTime(m_beg_point)+"-"+getFormatTime(m_end_point)+"] NoteLength:%1 TypeLength:%2 Selected %3,%4").
              arg(m_note_content.size()).arg(m_type_content.size()).
              arg(m_level_index).arg(m_sort_index));

    // 保存到内存，不是保存到文件
    createOneMark();

    // 清除界面数据
    clearText();

    // 保存到文件
    saveMarkIntoFile();

    // 刷新界面
    updateTableWidget();

    logWrite("Saved");
}

void MainWindow::loadTableWidget(){
    QStringList table_header;
    table_header << "Time" <<"Level" << "Content";
    ui->tableWidget->setColumnCount(table_header.size());
    ui->tableWidget->setHorizontalHeaderLabels(table_header);
    // 设置选中整行
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 设置不能编辑
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置为只能选中当个目标
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    // 设置表头不可选
    ui->tableWidget->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
}

void MainWindow::sortby(const QString key,QVector<int> & result_vec){
    QVector<int> value_seq;
    for (int i = 0;i<m_json_vec.size();i++){
        value_seq.push_back(m_json_vec[i][key].toInt());
    }
    //qDebug() << value_seq;
    int num,index,j;
    for (int i = 0; i<value_seq.size();i++){
        // 找一个有效值当基准
        for (j=0;j <value_seq.size();j++){
            if (value_seq[j] != -1){
                num = value_seq[j];
                index = j;
                break;
            }
        }
        //qDebug() << "take"<<num<<"("<<index<<")"<<"for benchmark";
        //找到匹配值，找到最小值
        for (int n = 0;n < value_seq.size();n++){
            if (value_seq[n]  != -1 && value_seq[n] < num){
                num = value_seq[n];
                index = n;
            }
        }
        //qDebug() << "found"<<num<<"("<<index<<")";
        value_seq[index]  = -1;
        result_vec.push_back(index);
    }
    //qDebug() << value_seq;
}

void MainWindow::on_clickHeader(int col)
{
    QVector<int>result_vec;
    // 按照标记时间排序
    // 原则： 从开始到结束
    if (col == 0){
        sortby("beg_point",result_vec);
        //qDebug()<<result_vec;
        updateTableWidget(result_vec);
    } // end col == 0
    // 按照难度排序
    // 原则： 顺序依据列表
    else if (col == 1){
        sortby("level_index",result_vec);
        //qDebug()<<result_vec;
        updateTableWidget(result_vec);
    }
    // 按照内容排序
    // 原则： 顺序依据列表
    else if(col == 2){
        sortby("sort_index",result_vec);
        //qDebug()<<result_vec;
        updateTableWidget(result_vec);
    }
    QVector<QJsonObject> temp_json_vec;
    for (int i = 0; i < result_vec.size() ; i++){
        temp_json_vec.push_back(m_json_vec[result_vec[i]]);
    }
    qSwap(temp_json_vec,m_json_vec);
}

void MainWindow::on_showMenu(const QPoint pos)
{
    //设置菜单选项
        QMenu *menu = new QMenu(ui->tableWidget);
        QAction *pnew = new QAction(QIcon(":/icon/icon/delete.png"),"Delete",ui->tableWidget);\
        connect (pnew,SIGNAL(triggered()),this,SLOT(on_deleteItem()));

        menu->addAction(pnew);
        menu->move (cursor().pos ());
        menu->show ();

        //获得鼠标点击的x，y坐标点
        int x = pos.x ();
        int y = pos.y ();
        QModelIndex index = ui->tableWidget->indexAt (QPoint(x,y));
        m_click_row = index.row ();//获得QTableWidget列表点击的行数
}

void MainWindow::on_deleteItem()
{
    // qDebug() <<"delete:"<<m_click_row;
    if (m_click_row == -1){
        // qDebug() << "No Target";
        return ;
    }

    logWrite(QString("Deleting:Index:%1 MarksSize:%2").arg(m_click_row).arg(m_json_vec.size()));
    // 确保index安全
    if (m_click_row < 0 && m_click_row >= m_json_vec.size()){
        return ;
    }
    // 在内存中删除指定item
    m_json_vec.erase(m_json_vec.begin() + m_click_row);

    // 将结果保存至文件
    saveMarkIntoFile();

    // 确保index不会误指
    m_click_row = -1;

    // 刷新listWidget
    updateTableWidget();

    logWrite(QString("Deleted:MarksSize:%1").arg(m_json_vec.size()));
}

QString MainWindow::getPlainContent(QPlainTextEdit * plainText)
{
    QTextDocument * doc = plainText->document();
    QTextBlock block = doc->firstBlock();
    QString text;

    for(int i = 0; i < doc->blockCount();i++,block=block.next()){
        text = text + block.text() + '\n';
    }
    return text;
}

void MainWindow::setPlainContent(){
    ui->text_type->setPlainText(m_type_content);
    ui->text_note->setPlainText(m_note_content);
}


QString MainWindow::getFormatTime(qint64 time)
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

QString MainWindow::getFormatMark(const QJsonObject &json)
{
    QString mark;
    // 获取起止时间
    QString beg_point = getFormatTime(json["beg_point"].toInt()),
            end_point = getFormatTime(json["end_point"].toInt());
    mark = "["+beg_point +"-"+end_point+"]\t";
    // 追加难度和种类
    mark += m_level_value[json["level_index"].toInt()] + "\t";
    mark += m_sort_value[json["sort_index"].toInt()];
    return mark;
}

QString MainWindow::getFormatContent(const QJsonObject &json, int index){
    // 获取时间的格式化数据
    if(index == 0){
        QString beg_point = getFormatTime(json["beg_point"].toInt()),
                end_point = getFormatTime(json["end_point"].toInt());
        return QString("["+beg_point +"-"+end_point+"]");
    }
    // 获取等级的格式化数据
    else if(index == 1) {
        return m_level_value[json["level_index"].toInt()];
    }
    // 获取内容的格式化数据
    else if(index == 2){
        return m_sort_value[json["sort_index"].toInt()];
    }
    return QString();
}

void MainWindow::loadConstant()
{
    // 初始化下拉菜单的选项
    QString
            level_value = "Easy,Midd,Hard",
            sort_value = "None,Unfamiliar Words,Too Long,Garmmar,Speak Fast";
    m_level_value =level_value.split(",");
    m_sort_value = sort_value.split(",");
    m_level_index = 0;
    m_sort_index = 0;

    // 初始化音频播放相关数据
    m_music.setNotifyInterval(100);
    m_play = false;
    m_beg_point = 0;
    m_end_point = 0;
    m_total_time = 0;
    m_step_millSecond = 3000; // 3s

    // 初始化标记数据
    m_click_row = -1;
    m_clicked_btn_end = false;
    m_clicked_btn_beg = false;
    m_exist_lyric = false;
    m_json_vec.clear();
    ui->tableWidget->clear();

    // 创建所有需要的目录
    loadFileDir();

     // 用于文件读写
    m_log_name = "log";
    createLogFile();
}

void MainWindow::fileOpenIsFailed(QString title,QString content){
    QMessageBox::about(NULL, title, content);
}

void MainWindow::logWrite(QString info){
    if (!m_log_object.isOpen()){
        return;
    }
    m_log_object.write((info+'\n').toStdString().c_str());
}

void MainWindow::createLogFile(){
    m_log_object.setFileName(m_log_dir+ m_log_name);
    if (!m_log_object.open(QIODevice::WriteOnly | QIODevice::Text)){
        fileOpenIsFailed("文件打开失败","详细信息请查阅log文件或反馈");
    }
}
void MainWindow::loadControl(bool status)
{
    //ui->text_type->clear();
    //ui->text_note->clear();
    if (!status){
        ui->button_play->setEnabled(false);
        ui->button_stop->setEnabled(false);
        ui->button_back->setEnabled(false);
        ui->button_forward->setEnabled(false);
        ui->button_set_end->setEnabled(false);
        ui->button_set_start->setEnabled(false);
        ui->button_save->setEnabled(false);
        ui->horizon_music->setEnabled(false);
    }
    else{
        ui->button_play->setEnabled(true);
        ui->button_stop->setEnabled(true);
        ui->button_back->setEnabled(true);
        ui->button_forward->setEnabled(true);
        ui->button_set_end->setEnabled(true);
        ui->button_set_start->setEnabled(true);
        ui->button_save->setEnabled(true);
        ui->horizon_music->setEnabled(true);
    }
}


void MainWindow::createOneMark()
{
    QJsonObject curr_seg;

    m_type_content = getPlainContent(ui->text_type);
    m_note_content = getPlainContent(ui->text_note);

    curr_seg.insert("type", m_type_content);
    curr_seg.insert("note", m_note_content);
    curr_seg.insert("beg_point", m_beg_point);
    curr_seg.insert("end_point", m_end_point);
    curr_seg.insert("level_index", m_level_index);
    curr_seg.insert("sort_index", m_sort_index);

    m_json_vec.push_back(curr_seg);
}

void MainWindow::saveMarkIntoFile()
{
    // 创建json列表用于保存
    // "序号":"标记内容(json)"
    QJsonObject save_json;
    int index = 0;
    for (QVector<QJsonObject>::iterator it =  m_json_vec.begin(); it !=  m_json_vec.end();++it,++index){
        save_json.insert(QString(index),*it);
    }
    QJsonDocument jsonDoc(save_json);

    // 将文件清空
    if(m_json_object.isOpen()){
        m_json_object.close();
    }
    m_json_object.open(QIODevice::ReadWrite|QIODevice::Truncate);
    // 写入文件
    m_json_object.write(jsonDoc.toJson());
    m_json_object.flush();
}

void MainWindow::loadJsonContent()
{
    QByteArray data = m_json_object.readAll();

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
        m_json_vec.push_back(curr_json_seg);
    }
}

void MainWindow::updateTableWidget(QVector<int> &index_vec){
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();
    int rowCount = 0;
    // 使用索引表刷新Table
    if (!index_vec.isEmpty()){
        // qDebug() << "flashed by index_vec";
        // qDebug() << index_vec;
        for (int i = 0;i <index_vec.size();++i){
            rowCount = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(rowCount);
            for (int c = 0;c<3;c++){
                ui->tableWidget->setItem(
                            rowCount,c,
                            new QTableWidgetItem(getFormatContent(m_json_vec[index_vec[i]],c)));
            }
        }
    }
    // 使用默认顺序刷新
    else {
        // qDebug() << "flashed by index_default";
        for (int r = 0;r < m_json_vec.size();r++){
            rowCount = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(rowCount);
            for (int c = 0;c<3;c++){
                ui->tableWidget->setItem(
                            rowCount,c,
                            new QTableWidgetItem(getFormatContent(m_json_vec[r],c)));
            }
        }
    }
}

void MainWindow::musicAfterStop()
{
    m_play = false;
    m_music.setPosition(0);
    m_beg_point = 0;
    m_clicked_btn_beg = false;
    m_clicked_btn_end = false;
    m_end_point = m_total_time;
    ui->button_set_start->setText("");
    ui->button_set_end->setText("");
    ui->button_play->setIcon(QIcon(":/icon/icon/play.png"));
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

}

void MainWindow::on_localFile_triggered()
{
    if(m_json_object.isOpen()){
        m_json_object.close();
    }
    QString temp = QFileDialog::getOpenFileName(this,"Open File",QString(),"*mp3 ");
    if (temp.indexOf(".mp3") == -1){
        return;
    }
    logWrite("Loading Audio File:"+temp);
    m_filepath = temp;
    m_filebase = QFileInfo(m_filepath).baseName();
    ui->label_info->setText(m_filebase);

    // 1. 清除与设置界面显示信息
    // 2. 清除之前文件信息
    // 3. 刷新界面
    // 4. [optional]加载"歌词"
    // 5. 载入音频文件以及json文件

    // 1. 清除与设置界面显示信息
    ui->button_set_start->setText("");
    ui->button_set_end->setText("");
    ui->combox_level->setCurrentIndex(0);
    ui->combox_sort->setCurrentIndex(0);
    ui->textBrowser_1->clear();
    ui->textBrowser_2->clear();
    ui->textBrowser_3->clear();

    // 2. 清除之前文件信息
    m_json_vec.clear();
    m_filepath = temp;
    m_beg_point = 0;
    m_play = false;
    clearText();
    m_level_index = 0;
    m_sort_index = 0;
    m_lyric_map.clear();
    m_lyric_name.clear();

    // 3. 刷新界面
    updateTableWidget();
    loadControl(true);

    // 4. [optional]加载"歌词"
    // 如果是listening部分的歌词请求
    if (generateLyricFileName()){
        loadLyric2Map();
    }
    else {
        m_exist_lyric = false;
    }

    // 5. 载入音频文件以及json文件
    m_music.setMedia(QUrl::fromLocalFile(m_filepath));
    loadJsonFile();

    logWrite(QString("Loaded"));
}
void MainWindow::loadJsonFile()
{
    m_json_name = m_json_dir+m_filebase.trimmed()+".json";
    m_json_object.setFileName(m_json_name);

    if (!m_json_object.exists()){
        logWrite("File(json) Created");
        m_json_object.open(QIODevice::ReadWrite | QIODevice::Text);
    }
    else{
        logWrite("FILE(json) Existed");
        m_json_object.open(QIODevice::ReadWrite | QIODevice::Text);
        loadJsonContent();
    }
}

bool MainWindow::createDir(QString dirname){
    QDir dir;
    if (!dir.exists(dirname)){
        logWrite(QString("Creating dirfile: %1").arg(dirname));
        return dir.mkdir(dirname);

    }
    else {
        logWrite(QString("%1(Dirfile) Existed ").arg(dirname));
        // ..
    }
    return true;
}


void MainWindow::loadFileDir()
{
    // Json 文件存储目录
    m_json_dir = "Json/";
    createDir(m_json_dir);

    // Log文件存储目录
    m_log_dir = "Log/";
    createDir(m_log_dir);

    // lyric 文件存储目录
    m_lyric_dir = "Lyric/";
    createDir(m_lyric_dir);

    // TPO文件的保存目录
    m_tpo_dir = "TPO";
    createDir(m_tpo_dir);


}

void MainWindow::on_button_play_clicked()
{
    if (m_play == true){
        m_music.pause();
        m_play = false;
        ui->button_play->setIcon(QIcon(":/icon/icon/play.png"));
    }else{
        m_music.play();
        m_play = true;
       ui->button_play->setIcon(QIcon(":/icon/icon/pause.png"));
    }
}

void MainWindow::on_button_stop_clicked()
{
    m_music.stop();
    musicAfterStop();
}

void MainWindow::on_media_updatePosition(qint64 position)
{
    // qDebug()<<(position);
    ui->horizon_music->setValue(position);
    ui->label_elapse_time->setText(getFormatTime(position));
    // qDebug() << QString("%1:%2").arg(m_music.position()).arg(m_end_point);
    if (m_music.position() >= m_end_point+1/*+1防止在结束时返回的技巧*/){
        m_music.setPosition(m_beg_point);
    }
    showLyric();
}

void MainWindow::on_media_updateDuration(qint64 duration)
{
    //qDebug()<<(duration);
    ui->horizon_music->setRange(0,duration);
    ui->horizon_music->setEnabled(duration>0);
    ui->horizon_music->setPageStep(duration/10);
    m_total_time = duration;
    m_end_point = m_total_time;
    ui->label_totle_time->setText("| "+getFormatTime(m_total_time));
}

void MainWindow::on_media_stateChanged()
{
    if (QMediaPlayer::State::StoppedState == m_music.state()){
        musicAfterStop();
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
    connect(ui->tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(on_clickHeader(int)));
    connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(on_showMenu(QPoint)));
}

void MainWindow::on_button_back_clicked()
{
    if (m_exist_lyric == false){
        m_music.setPosition(m_music.position() - m_step_millSecond > 0 ? m_music.position() - m_step_millSecond:0);
    }
    else {
        int index = 0;
        qint64 curr_pos = m_music.position();
        for (QMap<int,QString>::iterator it = m_lyric_map.begin();
             it != m_lyric_map.end(); ++it,++index) {
                if (curr_pos < it.key()){
                    m_music.setPosition((index-2 >= 0)?(it-2).key():0);
                    break;
                }
                if (it + 1 == m_lyric_map.end()) {
                    m_music.setPosition((index-2 >= 0)?(it-2).key():0);
                    break;
                }
        }
    }
}

void MainWindow::on_button_forward_clicked()
{
    if (m_exist_lyric == false){
        m_music.setPosition(m_music.position() + m_step_millSecond < m_total_time ? m_music.position() + m_step_millSecond:m_total_time);
    }
    else {
        int index = 0;
        qint64 curr_pos = m_music.position();
        for (QMap<int,QString>::iterator it = m_lyric_map.begin();
             it != m_lyric_map.end(); ++it,++index) {
                if (curr_pos < it.key()){
                    m_music.setPosition(it.key());
                    break;
                }
                if (it + 1 == m_lyric_map.end()) {
                    m_music.setPosition(it.key());
                    break;
                }
        }
    }
}

void MainWindow::on_button_set_start_clicked()
{
    // 取消断点
    if (m_clicked_btn_beg){
        m_beg_point = 0;
        ui->button_set_start->setText("");
        m_clicked_btn_beg = false;
     }
    // 记录断点
    else{
        m_beg_point = m_music.position();
        ui->button_set_start->setText(getFormatTime(m_beg_point));
        m_clicked_btn_beg = true;
    }
}

void MainWindow::on_button_set_end_clicked()
{
    // 取消断点
    if (m_clicked_btn_end){
        m_end_point = m_total_time;
        ui->button_set_end->setText("");
        m_clicked_btn_end = false;
     }
    // 记录断点
    else{
        m_end_point = m_music.position();
        ui->button_set_end->setText(getFormatTime(m_end_point));
        m_clicked_btn_end = true;
        m_music.setPosition(m_beg_point);
    }
}

void MainWindow::musicPlay(qint64 point){
    m_music.pause();
    m_music.setPosition(point);
    m_music.play();
    m_play = true;
    ui->button_play->setIcon(QIcon(":/icon/icon/pause.png"));
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
    if (m_json_object.isOpen()){
        m_json_object.close();
    }
    if(m_log_object.isOpen()){
        m_log_object.close();
    }
    m_music.stop();
    QApplication::exit();
}

void MainWindow::closeEvent(QCloseEvent *event){
    switch( QMessageBox::information(this,tr("MarkSentence"),tr("Are you sure?"),tr("YES"), tr("NO"),0,1))
    {
    case 0:
        event->accept();
        break;
    case 1:
    default:
        event->ignore();
        break;
    }
}

void MainWindow::on_onlineFile_triggered()
{
    m_loadDialog = new loadDialog(m_log_object,0);
    int res = m_loadDialog->exec();
    if (res == QDialog::Accepted)
    {
        delete m_loadDialog;
    }
}

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    column=column;
    m_click_row = row;
    m_clicked_btn_beg = true;
    m_clicked_btn_end = true;
    QJsonObject &select_json = m_json_vec[m_click_row];
    m_beg_point = select_json["beg_point"].toInt();
    m_end_point = select_json["end_point"].toInt();
    m_type_content = select_json["type"].toString();
    m_note_content = select_json["note"].toString();
    m_level_index= select_json["level_index"].toInt();
    m_sort_index= select_json["sort_index"].toInt();

    // 设置断点信息
    ui->button_set_start->setText(getFormatTime(m_beg_point));
    ui->button_set_end->setText(getFormatTime(m_end_point));

    // 设置难度以及种类
    ui->combox_level->setCurrentIndex(m_level_index);
    ui->combox_sort->setCurrentIndex(m_sort_index);

    // 填充Text内容
    setPlainContent();

    // 开始在断点起点播放
    musicPlay(m_beg_point);
}
