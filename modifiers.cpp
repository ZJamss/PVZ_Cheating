#include "modifiers.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

void SunlightModifier::modify(int sunlightValue, bool autoMod)
{
    if (autoMod) {
        if (!this->tSunlightRunning) {
            // 开启线程定时修改阳光
            this->tSunlightRunning = true;
            std::thread(&SunlightModifier::autoModify, this, sunlightValue).detach();
        }
    } else {
        this->tSunlightRunning = false;
        if (writeMemo(mainWindow->gameHandle,SUNLIGHT_BASE, SUNLIGHT_OFFSET1, SUNLIGHT_OFFSET2, sunlightValue)) {
            QMessageBox::information(mainWindow, "Info", "修改阳光数量成功");
        } else {
            QMessageBox::warning(mainWindow, "Warning", "修改阳光数量失败");
        }
    }
}

void SunlightModifier::autoModify(int sunlightValue)
{
    emit mainWindow->showMessageSignal("开启定时修改阳光");
    while (this->tSunlightRunning) {
        if (!writeMemo(mainWindow->gameHandle,SUNLIGHT_BASE, SUNLIGHT_OFFSET1, SUNLIGHT_OFFSET2, sunlightValue)) {
            break;
        }
        Sleep(3000); // 每隔三秒执行一次
    }
}

void CoinModifier::modify(int coinValue)
{
    bool res = true;
    res = res && writeMemo(mainWindow->gameHandle,COIN_BASE, COIN_OFFSET1, COIN_OFFSET2, coinValue);
    res = res && writeMemo(mainWindow->gameHandle,GCOIN_BASE, GCOIN_OFFSET1, GCOIN_OFFSET2, coinValue);
    res = res && writeMemo(mainWindow->gameHandle,DIAMOND_BASE, DIAMOND_OFFSET1, DIAMOND_OFFSET2, coinValue);

    if (res) {
        QMessageBox::information(mainWindow, "Info", "修改货币数量成功");
    } else {
        QMessageBox::warning(mainWindow, "Warning", "修改货币数量失败");
    }
}

void CooldownModifier::enable()
{
    if (!this->tCDRunning) {
        // 开启线程不间断修改冷却
        this->tCDRunning = true;
        std::thread(&CooldownModifier::modify, this).detach();
    }
}

void CooldownModifier::disable()
{
    this->tCDRunning = false;
}

void CooldownModifier::modify()
{
    emit mainWindow->showMessageSignal("开启无冷却");
    while (this->tCDRunning) {
        DWORD Final_Address = Readmeo(mainWindow->gameHandle, CD_BASE, CD_OFFSET1, CD_OFFSET2);
        ReadProcessMemory(mainWindow->gameHandle, (LPVOID)(Final_Address), &Final_Address, 4, 0);
        int value = 1;
        for (int i = 0; i < 14; i++) {
            WriteProcessMemory(mainWindow->gameHandle, (LPVOID)(Final_Address + 0x70 + (0x50) * i), &value, sizeof(DWORD), 0);
        }
        Sleep(500);
    }
}

void PauseModifier::togglePause(bool enable)
{
    LPVOID address = reinterpret_cast<LPVOID>(PAUSE_ADDRESS);
    SIZE_T size = PAUSE_SIZE;
    BYTE* destInstuctionArray;

    // 初始化指令数组
    if (this->pauseInstructionArray == nullptr) {
        this->pauseInstructionArray = new BYTE[size];
        SIZE_T bytesRead;
        if (!ReadProcessMemory(mainWindow->gameHandle, address, this->pauseInstructionArray, size, &bytesRead)) {
            QMessageBox::information(mainWindow, "Info", "读取内存指令失败");
            return;
        }
    }

    if (this->nopArray == nullptr) {
        this->nopArray = new BYTE[size];
        memset(this->nopArray, NOP, size);
    }

    // 切换目标指令数组
    if (enable) {
        destInstuctionArray = this->nopArray;
    } else {
        destInstuctionArray = this->pauseInstructionArray;
    }

    DWORD oldProtect;
    if (!VirtualProtectEx(mainWindow->gameHandle, address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        QMessageBox::warning(mainWindow, "Warning", "取消内存保护出错");
        return;
    }

    SIZE_T bytesWritten;
    if (!WriteProcessMemory(mainWindow->gameHandle, address, destInstuctionArray, size, &bytesWritten)) {
        QMessageBox::warning(mainWindow, "Warning", "写入内存出错");
        return;
    }

    if (!VirtualProtectEx(mainWindow->gameHandle, address, size, oldProtect, &oldProtect)) {
        QMessageBox::warning(mainWindow, "Warning", "恢复内存保护出错");
        return;
    }
}

PauseModifier::~PauseModifier(){
    if(this->pauseInstructionArray != nullptr){
        delete[] this->pauseInstructionArray;
    }
    if(this->nopArray != nullptr){
        delete[] this->nopArray;
    }
}

void GameHandleManager::initGameHandle()
{
    QString windowName = mainWindow->ui->window_name_input->text();
    HANDLE hgame = getGameHandle(nullptr, reinterpret_cast<LPCWSTR>(windowName.toStdWString().c_str()));
    bool reloadState = false;
    if (hgame == nullptr) {
        QMessageBox::warning(mainWindow, "Warning", "未找到PVZ游戏进程,请检查窗口名称输入是否正确或者游戏是否打开！");
    } else {
        QString handleStr = QString("%1").arg(reinterpret_cast<quintptr>(hgame), sizeof(void*) * 2, 16, QChar('0'));
        QMessageBox::information(mainWindow, "Information", QString("句柄ID：%1").arg(handleStr));
        mainWindow->gameHandle = hgame;
        reloadState = true;
    }
    reloadButtonsState(reloadState);
}

void GameHandleManager::reloadButtonsState(bool reloadState)
{
    if (this->handleInited == reloadState) {
        return;
    }
    this->handleInited = reloadState;
    mainWindow->ui->no_cd_btn->setEnabled(this->handleInited);
    mainWindow->ui->cd_btn->setEnabled(this->handleInited);
    mainWindow->ui->mod_sunlight_input->setEnabled(this->handleInited);
    mainWindow->ui->mod_sunlight_btn->setEnabled(this->handleInited);
    mainWindow->ui->mod_sunlight_auto_checkbox->setEnabled(this->handleInited);
    mainWindow->ui->mod_pause_checkbox->setEnabled(this->handleInited);
    mainWindow->ui->mod_coin_input->setEnabled(this->handleInited);
    mainWindow->ui->mod_coin_btn->setEnabled(this->handleInited);
}
