#ifndef _CONFIG_WIDGET_H
#define _CONFIG_WIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QMessageBox>

#include <QIntValidator>
#include <QCloseEvent>

#include "ConfigProvider.h"
#include "MainWindow.h"

class ConfigWidget : public QWidget {
    Q_OBJECT
private:
    bool _changed{ false };

private:
    QLineEdit* _userEdit;
    QLineEdit* _addressEdit;
    QLineEdit* _passwordEdit;
    QLineEdit* _smtpServerEdit;
    QLineEdit* _smtpPortEdit;
    QLineEdit* _pop3ServerEdit;
    QLineEdit* _pop3PortEdit;
    QTextEdit* _customFilterEdit;
    QCheckBox* _autofetchCheck;
    QLineEdit* _autofetchTimeEdit;
    QLineEdit* _maxFileSizeEdit;
private slots:
    void onTextChange();
    void save();
    void onCheckBox(int);
public:
    ConfigWidget(QWidget* = 0);
    void closeEvent(QCloseEvent*);
};

#endif