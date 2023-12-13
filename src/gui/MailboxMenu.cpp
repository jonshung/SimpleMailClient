#include "MailboxMenu.h"

MailboxMenu::MailboxMenu(QWidget* parent) : QWidget(parent) {
    QGridLayout* mLayout = new QGridLayout();
    mLayout->setContentsMargins(20, 20, 20, 20);
    setLayout(mLayout);
    _tree = new QTreeView();
    _tree->setFixedWidth(200);
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

    QWebEngineGlobalSettings::DnsMode dnsMode;
    dnsMode.secureMode = QWebEngineGlobalSettings::SecureDnsMode::SystemOnly;
    QWebEngineGlobalSettings::setDnsMode(dnsMode);
    _display = new QWebEngineView(this);
    _display->setHtml(QString(""));
    _display->setProperty("current-mail", "");
    _display->resize(970, 750);
    _display->show();
    _display->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    mLayout->addWidget(_tree, 0, 0, Qt::AlignLeft);
    mLayout->addWidget(b1, 1, 0, Qt::AlignCenter);
    mLayout->addWidget(_display, 0, 1);
}

void MailboxMenu::fetch() {
    if(MainWindow::_mailboxInstance == nullptr) return;
    MainWindow::_mailboxInstance->fetch("localhost", "2226");
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

    json filterData = MainWindow::_mailboxInstance->getFolders();

    for (auto& filter : filterData.items()) {
        QStandardItem* filterItem = new QStandardItem(QString::fromStdString(filter.key()));
        parentItem->appendRow(filterItem);
        QDir filterDir(filterDirPath / filter.key());
        if (!filterDir.exists()) {
            std::filesystem::create_directories(filterDir.filesystemPath());
        }
        for (const QFileInfo fInfo : filterDir.entryInfoList()) {
            if (!fInfo.isFile() || fInfo.suffix() != "msg") continue;
            QFile fileHandler(fInfo.filePath());
            if (!fileHandler.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

            QTextStream in(&fileHandler);
            std::string content = in.readAll().toStdString();
            mimetic::MimeEntity e = parseMIMEEntity(content);
            QString subj = QString::fromStdString(extractSubject(&e));
            QStandardItem* mailEntry = new QStandardItem(subj);
            mailEntry->setData(fInfo.filePath());
            filterItem->appendRow(mailEntry);
        }
    }
    model->setHeaderData(0, Qt::Orientation::Horizontal, "Mails");
    _tree->setModel(model);
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

    QFile fileHandler(itmDat.toString());
    if (!fileHandler.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&fileHandler);
    std::string content = in.readAll().toStdString();
    mimetic::MimeEntity e = parseMIMEEntity(content);
    mimetic::MimeEntity res;
    getMIMETextEntity(&e, res);
    mimetic::QP::Decoder decoderQp = mimetic::QP::Decoder();
    mimetic::Base64::Decoder decoder64 = mimetic::Base64::Decoder();
    res.body().code(decoderQp);
    QString cnt = QString::fromStdString(res.body());

    std::vector<mimetic::MimeEntity*> attachments;
    getMIMEAttachments(&e, attachments);
    _currentTemp = new QTemporaryDir();
    QString relPath = "";
    if (_currentTemp->isValid()) {
        relPath = _currentTemp->path();
        relPath.append("/.");               // QUrl bug
    }

    if(_currentTemp->isValid()) {
        for (mimetic::MimeEntity* att : attachments) {
            att->body().code(decoder64);
            std::string filename = att->header().contentDisposition().param("filename");
            QImage img;
            img.loadFromData(att->body());
            QString imgSavePath = _currentTemp->path();
            imgSavePath.append("/" + filename);
            img.save(imgSavePath);
        }
    }
    _display->setHtml(cnt, QUrl::fromUserInput(relPath));
}

#include "moc_MailboxMenu.cpp"