#include "processbar_widget.hpp"
#include "ui_processbar_widget.h"

#include <iostream>

ProcessbarWidget::ProcessbarWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessbarWidget)
{
    ui->setupUi(this);    

    process_timer_ = new QTimer(this);

    connect(process_timer_, SIGNAL(timeout()), this, SLOT(timerCallback()));
}

ProcessbarWidget::~ProcessbarWidget()
{
    delete process_timer_;
    delete ui;
}

void ProcessbarWidget::setTitle(QString name)
{
    this->setWindowTitle(name);
}

void ProcessbarWidget::timerCallback()
{
    int percent = (double)process_time_ / (double)process_step_ * 100.0;

    ui->progressBar->setValue(percent);

    if(percent >= 100)
    {
        process_timer_->stop();
        this->hide();
    }
}


void ProcessbarWidget::setProcessStep(int step)
{
    process_step_ = step;
}

void ProcessbarWidget::addProcessTime()
{
    process_time_++;
}

void ProcessbarWidget::start()
{
    process_time_ = 0;
    process_timer_->start(100);
    show();
}

void ProcessbarWidget::stop()
{
    hide();
    process_timer_->stop();
}
