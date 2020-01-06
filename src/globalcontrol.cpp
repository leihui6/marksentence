#include "inc/globalcontrol.h"

QString G_remote_ip = "149.129.78.144";

int G_preSecond = 3000;

int G_stepSecond = 3000;

bool G_closeAfterDownload = true;

bool G_playAfterDownload = true;

bool G_downloadWithoutNotifiction = true;

QString G_audioFilename = QString();

QString G_textFilename = QString();

QStringList G_reason_list = {
    QStringLiteral("短语"),
    QStringLiteral("连读"),
    QStringLiteral("语法"),
    QStringLiteral("太长"),
    QStringLiteral("其它")
};

