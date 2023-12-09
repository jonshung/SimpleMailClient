#include "SendMenu.h"

SendMenu::SendMenu(QWidget* parent) : QWidget(parent) {
    // construct
    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->setRowStretch(4, 1);
    setLayout(mainLayout);

    _toEdit = new QLineEdit();
    _toEdit->setMinimumSize(parent->geometry().width() * 0.85, 35);
    _toEdit->setTextMargins(10, 10, 10, 10);
    _toEdit->setStyleSheet("QLineEdit { font-size: 13px }");

    _subjectEdit = new QLineEdit();
    _subjectEdit->setMinimumSize(parent->geometry().width() * 0.85, 35);
    _subjectEdit->setStyleSheet("QLineEdit { font-size: 13px }");
    _subjectEdit->setTextMargins(10, 10, 10, 10);

    _ccEdit = new QLineEdit();
    _ccEdit->setMinimumSize(parent->geometry().width() * 0.85, 35);
    _ccEdit->setStyleSheet("QLineEdit { font-size: 13px }");
    _ccEdit->setTextMargins(10, 10, 10, 10);

    _bccEdit = new QLineEdit();
    _bccEdit->setMinimumSize(parent->geometry().width() * 0.85, 35);
    _bccEdit->setStyleSheet("QLineEdit { font-size: 13px }");
    _bccEdit->setTextMargins(10, 10, 10, 10);

    _contentEdit = new QTextEdit();
    _contentEdit->setMinimumSize(parent->geometry().width() * 0.85, parent->geometry().height() * 0.85);
    _contentEdit->setStyleSheet("QTextEdit { font-size: 12px }");
    _contentEdit->document()->setDocumentMargin(qreal(10));
    
    _toEdit->setPlaceholderText("To");
    _subjectEdit->setPlaceholderText("Subject");
    _ccEdit->setPlaceholderText("Cc");
    _bccEdit->setPlaceholderText("Bcc");

    _submitButton = new QPushButton();
    _submitButton->setText("&Send mail");
    _submitButton->setFixedSize(100, 40);
    SendMenu::connect(_submitButton, &QPushButton::clicked, this, &SendMenu::submit);

    mainLayout->addWidget(_toEdit, 0, 0, Qt::AlignLeft);
    mainLayout->addWidget(_subjectEdit, 1, 0, Qt::AlignLeft);
    mainLayout->addWidget(_ccEdit, 2, 0, Qt::AlignLeft);
    mainLayout->addWidget(_bccEdit, 3, 0, Qt::AlignLeft);
    mainLayout->addWidget(_contentEdit, 4, 0, Qt::AlignLeft);
    mainLayout->addWidget(_submitButton, 4, 2, Qt::AlignBottom | Qt::AlignHCenter);

    mainLayout->setContentsMargins(10, 10, 0, 0);
}

void SendMenu::submit() {
    if(MainWindow::_mailboxInstance == nullptr) return;
    //MainWindow::_mailboxInstance->
}

#include "moc_SendMenu.cpp"