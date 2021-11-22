#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTimer>

#include "lib/FtpClient.h"
#include "lib/Log.h"
#include "lib/Directory.h"

#include "processbar_widget.hpp"

class CFtpClient;
class CLog;
class CDirectory;

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
    void on_pb_connect_clicked(bool checked);
    void on_pb_create_clicked();
    void on_pb_get_clicked();
    void on_pb_select_clicked();
    void on_pb_download_clicked();
    void on_pb_upload_clicked();
    void on_le_path_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui_;
    ProcessbarWidget bar_widget_;

    CFtpClient ftp_;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_HPP
