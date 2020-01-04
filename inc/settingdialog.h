#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

/*
 * Author: ptsph@foxmail.com
 * Date: 16/07/2019
 * 对程序的部分变量进行控制，它们是
 *  - 远程IP(remote_ip)
 *  - 记录提前多少秒(presecond)
 *  - 快进步长(stepsecond)
 *  - 是否下载完后关闭(closeAfterDownload)
 *  - 是否下载完后播放(playAfterDownload)
 *
*/

#include "globalcontrol.h"

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);

    ~SettingDialog();
    
private slots:
    void on_btn_save_clicked();

    void on_btn_cane_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    void initValueOnWidget();

private:
    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
