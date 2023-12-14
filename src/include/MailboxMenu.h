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
#include <QtWidgets/QLineEdit>
#include <QtCore/QTimer>

#include "AttachmentListWidget.h"
#include "MainWindow.h"

class AttachmentListWidget;

class MailboxMenu : public QWidget {
    Q_OBJECT
private:
    QGridLayout* _Mlayout;
    QTreeView* _tree;
    QWidget* _mailContent;
    AttachmentListWidget* _attachmentList;
    QWebEngineView* _display;
    QTemporaryDir* _currentTemp = 0;
    QTimer* _autofetchTimer;

private:
    QLineEdit* _fromReadOnly;
    QLineEdit* _toReadOnly;
    QLineEdit* _subjectReadOnly;
    QLineEdit* _ccReadOnly;
    QLineEdit* _bccReadOnly;

    void initHeader();
public slots:
    void fetch();
private slots:
    void showMail(const QModelIndex&);
public:
    MailboxMenu(QWidget* = nullptr);
    void refetchTree();
    void checkConfigUpdate();
    void clear();
    inline virtual ~MailboxMenu() {
        if(_currentTemp == nullptr) return;
        delete _currentTemp;
    }
};
#endif