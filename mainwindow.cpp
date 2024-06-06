#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("PVZ修改器");
    this->resize(300, 200);

    // 初始化各个Modifier
    sunlightModifier = new SunlightModifier(this);
    coinModifier = new CoinModifier(this);
    cooldownModifier = new CooldownModifier(this);
    pauseModifier = new PauseModifier(this);
    gameHandleManager = new GameHandleManager(this);

    // 绑定按钮点击事件
    connect(ui->mod_sunlight_btn, &QPushButton::clicked, this, &MainWindow::onModSunlightClicked);
    connect(ui->mod_coin_btn, &QPushButton::clicked, this, &MainWindow::onModCoinClicked);
    connect(ui->cd_btn, &QPushButton::clicked, this, &MainWindow::onCdClicked);
    connect(ui->no_cd_btn, &QPushButton::clicked, this, &MainWindow::onNoCdClicked);
    connect(ui->handle_find_btn, &QPushButton::clicked, this, &MainWindow::initGameHandle);
    connect(ui->mod_pause_checkbox, &QCheckBox::clicked, this, &MainWindow::modPause);

    // 绑定信号
    connect(this, &MainWindow::showMessageSignal, this, &MainWindow::showMessage, Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete sunlightModifier;
    delete coinModifier;
    delete cooldownModifier;
    delete pauseModifier;
    delete gameHandleManager;
    delete ui;
}

// 修改阳光
void MainWindow::onModSunlightClicked()
{
    int sunlightValue = ui->mod_sunlight_input->value();
    bool autoMod = ui->mod_sunlight_auto_checkbox->isChecked();
    sunlightModifier->modify(sunlightValue, autoMod);
}

// 修改金币
void MainWindow::onModCoinClicked()
{
    int coinValue = ui->mod_coin_input->value();
    coinModifier->modify(coinValue);
}

// 关闭无冷却
void MainWindow::onCdClicked()
{
    cooldownModifier->disable();
}

// 开启无冷却
void MainWindow::onNoCdClicked()
{
    cooldownModifier->enable();
}

// 取消暂停窗口
void MainWindow::modPause()
{
    bool enable = ui->mod_pause_checkbox->isChecked();
    pauseModifier->togglePause(enable);
}

void MainWindow::showMessage(const QString& message)
{
    QMessageBox::information(this, "Info", message);
}

// 初始化游戏句柄
void MainWindow::initGameHandle()
{
    gameHandleManager->initGameHandle();
}

// 初始化控件状态
void MainWindow::reloadButtonsState(bool state)
{
    gameHandleManager->reloadButtonsState(state);
}
