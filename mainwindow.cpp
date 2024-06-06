#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("PVZ修改器");
    this->resize(300,200);

    // 绑定按钮点击事件
    connect(ui->mod_sunlight_btn, &QPushButton::clicked, this, &MainWindow::onModSunlightClicked);
    connect(ui->mod_coin_btn, &QPushButton::clicked, this, &MainWindow::onModCoinClicked);
    connect(ui->cd_btn,&QPushButton::clicked,this,&MainWindow::onCdClicked);
    connect(ui->no_cd_btn,&QPushButton::clicked,this,&MainWindow::onNoCdClicked);
    connect(ui->handle_find_btn,&QPushButton::clicked,this,&MainWindow::initGameHandle);
    connect(ui->mod_pause_checkbox,&QCheckBox::clicked,this,&MainWindow::modPause);

    //绑定信号
    connect(this, &MainWindow::showMessageSignal, this, &MainWindow::showMessage, Qt::QueuedConnection);

}
MainWindow::~MainWindow()
{
    delete ui;
}

//修改阳光
void MainWindow::onModSunlightClicked()
{
    int sunlightValue = ui->mod_sunlight_input->value();
    bool autoMod = ui->mod_sunlight_auto_checkbox->isChecked();
    //自动修改
    if(autoMod){
        if (!this->tSunlightRunning) {
            this->tSunlightRunning = true;
            std::thread(&MainWindow::autoModSunlight, this, sunlightValue).detach();
        }
    }else{
        this->tSunlightRunning = false;
        if (modMemoData(0x006A9F38, 0x768, 0x5560,sunlightValue)){
            QMessageBox::information(this,"Info","修改阳光数量成功");
            return;
        }
        QMessageBox::warning(this,"Warning","修改阳光数量失败");
    }

}

void MainWindow::autoModSunlight(int sunlightValue) {
    emit showMessageSignal("开启定时修改阳光");
    while (this->tSunlightRunning) {
        if (!modMemoData(0x006A9F38, 0x768, 0x5560, sunlightValue)) {
            break;
        }
        Sleep(3000); // 每隔三秒执行一次
    }
}

//修改金币
//TODO寻址
void MainWindow::onModCoinClicked()
{
    int coinValue = ui->mod_coin_input->value();
    if (modMemoData(0x006A9EC0, 0x82C, 0x28,coinValue)){
        QMessageBox::information(this,"Info","修改金币数量成功");
        return;
    }
    QMessageBox::warning(this,"Warning","修改金币数量失败");
}

//关闭无冷却
void MainWindow::onCdClicked()
{
    this->tCDRunning = false;
}

//开启无冷却
void MainWindow::onNoCdClicked()
{
    if (!this->tCDRunning) {
        this->tCDRunning = true;
        std::thread(&MainWindow::modCD, this).detach();
    }
}

void MainWindow::modCD(){
    emit showMessageSignal("开启无冷却");
    while (this->tCDRunning)
    {
        DWORD Final_Address = Readmeo(this->gameHandle, 0x006A9EC0, 0x768, 0x144);
        ReadProcessMemory(this->gameHandle, (LPVOID)(Final_Address), &Final_Address, 4, 0);
        int value = 1;
        //14个格子无冷却
        for (int i = 0; i < 14; i++)
        {
            WriteProcessMemory(this->gameHandle, (LPVOID)(Final_Address + 0x70 + (0x50) * i), &value, sizeof(DWORD), 0);
        }
        Sleep(500);
    }
}
\
// 取消自动暂停窗口
void MainWindow::modPause(){

    LPVOID address = reinterpret_cast<LPVOID>(0x004502C6);
    SIZE_T size = 0x004504A0 - 0x004502C6 + 1;
    BYTE* destInstuctionArray;

    //保存暂停窗口指令
    if(this->pauseInstructionArray == nullptr){
        this->pauseInstructionArray = new BYTE[size];
        SIZE_T bytesRead;
        if (!ReadProcessMemory(this->gameHandle, address, this->pauseInstructionArray, size, &bytesRead)) {
            QMessageBox::information(this, "Info", "读取内存指令失败");
            return;
        }
    }

    //创建nop指令数组
    if(this->nopArray == nullptr){
        this->nopArray = new BYTE[size];
        memset(this->nopArray, NOP, size);
    }

    if(ui->mod_pause_checkbox->isChecked()){
        destInstuctionArray = this->nopArray;
    }else{
        destInstuctionArray = this->pauseInstructionArray;
    }
    DWORD oldProtect;
    if (!VirtualProtectEx(this->gameHandle, address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        QMessageBox::information(this, "Info", "取消内存保护出错");
        return;
    }
    SIZE_T bytesWritten;
    if (!WriteProcessMemory(this->gameHandle, address, destInstuctionArray, size, &bytesWritten)) {
        QMessageBox::information(this, "Info", "写入内存出错");
        return;
    }

    if (!VirtualProtectEx(this->gameHandle, address, size, oldProtect, &oldProtect)) {
        QMessageBox::information(this, "Info", "恢复内存保护出错");
        return;
    }
}

void MainWindow::showMessage(const QString& message) {
    QMessageBox::information(this, "Info", message);
}

bool MainWindow::modMemoData(DWORD Adress,DWORD AdressFirst,DWORD AdressSecond,int value)
{
    DWORD Final_Adress = Readmeo(this->gameHandle, Adress, AdressFirst, AdressSecond);
    return WriteProcessMemory(this->gameHandle, (LPVOID)(Final_Adress), &value, sizeof(DWORD), 0);
}

//寻找游戏句柄
void MainWindow::initGameHandle()
{
    QString windowName = ui->window_name_input->text();
    HANDLE hgame = getGameHandle(nullptr,
                                 reinterpret_cast<LPCWSTR>(windowName.toStdWString().c_str()));
    bool reloadState = false;
    if(hgame == nullptr){
        QMessageBox::warning(this,"Warning","未找到PVZ游戏进程,请检查窗口名称输入是否正确或者游戏是否打开！");
    }else{
        QString handleStr = QString("%1").arg(reinterpret_cast<quintptr>(hgame), sizeof(void*) * 2, 16, QChar('0'));
        QMessageBox::information(this, "Information", QString("句柄ID：%1").arg(handleStr));
        this->gameHandle = hgame;
        reloadState = true;
    }
    reloadBtnsState(reloadState);
}

//重载控件状态
void MainWindow::reloadBtnsState(bool reloadState)
{
    if(this->handleInited == reloadState){
        return;
    }
    this->handleInited = reloadState;
    ui->no_cd_btn->setEnabled(this->handleInited);
    ui->cd_btn->setEnabled(this->handleInited);
    ui->mod_sunlight_input->setEnabled(this->handleInited);
    ui->mod_sunlight_btn->setEnabled(this->handleInited);
    ui->mod_sunlight_auto_checkbox->setEnabled(this->handleInited);
    ui->mod_pause_checkbox->setEnabled(this->handleInited);
    // ui->mod_coin_input->setEnabled(this->handleInited);
    // ui->mod_coin_btn->setEnabled(this->handleInited);
}
