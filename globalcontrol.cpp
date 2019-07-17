#include "globalcontrol.h"

QString G_remote_ip = "149.129.78.144";

int G_preSecond = 3000;

int G_stepSecond = 3000;

bool G_closeAfterDownload = true;

bool G_playAfterDownload = true;

bool G_downloadWithoutNotifiction = true;

QString G_audioFilename = QString();

QStringList G_reason_list = {
    QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("短语"),
    QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("连读"),
    QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("语法"),
    QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("太长"),
   QTextCodec::codecForName(QByteArray("GBK"))->toUnicode("其它")
};

