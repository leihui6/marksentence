#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_loadDialog(Q_NULLPTR),
  ui(new Ui::MainWindow)
{
    this->setWindowTitle("MarkSentence");
    ui->setupUi(this);

    // 初始化数据
    loadConstant();

    loadTableWidget();

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
        // 正确情况下的是三个
        if(indexvec.size() == 3){
            int id = m_filebase.mid(indexvec[0]+1,indexvec[1]-indexvec[0]-1).toInt();
            QString detail = m_filebase.mid(indexvec[2]+1);
            //qDebug() << id << " "<<detail;
            m_lyric_name = QString("TPO%1%2.txt").arg(id,2,10,QChar('0')).arg(detail);
#ifdef MAINDEBUG
            qDebug()<<"[MainWindow]"<<"generateLyricFileName()"<<"m_lyric_name"<< m_lyric_name;
#endif

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

    // write into the lyricVec
    for (auto i:m_lyric_map){
        m_lyricVec.push_back(stylized(i));
    }

    m_lyric_object.close();
    logWrite(QString("Loaded lyric file, size:%1").arg(m_lyric_map.size()));
}

void MainWindow::goToSpecificLine(int line){
    QTextCursor tc = ui->textEdit->textCursor();
    int toPost = ui->textEdit->document()->findBlockByNumber( line - 1).position();
    tc.setPosition(toPost,QTextCursor::MoveAnchor);
    ui->textEdit->setTextCursor(tc);
}

void MainWindow::showLyric()
{
    // --------------------
    if (m_lyric_map.isEmpty()) {
        if (m_exist_lyric == false){
            //ui->textBrowser_2->setText("No Lyric");
        }
        return ;
    }
    // 用index来辅助计数
    int index = 0;
    for (QMap<int,QString>::iterator it = m_lyric_map.begin();
         it != m_lyric_map.end(); ++it,++index) {

        if (m_music.position() < it.key()){\
            if (index-1 >= 0) {
                break;
            }
        }
    }
    if(index-1 >= 0){

#ifdef  MAINDEBUG
  qDebug()  <<"[mainwindow]"<<"specific line is "<<index-1<<endl;
#endif
        ui->textEdit->clear();
        for (int i=0;i != m_lyricVec.size();++i){
            if (i == index-1){
                ui->textEdit->append(highLightLine(m_lyricVec[i]));
                continue;
            }
            ui->textEdit->append(m_lyricVec[i]);
        }
        goToSpecificLine(index-1);
    }
    else{
        ui->textEdit->clear();
        for (int i=0;i != m_lyricVec.size();++i){
            ui->textEdit->append(m_lyricVec[i]);
        }
        goToSpecificLine(0);
    }
}

QString MainWindow::stylized(QString & str){
    return "<center>"+str + "</center>" + "\n";
}

QString MainWindow::highLightLine(QString & str){
    return "<b>"+str+"</b>";
}

QString MainWindow::getSelectedWords(){
    return ui->textEdit->textCursor().selectedText() ;
}

void MainWindow::on_showTextMenu(const QPoint &point){
    if (getSelectedWords().isEmpty()) return ;

    QMenu *menu = new QMenu(ui->textEdit);
    const int sub_menu_size = G_reason_list.size();

    for (int i=0;i < sub_menu_size;i++){
        switch (i) {
        case 0:
            menu->addAction(QIcon(":/appicon/mark.png"),G_reason_list[i],this,SLOT(on_textMenuSelectedPhrase()));
            break;
        case 1:
            menu->addAction(QIcon(":/appicon/mark.png"),G_reason_list[i],this,SLOT(on_textMenuSelectedLinking()));
            break;
        case 2:
            menu->addAction(QIcon(":/appicon/mark.png"),G_reason_list[i],this,SLOT(on_textMenuSelectedGrammar()));
            break;
        case 3:
            menu->addAction(QIcon(":/appicon/mark.png"),G_reason_list[i],this,SLOT(on_textMenuSelectedToolong()));
            break;
        case 4:
            menu->addAction(QIcon(":/appicon/mark.png"),G_reason_list[i],this,SLOT(on_textMenuSelectedOthers()));
            break;
        default:
#ifdef MAINDEBUG
            qDebug() <<"[mainwindow]"<<"on_showTextMenu"<<"please add new on_fun to reflect the operation of clicking of menu"<<endl;
#endif
            break;
        }
    }
    menu->move (cursor().pos ());
    menu->show ();
#ifdef  MAINDEBUG
  qDebug()  <<"[mainwindow]"<<"on_showTextMenu()"<<getSelectedWords()<<endl;
#endif
}

void MainWindow::on_textMenuSelectedPhrase(){
    saveTextMenuSelected(0);
}
void MainWindow::on_textMenuSelectedLinking(){
    saveTextMenuSelected(1);
}
void MainWindow::on_textMenuSelectedGrammar(){
    saveTextMenuSelected(2);
}
void MainWindow::on_textMenuSelectedToolong(){
    saveTextMenuSelected(3);
}
void MainWindow::on_textMenuSelectedOthers(){
    saveTextMenuSelected(4);
}
void MainWindow::saveTextMenuSelected(int index){

#ifdef  MAINDEBUG
  qDebug()  <<"[mainwindow]"<<"User choices the:"<<G_reason_list[index]<<endl;
#endif

    logWrite(QString("Saving:Segment ["+getFormatTime(m_music.position())));

    createOneMark(m_music.position(),index,getSelectedWords());

    saveMarkIntoFile();

    updateTableWidget();

    ui->button_play->setFocus();

    logWrite("Saved");
}

void MainWindow::loadTableWidget(){
    QStringList table_header;
    //QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("时间")
    table_header
            <<QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("时间")
           <<QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("理由")
          << QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("文本");
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
    // 自适应列宽
    //ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    // 水平滑动条按照像素来滑动
    ui->tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    // 水平滑动条按照像素来滑动
    ui->tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    // 去掉网格
    ui->tableWidget->setShowGrid(false);
}

void MainWindow::sortby(const QString key,QVector<int> & result_vec){
    QVector<int> value_seq;
    for (int i = 0;i<m_json_vec.size();i++){
        value_seq.push_back(m_json_vec[i][key].toInt());
    }

#ifdef MAINDEBUG
    qDebug() << value_seq ;
#endif

    int num = 0,index = 0,j = 0;
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
    if (col == 0){
        sortby("time",result_vec);
        updateTableWidget(result_vec);
    }
    else if (col == 1){
        sortby("reason",result_vec);
        updateTableWidget(result_vec);
    }

    //else if(col == 2){
    //    sortby("content",result_vec);
    //    updateTableWidget(result_vec);
    //}

    QVector<QJsonObject> temp_json_vec;
    for (int i = 0; i < result_vec.size() ; i++){
        temp_json_vec.push_back(m_json_vec[result_vec[i]]);
    }
    qSwap(temp_json_vec,m_json_vec);
}

void MainWindow::on_showTableMenu(const QPoint pos)
{
    if (m_json_vec.isEmpty()){
        return;
    }
    //设置菜单选项
    QMenu *menu = new QMenu(ui->tableWidget);
    QAction *pnew = new QAction(QIcon(":/appicon/delete.png"),"Delete",ui->tableWidget);\
    connect (pnew,SIGNAL(triggered()),this,SLOT(on_deleteItem()));

    menu->addAction(pnew);
    menu->move (cursor().pos ());
    menu->show ();

    //获得鼠标点击的x，y坐标点
    int
            x = pos.x (),
            y = pos.y ();
    QModelIndex index = ui->tableWidget->indexAt (QPoint(x,y));
    //获得QTableWidget列表点击的行数
    m_click_row = index.row ();
}

void MainWindow::on_deleteItem()
{
    if (m_click_row == -1){
#ifdef  MAINDEBUG
  qDebug()  <<"[mainwindow]"<<"on_deleteItem()"<<"No Target"<<endl;
#endif
        return ;
    }
#ifdef  MAINDEBUG
  qDebug()  <<"[mainwindow]"<<"on_deleteItem()"<<"m_click_row:"<<m_click_row<<endl;
#endif

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

QString MainWindow::getFormatContent(const QJsonObject &json, int index){

    // 获取顺序依据Table中的Colume
    if(index == 0){
        return getFormatTime(json["time"].toInt());
    }
    else if(index == 1){
        return G_reason_list[json["reason"].toInt()];
    }
    else if (index == 2){
        return json["content"].toString();
    }
    return QString();
}

void MainWindow::loadConstant()
{
    // 初始化音频播放相关数据
    m_music.setNotifyInterval(300);
    m_play = false;
    m_beg_point = 0;
    m_end_point = 0;
    m_total_time = 0;

    // 初始化标记数据
    m_click_row = -1;
    m_exist_lyric = false;
    m_json_vec.clear();
    ui->tableWidget->clear();

    // 创建所有需要的目录
    loadFileDir();

    // 创建所需文件
    m_log_name = "log";
    createLogFile();
}

void MainWindow::fileOpenIsFailed(QString title,QString content){
    QMessageBox::about(nullptr, title, content);
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
        fileOpenIsFailed(
                    QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("文件打开失败"),
                    QTextCodec::codecForName(QByteArray("GBK"))->toUnicode(
                        "请尝试使用管理员权限运行程序或麻烦将问题反馈至github.com/gltina/"
                        ));
    }
}
void MainWindow::loadControl(bool status)
{
    if (!status){
        ui->button_play->setEnabled(false);
        ui->button_back->setEnabled(false);
        ui->button_forward->setEnabled(false);
        ui->horizon_music->setEnabled(false);
        ui->textEdit->setReadOnly(true);
    }
    else{
        ui->button_play->setEnabled(true);
        ui->button_back->setEnabled(true);
        ui->button_forward->setEnabled(true);
        ui->horizon_music->setEnabled(true);
        ui->textEdit->setReadOnly(true);
    }
}

void MainWindow::createOneMark(qint64 pos,int index,const QString & str)
{
    QJsonObject curr_seg;

    curr_seg.insert("time", pos);
    curr_seg.insert("reason", index);
    curr_seg.insert("content", str);

#ifdef  MAINDEBUG
  qDebug()  <<"[mainwindow]"<<"createOneMark()"<<curr_seg<<endl;
#endif

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

#ifdef  MAINDEBUG
  qDebug()  <<"[mainwindow]"<<"loadJsonContent()"<<"data"<<data<<endl;
#endif

    QJsonParseError e;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &e);

    if(e.error == QJsonParseError::NoError && !jsonDoc.isNull())
    {
#ifdef  MAINDEBUG
  qDebug()  <<"[mainwindow]"<<"loadJsonContent()"<<"jsonDoc"<<jsonDoc<<endl;
#endif
    }
    QJsonObject json = jsonDoc.object();

    QJsonObject curr_json_seg;
    for (QJsonObject::iterator it=json.begin();it != json.end();++it){
        curr_json_seg = it.value().toObject();
        m_json_vec.push_back(curr_json_seg);
    }
}

