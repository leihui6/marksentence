#ifndef LOADDIALOG_H
#define LOADDIALOG_H

/*
 * Author: ptsph@foxmail.com
 * Date: 16/07/2019
 * 负责下载TPO以及对应的歌词（文本）文件
 * 音频文件会保存至./TPO/TPO[1-54]/
 * 文本文件会保存至./Lyric/
 *
*/

#include "globalcontrol.h"

class QNetworkReply;

namespace Ui {
class loadDialog;
}

class loadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loadDialog(QFile & log_file,QWidget *parent = 0);
    ~loadDialog();
private slots:
    void on_combox_section_activated(int index);
    void on_btn_comfirm_clicked();
    void on_combox_TPO_activated(int index);
    void on_combox_detail_activated(int index);
    void on_btn_cancel_clicked();
    void on_loadBeg();
    void on_audioIng(qint64 bytesRead, qint64 bytesTotal);
    void on_audioFin();
    void on_lyricIng(qint64 bytesRead, qint64 bytesTotal);
    void on_lyricFin();
private:
    void loadConnect();
    void loadCombox();
    void loadConstant();
    void generateUrl();
    void loadRemotePath();
    bool createDir(QString dir);
    void logWrite(QString info);
    void finishedOperation();
private:
    QStringList m_section_list;
    QStringList m_detail_list;
    QString m_filename;
    QString m_lyric_filename;
    QString m_remotepath;
    QString m_savePath;

    int m_tpo_index;
    int m_section_index;
    int m_detail_index;

    QNetworkAccessManager * m_audio_load;
    QNetworkReply * m_audio_reply; // 临时创建
    QNetworkAccessManager * m_lyric_load;
    QNetworkReply * m_lyric_reply; // 临时创建
    qint64 m_audio_total;
    qint64 m_lyric_total;

    QFile m_audio_file;
    QFile m_lyric_file;
    QFile &m_log_file;
    QString m_audio_dir;
    QString m_lyric_dir;

    QVector<int>downloadQueue;
private:
    Ui::loadDialog * ui;
};

#endif // LOADDIALOG_H
