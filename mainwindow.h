#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modifiers.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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

    // 修改器
    SunlightModifier* sunlightModifier;
    CoinModifier* coinModifier;
    CooldownModifier* cooldownModifier;
    PauseModifier* pauseModifier;
    PlantModifier* plantModifier;
    GameHandleManager* gameHandleManager;

    // 控件事件
    void onModSunlightClicked();
    void onModCoinClicked();
    void onCdClicked();
    void onNoCdClicked();
    void modPause();
    void modifyPlantDecrLife();
    void showMessage(const QString& message);

    // 初始化事件
    void initGameHandle();
    void reloadButtonsState(bool state);

signals:
    void showMessageSignal(const QString& message);

    friend class Modifier;
    friend class SunlightModifier;
    friend class CoinModifier;
    friend class CooldownModifier;
    friend class PauseModifier;
    friend class PlantModifier;
    friend class GameHandleManager;
};

#endif // MAINWINDOW_H
