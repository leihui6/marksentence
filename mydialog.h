#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QStringList>
#include <QSet>
#include <QDebug>
#include <QProcess>
#include <QFileDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
private slots:
    void on_combox_section_activated(int index);

    void on_btn_comfirm_clicked();

    void on_combox_TPO_activated(int index);

    void on_combox_detail_activated(int index);

    void on_btn_cancel_clicked();

private:
    void loadCombox();

    void generateUrl();

    void loadRemotePath();
private:
    QStringList m_section_list;
    QStringList m_detail_list;
    QString m_filename;
    QString m_remotepath;
    QString m_savePath;

    int m_tpo_index;
    int m_section_index;
    int m_detail_index;

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
