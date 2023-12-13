#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Mail client");
    setMinimumSize(800, 550);
    resize(1024, 720);
    this->setStyleSheet("background-color: white;");
    _mailboxInstance = std::make_unique<ClientMailbox>(Credential(MailboxAddress("David Buoi", "user1@abc.com"), "user1@abc.com"));

    QGridLayout* centralLayout = new QGridLayout();
    QLabel *Welcome_Msg = new QLabel("Mail client");
    Welcome_Msg->setStyleSheet("QLabel { font-size: 30px }");
    centralLayout->addWidget(Welcome_Msg, 0, 0, Qt::AlignCenter);

    //_infoLabel = new QLabel(tr("<i>Choose an option</i>"));
    //_infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    //_infoLabel->setAlignment(Qt::AlignCenter);
    //centralLayout->addWidget(Welcome_Msg, 1, 0, Qt::AlignBottom);

    _pages = new QStackedWidget(this);

    QWidget* welcomeMenu = new QWidget();
    welcomeMenu->setLayout(centralLayout);
    SendMenu* sendMenu = new SendMenu(this);
    MailboxMenu* mailboxMenu = new MailboxMenu(this);

    _pages->addWidget(welcomeMenu);
    _pages->addWidget(sendMenu);
    _pages->addWidget(mailboxMenu);

    setCentralWidget(_pages);
    createMenus();

    QString message = tr("Welcome to the simple mail client, start by selecting an item on the menu bar");
    statusBar()->showMessage(message);
}

void MainWindow::createMenus() {
    _sendMenuAction = new QAction(tr("&Send mail"));
    _sendMenuAction->setStatusTip(tr("Go to send menu"));
    connect(_sendMenuAction, &QAction::triggered, this, &MainWindow::directToSend);

    _mailboxMenuAction = new QAction(tr("&Mailbox"));
    _mailboxMenuAction->setStatusTip(tr("Go to mailbox"));
    connect(_mailboxMenuAction, &QAction::triggered, this, &MainWindow::directToMailbox);

    _toolBar = new QToolBar(this);
    _toolBar->addAction(_sendMenuAction);
    _toolBar->addAction(_mailboxMenuAction);
    _toolBar->setMovable(false);
    addToolBar(Qt::TopToolBarArea, _toolBar);
}

void MainWindow::directToMailbox() {
    if(_pages->currentIndex() == 2) {
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


#include "moc_MainWindow.cpp"