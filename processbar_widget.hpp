#ifndef PROCESSBAR_WIDGET_HPP
#define PROCESSBAR_WIDGET_HPP

#include <QDialog>
#include <QTimer>

namespace Ui {
class ProcessbarWidget;
}

class ProcessbarWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessbarWidget(QWidget *parent = nullptr);
    ~ProcessbarWidget();
    void setTitle(QString name);
    void setProcessStep(int step);
    void addProcessTime();
    void start();
    void stop();

private Q_SLOTS:
    void timerCallback();

public:

private:
    Ui::ProcessbarWidget *ui;
    QTimer *process_timer_;

    int process_time_;
    int process_step_;
};

#endif // PROCESSBAR_WIDGET_HPP
