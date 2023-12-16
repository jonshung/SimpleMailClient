#include "MailboxMenu.h"

MailboxMenu::MailboxMenu(QWidget* parent) : QWidget(parent) {
    setGeometry(parent->geometry());
    _Mlayout = new QGridLayout();
    _Mlayout->setContentsMargins(20, 20, 20, 20);
    setLayout(_Mlayout);
    _tree = new QTreeView();
    _tree->setFixedWidth(200);
    _tree->setMinimumHeight(this->geometry().height() * 0.75);
    _tree->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    _tree->setEditTriggers(QAbstractItemView::NoEditTriggers);

    _currentTemp = 0;

    QPushButton* b1 = new QPushButton();
    b1->setText("Fetch mail");
    b1->setMinimumSize(QSize(150, 40));
    b1->setStyleSheet("QPushButton { border: none; border: 1px solid black; border-radius: 7px; background-color: white; font-weight: bold; font-size: 14px; } "
                      "QPushButton:hover { background-color: black; color: white; } "
                      "QPushButton:hover:pressed { font-size: 13px; }");
    connect(b1, &QPushButton::clicked, this, &MailboxMenu::fetch);

    refetchTree();
    connect(_tree, &QTreeView::clicked, this, &MailboxMenu::showMail);

    initHeader();

    _Mlayout->addWidget(_tree, 0, 0, Qt::AlignLeft);
    _Mlayout->addWidget(b1, 1, 0, Qt::AlignCenter);
    _attachmentList = new AttachmentListWidget(this, false, 200, 0);

    const std::unique_ptr<ConfigProvider>& configData = ConfigProvider::_configProvider;
    if(configData->autofetch()) {
        _autofetchTimer = new QTimer(this);
        connect(_autofetchTimer, &QTimer::timeout, this, &MailboxMenu::fetch);
        _autofetchTimer->start(configData->autofetchTime() * 1000);
    }
}

void MailboxMenu::checkConfigUpdate() {
    const std::unique_ptr<ConfigProvider>& configData = ConfigProvider::_configProvider;
    if (_autofetchTimer) {
        if (!configData->autofetch()) _autofetchTimer->stop();
        else {
            _autofetchTimer->start(configData->autofetchTime() * 1000);
        }
    }
}

void MailboxMenu::fetch() {
    if (MainWindow::_mailboxInstance == nullptr) return;
    qDebug() << "fetching new data...";
    const std::unique_ptr<ConfigProvider>& configData = ConfigProvider::_configProvider;
    if(MainWindow::_mailboxInstance->getCredential().getAddress().getReceiptAddress().length() > 0) {
        MainWindow::_mailboxInstance->fetch(configData->POP3Server(), configData->POP3Port(), configData->customFilter());
    }
    refetchTree();
}

