#ifndef LOADDIALOG_H
#define LOADDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QSet>
#include <QDebug>
#include <QProcess>
#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDir>
#include <QFile>
#include <QVector>

class QNetworkReply;

namespace Ui {
class loadDialog;
}

class loadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loadDialog(QWidget *parent = 0);
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
    void closeEvent(QCloseEvent * event);

    void loadConnect();

    void loadCombox();

    void loadConstant();

    void generateUrl();

    void loadRemotePath();

    void createDir(QString dir);
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

    QFile * m_audio_file;
    QFile * m_lyric_file;
    QString m_saveDir;
private:
    Ui::loadDialog * ui;
};

#endif // LOADDIALOG_H
