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

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    inline static std::unique_ptr<ClientMailbox> _mailboxInstance = nullptr;
    MainWindow(QWidget* = nullptr);
    virtual ~MainWindow() {}
private slots:
    void directToSend();
    void directToMailbox();
private:
    QStackedWidget* _pages;
    void createMenus();
    QToolBar* _toolBar;
    QAction* _sendMenuAction;
    QAction* _mailboxMenuAction;
    QLabel* _infoLabel;
};

#endif