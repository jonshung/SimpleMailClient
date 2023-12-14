#ifndef _MAIN_SOCKET_WINDOW_H
#define _MAIN_SOCKET_WINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QStackedWidget>

#include <memory>

#include "ClientMailbox.h"
#include "SendMenu.h"
#include "MailboxMenu.h"
#include "ConfigWidget.h"
#include "ConfigProvider.h"

class SendMenu;
class MailboxMenu;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    inline static std::unique_ptr<ClientMailbox> _mailboxInstance = nullptr;
    inline static MainWindow* _mainWindowInstance = nullptr;
    inline static QWidget* _configPage = nullptr;
    MainWindow(QWidget* = nullptr);
    virtual ~MainWindow() {
        if(_configPage) {
            delete _configPage;
        }
    }
private slots:
    void directToSend();
    void directToMailbox();
    void openConfig();

public:
    void updateConfig();
    SendMenu* sendPage();
    MailboxMenu* mailboxPage();

private:
    QStackedWidget* _pages;
    SendMenu* _sendPage;
    MailboxMenu* _mailboxPage;

    void createMenus();
    QAction* _configAction;
    QToolBar* _toolBar;
    QAction* _sendMenuAction;
    QAction* _mailboxMenuAction;
    QLabel* _infoLabel;
};

#endif