void MainWindow::updateTableWidget(QVector<int> index_vec){
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();
    int rowCount = 0;
    // Contain Time, Reason and Content
    int colCount = 3;

    // 使用索引表刷新Table
    if (!index_vec.isEmpty()){
#ifdef  MAINDEBUG
  qDebug()  <<"[mainwindow]"<<"updateTableWidget()"<<"flashed by index_vec:"<<index_vec<<endl;
#endif
        for (int i = 0;i <index_vec.size();++i){
            rowCount = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(rowCount);
            for (int c = 0;c<colCount;c++){
                ui->tableWidget->setItem(
                            rowCount,c,
                            new QTableWidgetItem(getFormatContent(m_json_vec[index_vec[i]],c)));
                ui->tableWidget->item(rowCount,c)->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    // 使用默认顺序刷新
    else {
#ifdef  MAINDEBUG
  qDebug()  <<"[mainwindow]"<<"updateTableWidget()"<<"flashed by index_default"<<endl;
#endif
        for (int r = 0;r < m_json_vec.size();r++){
            rowCount = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(rowCount);
            for (int c = 0;c<colCount;c++){
                ui->tableWidget->setItem(
                            rowCount,c,
                            new QTableWidgetItem(getFormatContent(m_json_vec[r],c)));
                ui->tableWidget->item(rowCount,c)->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
}

void MainWindow::musicAfterStop()
{
    m_play = false;
    m_music.setPosition(0);
    m_beg_point = 0;
    m_end_point = m_total_time;
    ui->button_play->setIcon(QIcon(":/appicon/play.png"));
}

void MainWindow::openLocalFile(QString filepath){
    if(filepath.isEmpty()){
        if(m_json_object.isOpen()){
            m_json_object.close();
        }
        QString temp = QFileDialog::getOpenFileName(this,"Open File",QString(),"*mp3 ");
        if (temp.indexOf(".mp3") == -1){
            return;
        }
        m_filepath = temp;
    }
    else{
        m_filepath = filepath;
    }
    logWrite("Loading Audio File:"+m_filepath);
    m_filebase = QFileInfo(m_filepath).baseName();
    ui->label_info->setText(m_filebase);

#ifdef MAINDEBUG
    qDebug()
            <<"[MainWindow]"<<"openLocalFile()"
           <<"m_filepath:"<<m_filepath<<endl
          <<"m_filebase"<<m_filebase<<endl;
#endif
    // 1. 清除与设置界面显示信息
    // 2. 清除之前文件信息
    // 3. 刷新界面
    // 4. [optional]加载"歌词"
    // 5. 载入音频文件以及json文件

    // 1. 清除与设置界面显示信息
    ui->textEdit->clear();
    ui->tableWidget->clearContents();

    // 2. 清除之前文件信息
    m_json_vec.clear();
    m_beg_point = 0;
    m_play = false;
    m_lyric_map.clear();
    m_lyric_name.clear();
    m_lyricVec.clear();

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

    // 3. 刷新界面
    updateTableWidget();
    loadControl(true);

    logWrite(QString("Loaded"));
}

void MainWindow::on_localFile_triggered()
{
    openLocalFile();
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
        ui->button_play->setIcon(QIcon(":/appicon/play.png"));
    }else{
        m_music.play();
        m_play = true;
        ui->button_play->setIcon(QIcon(":/appicon/pause.png"));
    }
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
    ui->label_totle_time->setText(getFormatTime(m_total_time));
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
    connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(on_showTableMenu(QPoint)));
    connect(ui->textEdit,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(on_showTextMenu(QPoint)));
}

void MainWindow::on_button_back_clicked()
{
    if (m_exist_lyric == false){
        m_music.setPosition(m_music.position() - G_stepSecond > 0 ? m_music.position() - G_stepSecond:0);
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
        m_music.setPosition(m_music.position() + G_stepSecond < m_total_time ? m_music.position() + G_stepSecond:m_total_time);
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

void MainWindow::musicPlay(qint64 point){
    m_music.pause();
    m_music.setPosition(point);
    m_music.play();
    m_play = true;
    ui->button_play->setIcon(QIcon(":/appicon/pause.png"));
}

void MainWindow::on_howToUse_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/Gltina/MarkSencent"));
}

void MainWindow::on_about_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/Gltina/MarkSencent"));
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

    switch( QMessageBox::information(this,
                                     tr("!!!!"),
                                     QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("不再听会儿？"),
                                     QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("算了我休息一下"),
                                     QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("再听最后亿篇"),
                                     nullptr,1))
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
    m_loadDialog = new loadDialog(m_log_object,nullptr);
    QFile qssFile(":/qdarkstyle/style.qss");
    if(qssFile.isOpen())
    {
        QString qss = QLatin1String(qssFile.readAll());
        m_loadDialog->setStyleSheet(qss);
        qssFile.close();
    }
    int res = m_loadDialog->exec();

    if (res == QDialog::Accepted)
    {
        if(G_playAfterDownload){
            openLocalFile(G_audioFilename);
        }
        // delete m_loadDialog;
    }
}

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    m_click_row = row;
    QJsonObject &select_json = m_json_vec[m_click_row];
    m_beg_point = select_json["time"].toInt();

    m_beg_point = m_beg_point-G_stepSecond >0?m_beg_point-G_stepSecond:0;
    // 开始在断点起点播放
    musicPlay(m_beg_point);
}

void MainWindow::on_setting_triggered()
{
    m_settingDialog = new SettingDialog();
    int res = m_settingDialog->exec();
    if (res == QDialog::Accepted)
    {
        //delete m_settingDialog;
    }
}
