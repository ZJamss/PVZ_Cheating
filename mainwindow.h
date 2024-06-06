#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "utils.h"
#include <QThread>
#include <chrono>
#include <QMainWindow>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    HANDLE gameHandle;
    bool handleInited = false;
    bool tSunlightRunning = false;
    bool tCDRunning = false;
    BYTE* pauseInstructionArray = nullptr;
    BYTE* nopArray = nullptr;
    void onModSunlightClicked();
    void onModCoinClicked();
    void onCdClicked();
    void onNoCdClicked();
    void initGameHandle();
    void reloadBtnsState(bool reloadState);
    bool modMemoData(DWORD Adress,DWORD AdressFirst,DWORD AdressSecond,int value);
    void autoModSunlight(int sunlightValue);
    void modCD();
    void modPause();
    void showMessage(const QString& message);

signals: // 定义信号
    void showMessageSignal(const QString& message);
};
#endif // MAINWINDOW_H
