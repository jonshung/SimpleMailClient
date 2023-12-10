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

    _toEdit = new QLineEdit();
    _toEdit->setMinimumSize(parent->geometry().width(), 35);
    _toEdit->setTextMargins(10, 10, 10, 10);
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
    _ccEdit->setTextMargins(10, 10, 10, 10);

    _bccEdit = new QLineEdit();
    _bccEdit->setMinimumSize(parent->geometry().width(), 35);
    _bccEdit->setStyleSheet("QLineEdit { font-size: 13px }");
    _bccEdit->setTextMargins(10, 10, 10, 10);
    _bccEdit->contentsMargins().setBottom(30);

    _subjectEdit = new QLineEdit();
    _subjectEdit->setMinimumSize(parent->geometry().width(), 35);
    _subjectEdit->setStyleSheet("QLineEdit { font-size: 13px }");
    _subjectEdit->setTextMargins(10, 10, 10, 10);
    
    _toEdit->setPlaceholderText("To");
    _subjectEdit->setPlaceholderText("Subject");
    _ccEdit->setPlaceholderText("Cc");
    _bccEdit->setPlaceholderText("Bcc");

    _submitButton = new QPushButton();
    _submitButton->setText("&Send mail");
    _submitButton->setFixedSize(100, 40);
    SendMenu::connect(_submitButton, &QPushButton::clicked, this, &SendMenu::submit);

    _contentEdit = new ContentWidget(this);

    mainLayout->addWidget(_toEdit, 0, 0, Qt::AlignLeft);
    mainLayout->addWidget(ccOpen, 0, 1);
    mainLayout->addWidget(bccOpen, 0, 2);
    mainLayout->addWidget(_ccEdit, 1, 0, Qt::AlignLeft);
    mainLayout->addWidget(_bccEdit, 2, 0, Qt::AlignLeft);
    mainLayout->addWidget(_subjectEdit, 3, 0, Qt::AlignLeft);
    mainLayout->addWidget(_contentEdit, 4, 0);

    _ccEdit->setVisible(false);
    _bccEdit->setVisible(false);

    mainLayout->setContentsMargins(10, 10, 0, 0);
}

void SendMenu::submit() {
    if(MainWindow::_mailboxInstance == nullptr) return;
    QString contentHTML = _contentEdit->body()->toHtml();
    QString contentPlain = _contentEdit->body()->toPlainText();
    MailContent mailForm;
    mailForm.setFrom(MainWindow::_mailboxInstance->getCredential().getAddress());

    QString toField = _toEdit->text();
    QString subjectField = _subjectEdit->text();
    QString ccField = _ccEdit->text();
    QString bccField = _bccEdit->text();

    QStringList toList = toField.split(u',');
    QStringList ccList = ccField.split(u',');
    QStringList bccList = bccField.split(u',');

    for(QString rcpt : toList) {
        if(rcpt.length() == 0) continue;
        mailForm.addTo("", rcpt.toStdString());
    }
    for (QString rcpt : ccList) {
        if (rcpt.length() == 0) continue;
        mailForm.addCC("", rcpt.toStdString());
    }
    for (QString rcpt : bccList) {
        if (rcpt.length() == 0) continue;
        mailForm.addBCC("", rcpt.toStdString());
    }
    mailForm.setSubject(subjectField.toStdString());
    mailForm.setHTMLContent(contentHTML.toStdString());
    mailForm.setPlainContent(contentPlain.toStdString());
    qDebug().noquote() << MailContent::buildMessage(mailForm);
    //MainWindow::_mailboxInstance->
}

void SendMenu::showCC() {
    _ccEdit->setVisible(!_ccEdit->isVisible());
}

void SendMenu::showBCC() {
    _bccEdit->setVisible(!_bccEdit->isVisible());
}

#include "moc_SendMenu.cpp"