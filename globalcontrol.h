#ifndef GLOBALCONTROL_H
#define GLOBALCONTROL_H

#include <QStringList>
#include <QSet>
#include <QProcess>
#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDir>
#include <QFile>
#include <QVector>
#include <QCloseEvent>
#include <QMainWindow>
#include <QTextBlock>
#include <QDebug>
#include <QString>
#include <QPlainTextEdit>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QJsonObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QChar>
#include <QJsonDocument>
#include <QVariant>
#include <QDesktopServices>
#include <QMessageBox>
#include <QTextCodec>
#include <QTableWidget>
#include <QtAlgorithms>
#include <QDialog>
//#include <QStringLiteral>
#include <QTextCodec>

// Download Debug
//#define DLDEBUG

//#define MAINDEBUG

//#define SETDEBUG

extern QString G_remote_ip; // = "149.129.78.144";

extern int G_preSecond;// = 3000;

extern int G_stepSecond ;// = 3000;

extern bool G_closeAfterDownload; //= true;

extern bool G_playAfterDownload; //= true;

extern bool G_downloadWithoutNotifiction; // = true;

extern QString G_audioFilename;// created by "loaddialog"

extern QStringList G_reason_list; // phrase,linking,grammar,toolong,others

#endif // GLOBALCONTROL_H
