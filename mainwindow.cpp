#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <iostream>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>

#include <thread>
#include <time.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    setWindowTitle("FTP");
    setWindowIcon(QIcon(":/src/img/ftp.png"));

    move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    ui_->le_path->setPlaceholderText(QDir::homePath());

    for(int i=0; i<ui_->buttonGroup->buttons().size(); i++)
        ui_->buttonGroup->buttons().at(i)->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::on_pb_connect_clicked(bool checked)
{
    if(checked)
    {
        InitNetwork();
        CLog::SetLevel( LOG_DEBUG | LOG_NETWORK );

        std::string ip = ui_->le_ip->text().toStdString();
        if(ui_->le_ip->text().toStdString().empty())
        {
            ip = ui_->le_ip->placeholderText().toStdString();
            ui_->le_ip->setText(QString::fromStdString(ip));
        }

        if( ftp_.Connect( ip.c_str(), 21, true ) == false )
        {
            QMessageBox::warning(this, tr("ERROR"), QString::fromStdString("Cannot connect to " +  ip));
            ui_->pb_connect->setChecked(false);
            return;
        }

        std::string user = ui_->le_user->text().toStdString();
        if(ui_->le_user->text().toStdString().empty())
        {
            user = ui_->le_user->placeholderText().toStdString();
            ui_->le_user->setText(QString::fromStdString(user));
        }
        std::string pw = ui_->le_pw->text().toStdString();

        if( ftp_.Login( user.c_str(), pw.c_str() ) == false )
        {
            QMessageBox::warning(this, tr("ERROR"), QString::fromStdString("Login failed"));
            ui_->pb_connect->setChecked(false);
            ftp_.Close();
            return;
        }

        ui_->pb_connect->setText("Disconnect");

        for(int i=0; i<ui_->buttonGroup->buttons().size(); i++)
            ui_->buttonGroup->buttons().at(i)->setEnabled(true);
    }
    else
    {
        ftp_.Close();
        ui_->pb_connect->setText("Connect");

        for(int i=0; i<ui_->buttonGroup->buttons().size(); i++)
            ui_->buttonGroup->buttons().at(i)->setEnabled(false);

        ui_->le_path->clear();
        ui_->le_remote_path->clear();
    }
}

void MainWindow::on_pb_create_clicked()
{
    std::string remote_path = ui_->le_remote_path->text().toStdString();

    if( ftp_.CreateFolder( remote_path.c_str() ) == false )
    {
        QMessageBox::warning(this, tr("ERROR"), QString::fromStdString("Cannot create folder " + remote_path));
        return;
    }

    if( ftp_.ChangeFolder( remote_path.c_str() ) == false )
    {
        QMessageBox::warning(this, tr("ERROR"), QString::fromStdString("Cannot change folder to " + remote_path));
        return;
    }

    std::string cur_path;

    if( ftp_.GetCurrentFolder( cur_path ) == false )
    {
        QMessageBox::warning(this, tr("ERROR"), QString::fromStdString("Cannot get current folder"));
        return;
    }

    ui_->le_remote_path->setText(QString::fromStdString(cur_path));
}

void MainWindow::on_pb_get_clicked()
{
    std::string cur_path;

    if( ftp_.GetCurrentFolder( cur_path ) == false )
    {
        QMessageBox::warning(this, tr("ERROR"), QString::fromStdString("Cannot get current folder"));
        return;
    }

    ui_->le_remote_path->setText(QString::fromStdString(cur_path));
}

void MainWindow::on_pb_select_clicked()
{
    QString get_path = ui_->le_path->text();

    if(get_path.toStdString().empty())
        get_path = ui_->le_path->placeholderText();

    //File
    //QString save_path = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this, tr("Select File"), \
    get_path, \
            tr("All Files (*)"), \
            nullptr, QFileDialog::DontUseNativeDialog));

    //Folder
    QString save_path = QFileDialog::getExistingDirectory(this, tr("Select Directory"), \
                                                          get_path, \
                                                          QFileDialog::ShowDirsOnly
                                                          | QFileDialog::DontResolveSymlinks
                                                          | QFileDialog::DontUseNativeDialog);

    if(save_path.toStdString().empty())
        return;

    ui_->le_path->setText(save_path);
}

