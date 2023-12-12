#ifndef _SEND_WINDOW_H_
#define _SEND_WINDOW_H_

#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>

#include <QtCore/QDebug>

#include "MainWindow.h"
#include "ContentWidget.h"

class ContentWidget;

class SendMenu : public QWidget {
    Q_OBJECT
private:
    QLineEdit* _toEdit;
    QLineEdit* _subjectEdit;
    QLineEdit* _ccEdit;
    QLineEdit* _bccEdit;
    ContentWidget* _contentEdit;
private slots:
    void showCC();
    void showBCC();
public slots:
    void submit();
public:
    SendMenu(QWidget* = nullptr);

    ContentWidget* content();
    virtual ~SendMenu() {}
};

#endif