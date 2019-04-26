#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextBlock>
#include <QDebug>
#include <QString>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QJsonObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVector>
#include <QFile>
#include <QDir>
#include <QChar>
#include <QJsonDocument>
#include <QVariant>
#include <QDesktopServices>
#include <QMessageBox>
#include <QTextCodec>
#include <QCloseEvent>
#include "mydialog.h"

namespace Ui {
class MainWindow;
class Dialog;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_button_save_clicked();

    void on_actionopen_triggered();

    void on_button_play_clicked();

    void on_button_stop_clicked();

    void set_media_position(qint64 duration);

    void on_button_back_clicked();

    void on_button_forward_clicked();

    void on_button_set_start_clicked();

    void on_button_set_end_clicked();

    void on_button_clear_beg_point_clicked();

    void on_button_clear_end_point_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_button_delete_clicked();

    void on_combox_sort_level_activated(int index);

    void on_combox_sort_sort_activated(int index);

    void on_howToUse_triggered();

    void on_about_triggered();

    void on_combox_level_activated(int index);

    void on_combox_sort_activated(int index);

    void on_exit_triggered();

    void closeEvent(QCloseEvent *event);

    void on_onlineFile_triggered();
    
private:
    void on_media_updatePosition(qint64 duration);

    void on_media_updateDuration(qint64 duration);

    void on_media_stateChanged();

    void loadConnect();
    void loadComboxItems();
    void loadConstant();
    void loadControl(bool status);
    void loadJsonFile();
    void loadFileDir();
    void loadFileLog();
    void createOneMark();
    void saveMarkIntoFile();
    void loadJsonContent();
    void updateListWidget(bool add,const QJsonObject &json = QJsonObject());

    void music_play(qint64 point);
    void music_after_stop();
    void log_write(QString info);
    void fileOpenIsFailed(QString title,QString content);

    QString GetPlainContent(QPlainTextEdit * plainText);
    void SetPlainContent();
    QString GetFormatTime(qint64 time);
    QString GetFormatMark(const QJsonObject & json);
private:
    // 用于音乐播放
    QMediaPlayer m_music;
    bool m_play;
    qint64 m_total_time;
    qint64 m_beg_point;
    qint64 m_end_point;
    qint64 m_step_millSecond;

    // 用于界面内容
    QString m_type_content;
    QString m_note_content;
    QStringList m_level_value;
    int m_level_index;
    QStringList m_sort_value;
    int m_sort_index;
    QString m_filepath;
    QString m_filebase;

    // 用于标记
    QVector<QJsonObject> m_mark_vec;
    int m_mark_index;
    QFile m_file_object;
    QString m_file_name;
    QString m_file_saveDir;

    // 用于文件记录
    QFile m_log_object;
    QString m_log_name;


public://新打开的窗口
    Dialog * pDislog;

private:
    Ui::MainWindow *ui;
    Ui::Dialog *dialog;
};

#endif // MAINWINDOW_H
