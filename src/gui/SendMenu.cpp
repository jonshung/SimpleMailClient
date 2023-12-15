#include "SendMenu.h"

SendMenu::SendMenu(QWidget* parent) : QWidget(parent) {
    setGeometry(parent->geometry());
    // construct
    QGridLayout* mainLayout = new QGridLayout(this);
    this->setMaximumSize(parent->geometry().size());
    setLayout(mainLayout);

    setStyleSheet("QLineEdit { border: none; border-bottom: 1px solid #dedede; outline-offset: 5px; }");

    auto palette = this->palette();
    palette.setColor(QPalette::Window, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(palette);
    this->setPalette(palette);

    _fromReadOnly = new QLineEdit();
    Credential cred = MainWindow::_mailboxInstance->getCredential();
    QString fromField = QString::fromStdString(cred.getAddress().getReceiptName() + 
                        ((cred.getAddress().getReceiptName().length() > 0) ? " " : "") + cred.getAddress().getReceiptAddress());
    _fromReadOnly->setText(fromField);
    _fromReadOnly->setReadOnly(true);
    _fromReadOnly->setMinimumSize(parent->geometry().width(), 35);
    _fromReadOnly->setTextMargins(10, 10, 10, 0);
    _fromReadOnly->setStyleSheet("QLineEdit { font-size: 13px }");

    QPalette* paletteFrom = new QPalette();
    paletteFrom->setColor(QPalette::Base, Qt::gray);
    paletteFrom->setColor(QPalette::Text, Qt::darkGray);
    _fromReadOnly->setPalette(*paletteFrom);

    _toEdit = new QLineEdit();
    _toEdit->setMinimumSize(parent->geometry().width(), 35);
    _toEdit->setTextMargins(10, 10, 10, 0);
    _toEdit->setStyleSheet("QLineEdit { font-size: 13px }");
    
    QPushButton* ccOpen = new QPushButton();
    ccOpen->setStyleSheet("QPushButton { border: none; font-size: 13px; background: white; margin-right: 5px; } "
                          "QPushButton:hover { border-bottom: 1px solid black; outline-offset: 1px }");
    ccOpen->setText("Cc");
    SendMenu::connect(ccOpen, &QPushButton::clicked, this, &SendMenu::showCC);

    QPushButton* bccOpen = new QPushButton();
    bccOpen->setStyleSheet("QPushButton { border: none; font-size: 13px; background: white; margin-right: 30px; } "
                           "QPushButton:hover { border-bottom: 1px solid black; outline-offset: 1px }");
    bccOpen->setText("Bcc");
    SendMenu::connect(bccOpen, &QPushButton::clicked, this, &SendMenu::showBCC);

    _ccEdit = new QLineEdit();
    _ccEdit->setMinimumSize(parent->geometry().width(), 35);
    _ccEdit->setStyleSheet("QLineEdit { font-size: 13px }");
    _ccEdit->setTextMargins(10, 10, 10, 0);

    _bccEdit = new QLineEdit();
    _bccEdit->setMinimumSize(parent->geometry().width(), 35);
    _bccEdit->setStyleSheet("QLineEdit { font-size: 13px }");
    _bccEdit->setTextMargins(10, 10, 10, 0);
    _bccEdit->contentsMargins().setBottom(30);

    _subjectEdit = new QLineEdit();
    _subjectEdit->setMinimumSize(parent->geometry().width(), 35);
    _subjectEdit->setStyleSheet("QLineEdit { font-size: 13px }");
    _subjectEdit->setTextMargins(10, 10, 10, 0);
    
    _toEdit->setPlaceholderText("To");
    _subjectEdit->setPlaceholderText("Subject");
    _ccEdit->setPlaceholderText("Cc");
    _bccEdit->setPlaceholderText("Bcc");

    _contentEdit = new ContentWidget(this);
    _contentEdit->setMinimumWidth(parent->geometry().width());

    mainLayout->addWidget(_fromReadOnly, 0, 0, Qt::AlignLeft);
    mainLayout->addWidget(_toEdit, 1, 0, Qt::AlignLeft);
    mainLayout->addWidget(ccOpen, 1, 1);
    mainLayout->addWidget(bccOpen, 1, 2);
    mainLayout->addWidget(_ccEdit, 2, 0, Qt::AlignLeft);
    mainLayout->addWidget(_bccEdit, 3, 0, Qt::AlignLeft);
    mainLayout->addWidget(_subjectEdit, 4, 0, Qt::AlignLeft);
    mainLayout->addWidget(_contentEdit, 5, 0);

    _ccEdit->setVisible(false);
    _bccEdit->setVisible(false);

    mainLayout->setContentsMargins(10, 10, 0, 0);
}

void SendMenu::updateConfig() {
    if(_fromReadOnly == nullptr) return;
    Credential cred = MainWindow::_mailboxInstance->getCredential();
    QString fromField = QString::fromStdString(cred.getAddress().getReceiptName() +
        ((cred.getAddress().getReceiptName().length() > 0) ? " " : "") + cred.getAddress().getReceiptAddress());
    _fromReadOnly->setText(fromField);
}

void SendMenu::submit() {
    if(MainWindow::_mailboxInstance == nullptr) return;
    QString contentHTML = _contentEdit->body()->toHtml();
    QString contentPlain = _contentEdit->body()->toPlainText();
    MailContent mailForm;
    mailForm.setFrom(MainWindow::_mailboxInstance->getCredential().getAddress());

    QString toField = _toEdit->text();
    if (_fromReadOnly->text().length() <= 0) {
        QMessageBox confirm(this);
        confirm.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        confirm.setText("Not yet logged in");
        confirm.setStandardButtons(QMessageBox::Ok);
        confirm.exec();
        return;
    }
    if(toField.length() <= 0) {
        QMessageBox confirm(this);
        confirm.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        confirm.setText("Must atleast specify a valid recipient");
        confirm.setStandardButtons(QMessageBox::Ok);
        confirm.exec();
        return;
    }
    QString subjectField = _subjectEdit->text();
    QString ccField = _ccEdit->text();
    QString bccField = _bccEdit->text();

    QStringList toList = toField.split(u',');
    QStringList ccList = ccField.split(u',');
    QStringList bccList = bccField.split(u',');

    for(QString rcpt : toList) {
        if(rcpt.length() == 0) continue;
        QStringList rcptSplit = rcpt.split(u' ');
        std::string rcptName = "";
        std::string rcptAddr = rcpt.toStdString();
        if(rcptSplit.size() >= 2) {
            rcptName = rcptSplit[0].toStdString();
            rcptAddr = rcptSplit[1].toStdString();
        }
        mailForm.addTo(rcptName, rcptAddr);
    }
    for (QString rcpt : ccList) {
        if (rcpt.length() == 0) continue;
        QStringList rcptSplit = rcpt.split(u' ');
        std::string rcptName = "";
        std::string rcptAddr = rcpt.toStdString();
        if (rcptSplit.size() >= 2) {
            rcptName = rcptSplit[0].toStdString();
            rcptAddr = rcptSplit[1].toStdString();
        }
        mailForm.addCC("", rcpt.toStdString());
    }
    for (QString rcpt : bccList) {
        if (rcpt.length() == 0) continue;
        QStringList rcptSplit = rcpt.split(u' ');
        std::string rcptName = "";
        std::string rcptAddr = rcpt.toStdString();
        if (rcptSplit.size() >= 2) {
            rcptName = rcptSplit[0].toStdString();
            rcptAddr = rcptSplit[1].toStdString();
        }
        mailForm.addBCC("", rcpt.toStdString());
    }
    mailForm.setSubject(subjectField.toStdString());
    mailForm.setHTMLContent(contentHTML.toStdString());
    mailForm.setPlainContent(contentPlain.toStdString());

    for(const QFileInfo& fileinfo : _contentEdit->attachmentListWidget()->attachments()) {
        const QString& filepath = fileinfo.filePath();
        MIMEAttachment _mime = mimeFromFile(filepath.toStdString(), "", 20 * 1024 * 1024);
        if (_mime.getData().length() == 0) {
            continue;
        }
        mailForm.addAttachment(_mime);
    }

    const std::unique_ptr<ConfigProvider>& configData = ConfigProvider::_configProvider;

    MainWindow::_mailboxInstance->sendContent(configData->SMTPServer(), configData->SMTPPort(), mailForm);
    if (MainWindow::_mailboxInstance->_error.length() > 0 && MainWindow::_mailboxInstance->_error[0] == '-') {
        qDebug() << MainWindow::_mailboxInstance->_error;
        MainWindow::_mailboxInstance->_error = "";
    } else {
        QMessageBox confirm(this);
        confirm.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        confirm.setText("Sent!");
        confirm.setStandardButtons(QMessageBox::Ok);
        confirm.exec();

        _toEdit->setText("");
        _ccEdit->setText("");
        _bccEdit->setText("");
        _subjectEdit->setText("");
        _contentEdit->body()->clear();
        _contentEdit->attachmentListWidget()->delAll();

        if (MainWindow::_mainWindowInstance->mailboxPage()) {
            MainWindow::_mainWindowInstance->mailboxPage()->fetch();
        }
    }
}

void SendMenu::showCC() {
    _ccEdit->setVisible(!_ccEdit->isVisible());
}

void SendMenu::showBCC() {
    _bccEdit->setVisible(!_bccEdit->isVisible());
}


ContentWidget* SendMenu::content() {
    return _contentEdit;
}

#include "moc_SendMenu.cpp"