#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "loaddialog.h"
#include "settingdialog.h"
#include "globalcontrol.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void on_button_play_clicked();
    void set_media_position(qint64 duration);
    void on_button_back_clicked();
    void on_button_forward_clicked();
    void on_howToUse_triggered();
    void on_about_triggered();
    void on_exit_triggered();
    void closeEvent(QCloseEvent *event);
    //void on_onlineFile_triggered();
    void on_tableWidget_cellDoubleClicked(int row, int column);
    //void on_localFile_triggered();
    void on_clickHeader(int col);
    void on_showTableMenu(const QPoint pos);
    void on_showTextMenu(const QPoint &point);
    void on_setting_triggered();

    void on_deleteItem();
    void on_textMenuSelectedPhrase();
    void on_textMenuSelectedLinking();
    void on_textMenuSelectedGrammar();
    void on_textMenuSelectedToolong();
    void on_textMenuSelectedOthers();

    void on_localAudio_triggered();

    void on_localText_triggered();

public:
    void on_media_updatePosition(qint64 duration);
    void on_media_updateDuration(qint64 duration);
    void on_media_stateChanged();

    void openLocalAudio(QString filepath = QString());
    void openLocalText(QString filepath = QString());

    void saveTextMenuSelected(int index);
    void loadConnect();
    void loadConstant();
    void loadControl(bool status);
    void loadJsonFile();
    void loadFileDir();
    bool createDir(QString dirname);
    void createLogFile();
    void loadTableWidget();
    void createOneMark(qint64 pos,int index,const QString & str);
    void saveMarkIntoFile();
    void loadJsonContent();
    void updateTableWidget(QVector<int> index_vec = QVector<int>());
    void sortby(const QString sort_key,QVector<int> & result);
    void musicPlay(qint64 point);
    void musicAfterStop();
    void logWrite(QString info);
    void fileOpenIsFailed(QString title,QString content);
    void setPlainContent();
    void clearText();
    bool generateLyricFileName();
    void loadLyric2Map();
    void showLyric();
    void goToSpecificLine(int line);
    QString stylized(QString & str);
    QString highLightLine(QString & str);
    QString getSelectedWords();
    QString getPlainContent(QPlainTextEdit * plainText);
    QString getFormatTime(qint64 time);
    //QString getFormatMark(const QJsonObject & json);
    QString getFormatContent(const QJsonObject & json, int index);
private: // 参数
    // 用于音乐播放
    QMediaPlayer m_music;
    bool m_play;
    qint64 m_total_time;
    qint64 m_beg_point;
    qint64 m_end_point;

    // 用于歌词文件
    QFile  m_lyric_object;
    QString m_lyric_name;
    QString m_lyric_dir;
    QMap<int,QString> m_lyric_map;
    QVector<QString> m_lyricVec;

    // 用于界面内容
    QString m_filepath;
    QString m_filebase;
    int m_click_row;
    bool m_exist_lyric;
    int m_font_size;

    // 用于标记
    QVector<QJsonObject> m_json_vec;
    QFile m_json_object;
    QString m_json_name;
    QString m_json_dir;

    // 用于文件记录
    QFile m_log_object;
    QString m_log_name;
    QString m_log_dir;

    // TPO音频文件相关
    QString m_tpo_dir;
    QString m_remote_ip;

private: //新打开的窗口
    loadDialog * m_loadDialog;
    SettingDialog * m_settingDialog;

private:
    Ui::MainWindow * ui;
};

#endif // MAINWINDOW_H
