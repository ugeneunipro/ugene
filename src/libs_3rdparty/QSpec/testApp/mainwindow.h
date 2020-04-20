#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>

class QLineEdit;
class EventRecorderWidget;
class EventFilter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow* getInstance();
    EventFilter* eventFilter;
    ~MainWindow();
protected:
    MainWindow(QWidget *parent = 0);
private:
    static MainWindow* instance;
    QLineEdit* testLine;
    EventRecorderWidget* eventRecorderWidget;
};

#endif // MAINWINDOW_H
