#ifndef _MAILBOX_MENU_H_
#define _MAILBOX_MENU_H_

#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTreeView>
#include <QtGui/QStandardItemModel>
#include <QtGui/QFileSystemModel>
#include <QWebEngineView>
#include <QtWebEngineCore/QWebEngineGlobalSettings>

#include <QTemporaryDir>
#include <QImage>
#include <QtWidgets/QPushButton>
#include <QSizePolicy>
#include <QDir>
#include <filesystem>

#include "MainWindow.h"

class MailboxMenu : public QWidget {
    Q_OBJECT
private:
    QTreeView* _tree;
    QWebEngineView* _display;
    QTemporaryDir* _currentTemp = 0;
private slots:
    void showMail(const QModelIndex&);
    void fetch();
public:
    MailboxMenu(QWidget* = nullptr);
    void refetchTree();
    inline virtual ~MailboxMenu() {
        if(_currentTemp == nullptr) return;
        delete _currentTemp;
    }
};
#endif