void MainWindow::on_pb_download_clicked()
{
    std::string remote_folder = ui_->le_remote_path->text().toStdString();
    if( ftp_.ChangeFolder( remote_folder.c_str() ) == false )
    {
        QMessageBox::warning(this, tr("ERROR"), QString::fromStdString("Cannot change folder to " + remote_folder));
        return;
    }

    std::string local_folder = ui_->le_path->text().toStdString();

    FTP_FILE_LIST container_list;
    if( ftp_.List( container_list ) == false )
    {
        QMessageBox::warning(this, tr("ERROR"), QString::fromStdString("Get list failed"));
        return;
    }

    ui_->pb_upload->setEnabled(false);
    ui_->pb_download->setEnabled(false);
    bar_widget_.setTitle(QString::fromStdString("Download files.."));
    bar_widget_.setProcessStep(container_list.size());
    bar_widget_.start();

    new std::thread([this](FTP_FILE_LIST container_list, std::string local_folder)
    {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        FTP_FILE_LIST::iterator it;
        for( it = container_list.begin(); it != container_list.end(); ++it )
        {
            bar_widget_.addProcessTime();

            if( it->m_bFolder )
            {
                //printf( "folder[%s]\n", itFL->m_strFileName.c_str() );
            }
            else
            {
                std::string filename = it->m_strFileName;
                std::string local_file = local_folder + "/" + filename;

                if( ftp_.Download( it->m_strFileName.c_str(), local_file.c_str() ) == false )
                {
                    QMessageBox::warning(this, tr("ERROR"), QString::fromStdString("Cannot download " + filename));
                    bar_widget_.stop();
                    break;
                }
            }
        }

        ui_->pb_upload->setEnabled(true);
        ui_->pb_download->setEnabled(true);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_result = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec)*1e-9;
        QMessageBox::information(this, tr("INFO"), QString::fromStdString("Download Complete! (" + std::to_string(time_result) + "s elapsed)"));

    }, container_list, local_folder);
}

void MainWindow::on_pb_upload_clicked()
{
    std::string remote_folder = ui_->le_remote_path->text().toStdString();
    if( ftp_.ChangeFolder( remote_folder.c_str() ) == false )
    {
        QMessageBox::warning(this, tr("ERROR"), QString::fromStdString("Cannot change folder to " + remote_folder));
        return;
    }

    FILE_LIST file_list;
    std::string local_folder = ui_->le_path->text().toStdString();
    CDirectory::FileList( local_folder.c_str(), file_list );

    ui_->pb_upload->setEnabled(false);
    ui_->pb_download->setEnabled(false);
    bar_widget_.setTitle(QString::fromStdString("Upload files.."));
    bar_widget_.setProcessStep(file_list.size());
    bar_widget_.start();

    new std::thread([this](FILE_LIST file_list, std::string local_folder)
    {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        FILE_LIST::iterator it;
        for( it = file_list.begin(); it != file_list.end(); ++it )
        {
            bar_widget_.addProcessTime();
            std::string upload_path = local_folder;
            CDirectory::AppendName( upload_path, it->c_str() );

            if( ftp_.Upload( upload_path.c_str() ) == false )
            {
                QMessageBox::warning(this, tr("ERROR"), QString::fromStdString("Cannot upload " + upload_path));
                bar_widget_.stop();
                break;
            }
        }

        ui_->pb_upload->setEnabled(true);
        ui_->pb_download->setEnabled(true);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_result = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec)*1e-9;
        QMessageBox::information(this, tr("INFO"), QString::fromStdString("Upload Complete! (" + std::to_string(time_result) + "s elapsed)"));

    }, file_list, local_folder);
}

void MainWindow::on_le_path_textChanged(const QString &arg1)
{
    if(!arg1.toStdString().empty())
    {
        ui_->pb_upload->setEnabled(true);
        ui_->pb_download->setEnabled(true);
    }
    else
    {
        ui_->pb_upload->setEnabled(false);
        ui_->pb_download->setEnabled(false);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    int ret = QMessageBox::warning(this, tr("프로그램 종료"), tr("프로그램을 종료 하시겠습니까?"), \
                                   QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

    switch(ret)
    {
    case QMessageBox::Ok:
        event->accept();
        break;

    default:
        break;
    }
}
