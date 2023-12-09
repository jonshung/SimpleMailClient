#ifndef _SEND_WINDOW_H_
#define _SEND_WINDOW_H_

#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>

#include <QtCore/QDebug>

#include "MainWindow.h"

class SendMenu : public QWidget {
    Q_OBJECT
private:
    QLineEdit* _toEdit;
    QLineEdit* _subjectEdit;
    QLineEdit* _ccEdit;
    QLineEdit* _bccEdit;
    QTextEdit* _contentEdit;
    QPushButton* _submitButton;
private slots:
    void submit();
public:
    SendMenu(QWidget* = nullptr);
    virtual ~SendMenu() {}
};

#endif