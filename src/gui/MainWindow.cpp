#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    _mainWindowInstance = this;
    ConfigProvider::init();
    _sendPage = nullptr;
    _mailboxPage = nullptr;
    updateConfig();
    qDebug() << "configured config successfully";

    setWindowTitle("Mail client");
    setMinimumSize(800, 550);
    resize(1024, 720);
    this->setStyleSheet("background-color: white;");

    QGridLayout* centralLayout = new QGridLayout();
    QLabel* Welcome_Msg = new QLabel("Mail client");
    Welcome_Msg->setStyleSheet("QLabel { font-size: 30px }");
    centralLayout->addWidget(Welcome_Msg, 0, 0, Qt::AlignCenter);

    _pages = new QStackedWidget(this);

    QWidget* welcomeMenu = new QWidget();
    welcomeMenu->setLayout(centralLayout);
    _sendPage = new SendMenu(this);
    _mailboxPage = new MailboxMenu(this);

    _pages->addWidget(welcomeMenu);
    _pages->addWidget(_sendPage);
    _pages->addWidget(_mailboxPage);

    setCentralWidget(_pages);
    createMenus();

    QString message = tr("Welcome to the simple mail client, start by selecting an item on the menu bar");
    statusBar()->showMessage(message);
}

void MainWindow::updateConfig() {
    _mailboxInstance.reset();
    const std::unique_ptr<ConfigProvider>& configData = ConfigProvider::_configProvider;
    _mailboxInstance = std::make_unique<ClientMailbox>(Credential(MailboxAddress(configData->user(), configData->address()), configData->password()));

    if (_sendPage != nullptr) _sendPage->updateConfig();
    if (_mailboxPage != nullptr) {
        _mailboxPage->clear();
        _mailboxPage->refetchTree();
    }
}

void MainWindow::createMenus() {
    _sendMenuAction = new QAction(tr("&Send mail"));
    _sendMenuAction->setStatusTip(tr("Go to send menu"));
    connect(_sendMenuAction, &QAction::triggered, this, &MainWindow::directToSend);

    _mailboxMenuAction = new QAction(tr("&Mailbox"));
    _mailboxMenuAction->setStatusTip(tr("Go to mailbox"));
    connect(_mailboxMenuAction, &QAction::triggered, this, &MainWindow::directToMailbox);

    _configAction = new QAction(tr("&Configure"));
    _configAction->setStatusTip(tr("Open configuration"));
    connect(_configAction, &QAction::triggered, this, &MainWindow::openConfig);

    _toolBar = new QToolBar(this);
    _toolBar->addAction(_sendMenuAction);
    _toolBar->addAction(_mailboxMenuAction);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _toolBar->addWidget(spacer);

    _toolBar->addAction(_configAction);
    _toolBar->setMovable(false);
    addToolBar(Qt::TopToolBarArea, _toolBar);
}

void MainWindow::directToMailbox() {
    if (_pages->currentIndex() == 2) {
        _pages->setCurrentIndex(0);
        return;
    }
    _pages->setCurrentIndex(2);
}

void MainWindow::directToSend() {
    if (_pages->currentIndex() == 1) {
        _pages->setCurrentIndex(0);
        return;
    }
    _pages->setCurrentIndex(1);
}

void MainWindow::openConfig() {
    if (_configPage) {
        _configPage->hide();
        delete _configPage;
        _configPage = nullptr;
        return;
    }
    _configPage = new ConfigWidget();
}

#include "moc_MainWindow.cpp"