void MailboxMenu::refetchTree() {
    if (_tree->model() != nullptr) {
        delete _tree->model();
    }
    if (MainWindow::_mailboxInstance == nullptr) return;

    QStandardItemModel* model = new QStandardItemModel();
    QStandardItem* parentItem = model->invisibleRootItem();

    std::filesystem::path filterDirPath(QDir::currentPath().toStdString());
    filterDirPath.append("mailboxes/").append(MainWindow::_mailboxInstance->getCredential().getAddress().getReceiptAddress());

    ordered_json filterData = MainWindow::_mailboxInstance->getFolders();

    for (auto& filter : filterData.items()) {
        QStandardItem* filterItem = new QStandardItem(QString::fromStdString(filter.key()));
        parentItem->appendRow(filterItem);
        QDir filterDir(filterDirPath / filter.key());
        if (!filterDir.exists()) {
            std::filesystem::create_directories(filterDir.filesystemPath());
        }
        int unread = 0;
        for (const QFileInfo fInfo : filterDir.entryInfoList()) {
            if (!fInfo.isFile() || fInfo.suffix() != "msg") continue;
            QFile fileHandler(fInfo.filePath());
            if (!fileHandler.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

            QTextStream in(&fileHandler);
            std::string content = in.readAll().toStdString();
            MIMESegment e = parseMIMEEntity(content);
            QString subj = QString::fromStdString(extractSubject(&e));
            QStandardItem* mailEntry = new QStandardItem(subj);

            if(MainWindow::_mailboxInstance) {
                if(!MainWindow::_mailboxInstance->mailIsRead(fInfo.fileName().toStdString())) {
                    QFont tFont = mailEntry->font();
                    tFont.setBold(true);
                    mailEntry->setFont(tFont);
                    unread++;
                }
            }
            mailEntry->setData(fInfo.filePath());
            filterItem->appendRow(mailEntry);
            fileHandler.close();
        }
        if(unread > 0) {
            QFont tFont = filterItem->font();
            tFont.setBold(true);
            filterItem->setFont(tFont);
        }
    }
    model->setHeaderData(0, Qt::Orientation::Horizontal, "Mails");
    _tree->setModel(model);
}

void MailboxMenu::initHeader() {
    _mailContent = new QWidget(this);
    //_mailContent->setMaximumHeight(10);
    _mailContent->setStyleSheet("QLineEdit { border: none; border-bottom: 1px solid #dedede; outline-offset: 5px; }");
    QGridLayout* headerLayout = new QGridLayout();
    _mailContent->setLayout(headerLayout);

    _fromReadOnly = new QLineEdit();
    Credential cred = MainWindow::_mailboxInstance->getCredential();
    QString fromField = QString::fromStdString(cred.getAddress().getReceiptName() +
        ((cred.getAddress().getReceiptName().length() > 0) ? " " : "") + cred.getAddress().getReceiptAddress());
    _fromReadOnly->setText(fromField);
    _fromReadOnly->setReadOnly(true);
    _fromReadOnly->setMinimumSize(geometry().width() * 0.73, 35);
    _fromReadOnly->setTextMargins(10, 10, 10, 0);
    _fromReadOnly->setStyleSheet("QLineEdit { font-size: 13px }");

    QPalette* paletteReadOnly = new QPalette();
    paletteReadOnly->setColor(QPalette::Base, Qt::gray);
    _fromReadOnly->setPalette(*paletteReadOnly);

    _toReadOnly = new QLineEdit();
    _toReadOnly->setReadOnly(true);
    _toReadOnly->setMinimumSize(geometry().width() * 0.73, 35);
    _toReadOnly->setTextMargins(10, 10, 10, 0);
    _toReadOnly->setStyleSheet("QLineEdit { font-size: 13px }");
    _toReadOnly->setPalette(*paletteReadOnly);

    _ccReadOnly = new QLineEdit();
    _ccReadOnly->setReadOnly(true);
    _ccReadOnly->setMinimumSize(geometry().width() * 0.73, 35);
    _ccReadOnly->setStyleSheet("QLineEdit { font-size: 13px }");
    _ccReadOnly->setTextMargins(10, 10, 10, 0);
    _ccReadOnly->setPalette(*paletteReadOnly);

    _bccReadOnly = new QLineEdit();
    _bccReadOnly->setReadOnly(true);
    _bccReadOnly->setMinimumSize(geometry().width() * 0.73, 35);
    _bccReadOnly->setStyleSheet("QLineEdit { font-size: 13px }");
    _bccReadOnly->setTextMargins(10, 10, 10, 0);
    _bccReadOnly->contentsMargins().setBottom(30);
    _bccReadOnly->setPalette(*paletteReadOnly);

    _subjectReadOnly = new QLineEdit();
    _subjectReadOnly->setMinimumSize(geometry().width() * 0.73, 35);
    _subjectReadOnly->setStyleSheet("QLineEdit { font-size: 13px }");
    _subjectReadOnly->setTextMargins(10, 10, 10, 0);
    _subjectReadOnly->setPalette(*paletteReadOnly);

    _toReadOnly->setPlaceholderText("To");
    _subjectReadOnly->setPlaceholderText("Subject");
    _ccReadOnly->setPlaceholderText("Cc");
    _bccReadOnly->setPlaceholderText("Bcc");

    headerLayout->addWidget(_fromReadOnly, 0, 0, Qt::AlignLeft);
    headerLayout->addWidget(_toReadOnly, 1, 0, Qt::AlignLeft);
    headerLayout->addWidget(_ccReadOnly, 2, 0, Qt::AlignLeft);
    headerLayout->addWidget(_bccReadOnly, 3, 0, Qt::AlignLeft);
    headerLayout->addWidget(_subjectReadOnly, 4, 0, Qt::AlignLeft);

    headerLayout->setRowStretch(0, 0);
    headerLayout->setRowStretch(1, 0);
    headerLayout->setRowStretch(2, 0);
    headerLayout->setRowStretch(3, 0);
    headerLayout->setRowStretch(4, 0);

    _fromReadOnly->setVisible(false);
    _toReadOnly->setVisible(false);
    _subjectReadOnly->setVisible(false);
    _ccReadOnly->setVisible(false);
    _bccReadOnly->setVisible(false);

    _Mlayout->addWidget(_mailContent, 0, 1);

    _display = new QWebEngineView(this);
    _display->setHtml(QString(""));
    _display->setProperty("current-mail", "");
    _display->resize(970, 750);
    _display->show();
    _display->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    headerLayout->addWidget(_display, 5, 0);
}

void MailboxMenu::clear() {
    if(_attachmentList != nullptr) _attachmentList->delAll();
    if (_display != nullptr) _display->setHtml("");
    _fromReadOnly->setVisible(false);
    _toReadOnly->setVisible(false);
    _subjectReadOnly->setVisible(false);
    _ccReadOnly->setVisible(false);
    _bccReadOnly->setVisible(false);
}

void MailboxMenu::showMail(const QModelIndex& idx) {
    QStandardItemModel* mdl = qobject_cast<QStandardItemModel*>(_tree->model());
    if (mdl == nullptr) return;
    if (_display == nullptr) return;
    QStandardItem* itm = mdl->itemFromIndex(idx);
    QVariant itmDat = itm->data();
    if(!itmDat.isValid() || itmDat.isNull()) return;

    QVariant validator = _display->property("current-mail");
    if(!validator.isNull() && validator.isValid()) {
        if (validator.value<QString>() == itmDat.toString()) return;
    }

    if(_currentTemp != nullptr && _currentTemp->isValid()) {
        delete _currentTemp;
    }

    clear();

    QFile fileHandler(itmDat.toString());
    if (!fileHandler.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&fileHandler);
    std::string content = in.readAll().toStdString();
    fileHandler.close();

    // read
    QString tfileName = QFileInfo(itmDat.toString()).fileName();
    if(MainWindow::_mailboxInstance) {
        const std::unique_ptr<ClientMailbox>& mbl = MainWindow::_mailboxInstance;
        if (!mbl->mailIsRead(tfileName.toStdString())) {
            mbl->setMailIsRead(tfileName.toStdString());
            refetchTree();
        }
    }

    MIMESegment e = parseMIMEEntity(content);

    QString fromList = QString::fromStdString(e.header().get("From").full());
    QString toList = QString::fromStdString(e.header().get("To").full());
    QString ccList = QString::fromStdString(e.header().get("Cc").full());
    QString bccList = QString::fromStdString(e.header().get("Bcc").full());

    if(fromList.length() > 0) {
        _fromReadOnly->setText(fromList);
        _fromReadOnly->setVisible(true);
    }
    if (toList.length() > 0) {
        _toReadOnly->setText(toList);
        _toReadOnly->setVisible(true);
    }
    if (ccList.length() > 0) {
        _ccReadOnly->setText(ccList);
        _ccReadOnly->setVisible(true);
    }
    if (bccList.length() > 0) {
        _bccReadOnly->setText(bccList);
        _bccReadOnly->setVisible(true);
    }

    /*if(fromList.size() > 0) {
        QString label = "From: ";
        int i = 0;
        for (const mimetic::Mailbox& mb : fromList) {
            QString mailString = "";
            if (mb.label().length() > 0 && mb.label().find("\"\"") == mb.label().npos) {
                mailString.append(mb.label() + " " + mb.mailbox() + "@" + mb.domain());
            } else {
                mailString.append(mb.mailbox() + "@" + mb.domain());
            }
            label.append(mailString);
            if(i < fromList.size() - 1) {
                label.append(", ");
            }
            i++;
        }
        _fromReadOnly->setText(label);
        _fromReadOnly->setVisible(true);
    } else {
        _fromReadOnly->setVisible(false);
    }
    if (toList.size() > 0) {
        QString label = "To: ";
        int i = 0;
        for (const mimetic::Address& addr : toList) {
            QString mailString = "";
            if (addr.mailbox().label().length() > 0 && addr.mailbox().label().find("\"\"") == addr.mailbox().label().npos) {
                mailString.append(addr.mailbox().label() + " " + addr.mailbox().mailbox() + "@" + addr.mailbox().domain());
            }
            else {
                mailString.append(addr.mailbox().mailbox() + "@" + addr.mailbox().domain());
            }
            label.append(mailString);
            if (i < toList.size() - 1) {
                label.append(", ");
            }
            i++;
        }
        _toReadOnly->setText(label);
        _toReadOnly->setVisible(true);
    } else {
        _toReadOnly->setVisible(false);
    }
    if (ccList.size() > 0) {
        QString label = "Cc: ";
        int i = 0;
        for (const mimetic::Address& addr : ccList) {
            QString mailString = "";
            if (addr.mailbox().label().length() > 0 && addr.mailbox().label().find("\"\"") == addr.mailbox().label().npos) {
                mailString.append(addr.mailbox().label() + " " + addr.mailbox().mailbox() + "@" + addr.mailbox().domain());
            }
            else {
                mailString.append(addr.mailbox().mailbox() + "@" + addr.mailbox().domain());
            }
            label.append(mailString);
            if (i < ccList.size() - 1) {
                label.append(", ");
            }
            i++;
        }
        _ccReadOnly->setText(label);
        _ccReadOnly->setVisible(true);
    } else {
        _ccReadOnly->setVisible(false);
    }
    if (bccList.size() > 0) {
        QString label = "Bcc: ";
        int i = 0;
        for (const mimetic::Address& addr : bccList) {
            QString mailString = "";
            if (addr.mailbox().label().length() > 0 && addr.mailbox().label().find("\"\"") == addr.mailbox().label().npos) {
                mailString.append(addr.mailbox().label() + " " + addr.mailbox().mailbox() + "@" + addr.mailbox().domain());
            }
            else {
                mailString.append(addr.mailbox().mailbox() + "@" + addr.mailbox().domain());
            }
            label.append(mailString);
            if (i < bccList.size() - 1) {
                label.append(", ");
            }
            i++;
        }
        _bccReadOnly->setText(label);
        _bccReadOnly->setVisible(true);
    } else {
        _bccReadOnly->setVisible(false);
    }*/
    
    _subjectReadOnly->setText(QString::fromStdString(e.header().get("Subject").full()));
    _subjectReadOnly->setVisible(true);

    MIMESegment res;
    getMIMETextEntity(&e, res);
    QString cnt = QString::fromStdString(Encoder::decode_quoted_printable(res.body().rawContent()));

    std::vector<MIMESegment> attachments;
    getMIMEAttachments(e, attachments);

    _currentTemp = new QTemporaryDir("./");
    QString relPath = "";
    if (_currentTemp->isValid()) {
        relPath = _currentTemp->path();
        relPath.append("/.");               // QUrl bug
    }

    if(_currentTemp->isValid()) {
        for (MIMESegment att : attachments) {
            std::string filename = att.header().get("Content-Disposition").param("filename");
            QString fileNewPath = _currentTemp->path();
            fileNewPath.append("/" + filename);

            if(att.header().get("Content-Type").value().find("image") != std::string::npos) {
                QImage img;
                std::string decoded = Encoder::decode_base64(att.body().rawContent(), true);
                img.loadFromData(decoded);
                img.save(fileNewPath);
            } else {
                std::ofstream fl(fileNewPath.toStdString());
                if (fl.is_open()) {
                    std::string decoded = Encoder::decode_base64(att.body().rawContent(), true);
                    fl << decoded;
                }
                fl.close();
            }
            _attachmentList->add(QFileInfo(fileNewPath));
        }
    }
    _display->setHtml(cnt, QUrl::fromUserInput(relPath));
}

#include "moc_MailboxMenu.cpp"