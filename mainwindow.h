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
#include <QJsonDocument>
#include <QVariant>
#include <QDesktopServices>

namespace Ui {
class MainWindow;
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

    void on_media_updatePosition(qint64 duration);

    void on_media_updateDuration(qint64 duration);

    void set_media_position(qint64 duration);

    void on_button_back_clicked();

    void on_button_forward_clicked();

    void on_button_set_start_clicked();

    void on_button_set_end_clicked();

    void on_button_clear_beg_point_clicked();

    void on_button_clear_end_point_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_listWidget_customContextMenuRequested(const QPoint &pos);

    void on_button_delete_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_combox_sort_level_activated(int index);

    void on_combox_sort_sort_activated(int index);

    void on_howToUse_triggered();

    void on_about_triggered();

private:
    void loadConnect();
    void loadComboxItems();
    void loadConstant();
    void loadListWidget();
    void loadControl(bool status);
    void loadJsonFile();
    void createOneMark();
    void saveMarkIntoFile();
    void loadJsonContent();
    void updateListWidget(bool add,const QJsonObject &json = QJsonObject());
    QString GetPlainContent(QPlainTextEdit * plainText);
    void SetPlainContent();
    QString GetFormatTime(qint64 time);
    QString GetFormatMark(const QJsonObject & json);
private:
    // For Playing Music
    QMediaPlayer * m_music;
    bool m_play;
    qint64 m_total_time;
    qint64 m_beg_point;
    qint64 m_end_point;

    QString m_type_content;
    QString m_note_content;
    QStringList m_level_value;
    int m_level_index;
    QStringList m_sort_value;
    int m_sort_index;
    QString m_filepath;
    QString m_filebase;

    // For Marks
    QVector<QJsonObject> m_mark_vec;
    int m_mark_index;
    QFile m_file_object;
    QString m_file_name;

private:
    // Use for testing
    void print(QString text);
    void print(qint64 text);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
