#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString lastLocation;

private slots:
    void addFiles();
    void removeFiles();
    void clearFiles();
    void fileSelected();
    void convertFiles();
    void selectOutputFile();
};

#endif // MAINWINDOW_H
