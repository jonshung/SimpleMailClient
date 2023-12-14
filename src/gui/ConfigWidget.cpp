#include "ConfigWidget.h"

ConfigWidget::ConfigWidget(QWidget* parent) : QWidget(parent) {
    resize(850, 400);
    setWindowTitle("Configuration");
    this->setStyleSheet("background-color: white;");
    const std::unique_ptr<ConfigProvider>& configData = ConfigProvider::_configProvider;

    _userEdit = new QLineEdit(QString::fromStdString(configData->user()));
    connect(_userEdit, &QLineEdit::textChanged, this, &ConfigWidget::onTextChange);
    _addressEdit = new QLineEdit(QString::fromStdString(configData->address()));
    connect(_addressEdit, &QLineEdit::textChanged, this, &ConfigWidget::onTextChange);
    _passwordEdit = new QLineEdit(QString::fromStdString(configData->password()));
    connect(_passwordEdit, &QLineEdit::textChanged, this, &ConfigWidget::onTextChange);
    _passwordEdit->setEchoMode(QLineEdit::EchoMode::Password);
    _smtpServerEdit = new QLineEdit(QString::fromStdString(configData->SMTPServer()));
    connect(_smtpServerEdit, &QLineEdit::textChanged, this, &ConfigWidget::onTextChange);
    _smtpPortEdit = new QLineEdit(QString::fromStdString(configData->SMTPPort()));
    connect(_smtpPortEdit, &QLineEdit::textChanged, this, &ConfigWidget::onTextChange);
    _pop3ServerEdit = new QLineEdit(QString::fromStdString(configData->POP3Server()));
    connect(_pop3ServerEdit, &QLineEdit::textChanged, this, &ConfigWidget::onTextChange);
    _pop3PortEdit = new QLineEdit(QString::fromStdString(configData->POP3Port()));
    connect(_pop3PortEdit, &QLineEdit::textChanged, this, &ConfigWidget::onTextChange);
    _customFilterEdit = new QTextEdit(QString::fromStdString(configData->customFilter()));
    _customFilterEdit->setPlaceholderText("Available variable:\n+subject: string // the subject of the mail\n"
                                          "+from: string // the names (if available) and addresses of the sender(s)\n"
                                          "+bodyText: string // the text content of the mail");
    connect(_customFilterEdit, &QTextEdit::textChanged, this, &ConfigWidget::onTextChange);
    _autofetchCheck = new QCheckBox();
    _autofetchCheck->setCheckState((configData->autofetch()) ? Qt::Checked : Qt::Unchecked);
    connect(_autofetchCheck, &QCheckBox::stateChanged, this, &ConfigWidget::onCheckBox);
    _autofetchTimeEdit = new QLineEdit(QString::number(configData->autofetchTime()));
    connect(_autofetchTimeEdit, &QLineEdit::textChanged, this, &ConfigWidget::onTextChange);
    _autofetchTimeEdit->setValidator(new QIntValidator(this));
    _maxFileSizeEdit = new QLineEdit(QString::number(configData->maxFileSize()));
    _maxFileSizeEdit->setValidator(new QIntValidator(this));
    connect(_maxFileSizeEdit, &QLineEdit::textChanged, this, &ConfigWidget::onTextChange);

    QFormLayout* formLayout = new QFormLayout(this);
    formLayout->addRow(tr("&User name:"), _userEdit);
    formLayout->addRow(tr("&Email:"), _addressEdit);
    formLayout->addRow(tr("&Password:"), _passwordEdit);
    formLayout->addRow(tr("&SMTP server:"), _smtpServerEdit);
    formLayout->addRow(tr("&SMTP port:"), _smtpPortEdit);
    formLayout->addRow(tr("&POP3 server:"), _pop3ServerEdit);
    formLayout->addRow(tr("&POP3 port:"), _pop3PortEdit);
    formLayout->addRow(tr("&Custom filter:"), _customFilterEdit);
    formLayout->addRow(tr("&Autofetch:"), _autofetchCheck);
    formLayout->addRow(tr("&Autofetch interval (seconds):"), _autofetchTimeEdit);
    formLayout->addRow(tr("&Max file size (bytes):"), _maxFileSizeEdit);
    QPushButton* applyButton = new QPushButton("Apply");
    connect(applyButton, &QPushButton::pressed, this, &ConfigWidget::save);
    QPushButton* cancelButton = new QPushButton("Cancel");
    connect(cancelButton, &QPushButton::pressed, this, &ConfigWidget::close);
    formLayout->addRow(cancelButton, applyButton);
    
    setLayout(formLayout);
    show();
}

void ConfigWidget::save() {
    const std::unique_ptr<ConfigProvider>& configData = ConfigProvider::_configProvider;
    configData->setUser(_userEdit->text().toStdString());
    configData->setAddress(_addressEdit->text().toStdString());
    configData->setPassword(_passwordEdit->text().toStdString());
    configData->setSMTPServer(_smtpServerEdit->text().toStdString());
    configData->setSMTPPort(_smtpPortEdit->text().toStdString());
    configData->setPOP3Server(_pop3ServerEdit->text().toStdString());
    configData->setPOP3Port(_pop3PortEdit->text().toStdString());
    configData->setCustomFilter(_customFilterEdit->toPlainText().toStdString());
    configData->setAutofetch((_autofetchCheck->isChecked()) ? true : false);
    configData->setAutofetchTime(_autofetchTimeEdit->text().toInt());
    configData->setMaxFileSize(_maxFileSizeEdit->text().toInt());
    configData->save();
    _changed = false;
    if (MainWindow::_mainWindowInstance) MainWindow::_mainWindowInstance->updateConfig();

    QMessageBox confirm(this);
    confirm.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    confirm.setText("Saved!");
    confirm.setStandardButtons(QMessageBox::Ok);
    confirm.exec();
}

void ConfigWidget::closeEvent(QCloseEvent* ev) {
    if(_changed) {
        QMessageBox confirm(this);
        confirm.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        confirm.setText("You have unsaved changes");
        confirm.setInformativeText("closing will lose all previous changes, do you want to continue?");
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::SaveAll | QMessageBox::Cancel);
        confirm.setDefaultButton(QMessageBox::Cancel);

        int ret = confirm.exec();
        if (ret == QMessageBox::Yes) {
        }
        else if (ret == QMessageBox::SaveAll) {
            save();
        } else if(ret == QMessageBox::Cancel) {
            ev->ignore();
        }
    }
    if (MainWindow::_configPage) {
        MainWindow::_configPage = nullptr;
    }
}

void ConfigWidget::onTextChange() {
    _changed = true;
}

void ConfigWidget::onCheckBox(int state) {
    _changed = true;
}

#include "moc_ConfigWidget.cpp"