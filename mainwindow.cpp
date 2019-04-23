#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Init constant variable
    loadConstant();

    // Init Widget
    loadComboxItems();
    loadListWidget();

    // Init Connect
    loadConnect();

    // Init Control
    loadControl(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_save_clicked()
{
    createOneMark();
    ui->text_type->clear();
    ui->text_note->clear();
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
    return QString("%1:%2:%3").arg(h).arg(m).arg(s);
}

QString MainWindow::GetFormatMark(QJsonObject &json)
{
    QString mark;
    QString beg_point = GetFormatTime(json["beg_point"].toInt());
    QString end_point = GetFormatTime(json["end_point"].toInt());
    mark = "["+beg_point +"-"+end_point+"]\t";
    mark += m_level_value[m_level_index] + "\t";
    mark += m_sort_value[m_sort_index];
    return mark;
}

void MainWindow::print(QString text)
{
    qDebug() << text;
}
void MainWindow::print(qint64 text)
{
    qDebug() << text;
}
void MainWindow::loadConstant()
{
    // Init Combox Value
    QString level_value = "Easy,Midd,Hard",
            sort_value = "None,Unfamiliar Words,Too Long,Garmmar";

    m_level_value =level_value.split(",");
    m_sort_value = sort_value.split(",");

    // Init Music Widget & Config
    m_music = new QMediaPlayer;
    m_play = false;
    m_beg_point = 0;
    m_end_point = 0;
    m_mark_index = -1;

    // Init Bind Event
    ui->button_play->setShortcut(Qt::Key_Enter);
}

void MainWindow::loadListWidget()
{

}

void MainWindow::loadControl(bool status)
{
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
    m_level_index = ui->combox_level->currentIndex();
    m_sort_index = ui->combox_sort->currentIndex();

    curr_seg.insert("type", m_type_content);
    curr_seg.insert("note", m_note_content);
    curr_seg.insert("beg_point", m_beg_point);
    curr_seg.insert("end_point", m_end_point);
    curr_seg.insert("level_index", m_level_index);
    curr_seg.insert("sort_index", m_sort_index);

    // Frist need to do
    m_mark_vec.push_back(curr_seg);

    // Update the ListWidget
    QListWidgetItem * item = new QListWidgetItem;
    item->setText(GetFormatMark(curr_seg));
    ui->listWidget->addItem(item);
}

void MainWindow::loadComboxItems()
{
    for (int i = 0;i < m_level_value.size();i++){
        ui->combox_level->addItem(m_level_value[i]);
    }
    ui->combox_level->setCurrentIndex(0);

    for (int i = 0;i < m_sort_value.size();i++){
        ui->combox_sort->addItem(m_sort_value[i]);
    }
    ui->combox_sort->setCurrentIndex(0);
}

void MainWindow::on_actionopen_triggered()
{
    m_filepath = QFileDialog::getOpenFileName(this,"Open File",QString(),"*mp3 ");
    m_filebase = QFileInfo(m_filepath).baseName();
    m_music->setMedia(QUrl::fromLocalFile(m_filepath));
    ui->label_info->setText(m_filebase);
    loadControl(true);
}

void MainWindow::on_button_play_clicked()
{
    if (m_play == true){
        m_music->pause();
        m_play = false;
        ui->button_play->setText("Play");
    }else{
        m_music->play();
        m_play = true;
        ui->button_play->setText("Pause");
    }
}

void MainWindow::on_button_stop_clicked()
{
    m_music->stop();
    m_play = false;
    m_music->setPosition(0);
    ui->button_play->setText("Play");
    ui->label_beg_point->clear();
    ui->label_end_point->clear();
}

void MainWindow::on_media_updatePosition(qint64 position)
{
    // qDebug()<<(position);
    ui->horizon_music->setValue(position);
    ui->label_elapse_time->setText(GetFormatTime(position));
    qDebug() << QString("%1:%2").arg(m_music->position()).arg(m_end_point);
    if (m_music->position() >= m_end_point){
        m_music->setPosition(m_beg_point);
    }
}

void MainWindow::on_media_updateDuration(qint64 duration)
{
    //qDebug()<<(duration);
    ui->horizon_music->setRange(0,duration);//根据播放时长来设置滑块的范围
    ui->horizon_music->setEnabled(duration>0);
    ui->horizon_music->setPageStep(duration/10);
    m_total_time = duration;
    m_end_point = m_total_time;
    ui->label_totle_time->setText("/"+GetFormatTime(m_total_time));
}

void MainWindow::set_media_position(qint64 duration)
{
    m_music->setPosition(duration);
}

void MainWindow::loadConnect()
{
    connect(m_music,&QMediaPlayer::positionChanged,this,&MainWindow::on_media_updatePosition);
    connect(m_music,&QMediaPlayer::durationChanged,this,&MainWindow::on_media_updateDuration);
    connect(ui->horizon_music,&QSlider::sliderMoved,this,&MainWindow::set_media_position);
}

void MainWindow::on_button_back_clicked()
{
    m_music->setPosition(m_music->position() - 3000 > 0 ? m_music->position() - 3000:0);
}

void MainWindow::on_button_forward_clicked()
{
    m_music->setPosition(m_music->position() + 3000 < m_total_time ? m_music->position() + 3000:m_total_time);
}

void MainWindow::on_button_set_start_clicked()
{
    m_beg_point = m_music->position();
    m_end_point = m_total_time;
    ui->label_beg_point->setText(GetFormatTime(m_music->position()));
    ui->label_end_point->clear();
}

void MainWindow::on_button_set_end_clicked()
{
    m_end_point = m_music->position();
    ui->label_end_point->setText(GetFormatTime(m_music->position()));
    m_music->setPosition(m_beg_point);

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
    m_mark_index = ui->listWidget->currentRow();
    QJsonObject select_json = m_mark_vec[m_mark_index];
    m_beg_point = select_json["beg_point"].toInt();
    m_end_point = select_json["end_point"].toInt();
    m_type_content = select_json["type"].toString();
    m_note_content = select_json["note"].toString();
    m_level_index= select_json["level_index"].toInt();
    m_sort_index= select_json["sort_index"].toInt();
    ui->label_beg_point->setText(GetFormatTime(m_beg_point));
    ui->label_end_point->setText(GetFormatTime(m_end_point));
    ui->combox_level->setCurrentIndex(m_level_index);
    ui->combox_sort->setCurrentIndex(m_sort_index);

    // Fill the Content
    SetPlainContent();


    // Set the player state
    // Just Play What is selected
    m_music->setPosition(m_beg_point);
    m_play = true;
    m_music->play();
    ui->button_play->setText("Pause");

    // Activate Button of Delete
    ui->button_delete->setEnabled(true);
}

void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    //current->setBackgroundColor(Qt::blue);

}

void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    //print("sha");
}

void MainWindow::on_button_delete_clicked()
{
    // No jsons be loaded
    if (m_mark_index == -1){
        return ;
    }

    // Delete from Vec instead of the ListWidget
    // ListWidget is just a Windows to Show. Dont Care!
    m_mark_vec.erase(m_mark_vec.begin() + m_mark_index);
    // Refresh the ListWidget
    ui->listWidget->clear();
    for (QVector<QJsonObject>::iterator it=m_mark_vec.begin(); it != m_mark_vec.end();it++){
        ui->listWidget->addItem(new QListWidgetItem(GetFormatMark(*it)));
    }
    ui->button_delete->setEnabled(false);

    // For safety
    m_mark_index = -1;
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    m_mark_index = ui->listWidget->currentRow();
    // Activate Button of Delete
    ui->button_delete->setEnabled(true);
}
