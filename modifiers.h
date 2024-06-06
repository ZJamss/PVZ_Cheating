#ifndef MODIFIERS_H
#define MODIFIERS_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <thread>
#include <Windows.h>
#include "constants.h"
#include "utils.h"

class MainWindow;

class Modifier
{
public:
    explicit Modifier(MainWindow* mainWindow) : mainWindow(mainWindow) {}
    virtual ~Modifier() {}

protected:
    MainWindow* mainWindow;
};

// 阳光修改器
class SunlightModifier : public Modifier
{
public:
    using Modifier::Modifier;
    void modify(int sunlightValue, bool autoMod);
    void autoModify(int sunlightValue);
private:
    bool tSunlightRunning = false;
};

// 银币修改器
class CoinModifier : public Modifier
{
public:
    using Modifier::Modifier;
    void modify(int coinValue);
};

// 冷却修改器
class CooldownModifier : public Modifier
{
public:
    using Modifier::Modifier;
    void enable();
    void disable();
    void modify();
private:
    bool tCDRunning = false;
};

// 暂停框修改器
class PauseModifier : public Modifier
{
public:
    using Modifier::Modifier;
    ~PauseModifier();
    void togglePause(bool enable);
private:
    BYTE* pauseInstructionArray = nullptr;
    BYTE* nopArray = nullptr;
};

// 游戏上下文管理
class GameHandleManager : public Modifier
{
public:
    using Modifier::Modifier;
    void initGameHandle();
    void reloadButtonsState(bool state);
private:
    bool handleInited = false;
};

#endif // MODIFIERS_H
