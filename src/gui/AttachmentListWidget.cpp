#include "AttachmentListWidget.h"

AttachmentListWidget::AttachmentListWidget(QWidget* parent, bool deletable, int offsetX, int offsetY) : QWidget(parent) {
    setMaximumHeight(90);
    _Mlayout = new QHBoxLayout();
    setLayout(_Mlayout);
    _Mlayout->setContentsMargins(25, 0, 0, 10);
    _Mlayout->addStretch(1);
    _totalSize = 0;
    _deletable = deletable;
    _offsetX = offsetX;
    _offsetY = offsetY;

    QPalette p = palette();
    setAutoFillBackground(true);
    setPalette(p);
    show();
}

void AttachmentListWidget::add(const QFileInfo& fInfo) {
    if (!fInfo.isFile()) return;
    const std::unique_ptr<ConfigProvider>& configData = ConfigProvider::_configProvider;
    if ((fInfo.size() + _totalSize) > configData->maxFileSize()) {
        QMessageBox warnBox(parentWidget());
        warnBox.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        warnBox.setText("Files size limit is " + QString::number(configData->maxFileSize() / (1024.0 * 1024.0)) + "MB");
        warnBox.exec();
        return;
    } 

    _attachments.append(fInfo);
    QPushButton* wid = getAttachmentWidget(fInfo.filePath());
    if(_deletable) wid->setStatusTip("Remove " + wid->text());
    else wid->setStatusTip("Open " + wid->text());
    AttachmentListWidget::connect(wid, &QPushButton::clicked, this, &AttachmentListWidget::delSig);
    int pos = _Mlayout->count() - 1;
    _Mlayout->insertWidget(pos, wid);
    _totalSize += fInfo.size();
}

void AttachmentListWidget::delAll() {
    for (QWidget* obj : findChildren<QWidget*>()) {
        execDel(obj);
    }
}

void AttachmentListWidget::delSig() {
    QObject* obj = sender();
    QPushButton* wid = qobject_cast<QPushButton*>(obj);
    if (wid == nullptr) return;
    if (!_deletable) {
        execShow(wid);
        return;
    }

    QMessageBox confirm(this);
    confirm.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    confirm.setText("Do you want to delete " + wid->text() + "?");
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    confirm.setDefaultButton(QMessageBox::Cancel);

    int ret = confirm.exec();
    if(ret == QMessageBox::Cancel) return;
    execDel(wid);
}

void AttachmentListWidget::execDel(QWidget* wid) {
    QVariant vl = wid->property("filename");
    int idx = -1;
    if (vl.isValid() && !vl.isNull()) {
        idx = _Mlayout->indexOf(wid);
    }

    _Mlayout->removeWidget(wid);
    wid->hide();
    delete wid;
    if (idx != -1 && idx < _attachments.size()) {
        _totalSize -= _attachments.at(idx).size();
        _attachments.remove(idx);
    }
}

void AttachmentListWidget::execShow(QWidget* wid) {
    QVariant vl = wid->property("filename");
    int idx = -1;
    if (!vl.isValid() || vl.isNull()) return;
    QString path = vl.toString();
    QFileInfo info(path);
#if ( defined (_WIN32) || defined (_WIN64) )
    QStringList args;
    if (!info.isDir())
        args << "/select,";
    args << QDir::toNativeSeparators(path);
    if (QProcess::startDetached("explorer", args))
        return;
#elif defined(Q_OS_MAC)
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \"" + path + "\"";
    args << "-e";
    args << "end tell";
    args << "-e";
    args << "return";
    if (!QProcess::execute("/usr/bin/osascript", args))
        return;
#endif
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.isDir() ? path : info.filePath()));
}

QPushButton* AttachmentListWidget::getAttachmentWidget(const QString& filename) {
    QPushButton* wid = new QPushButton();
    QFileInfo fInfo(filename);
    wid->setProperty("filename", filename);
    wid->setText(fInfo.fileName());
    QString styleSheet = "QPushButton { font-size: 10px; border: none; font-weight: bold; background-color: white; border-radius: 10px; border-width: thin; } "
        "QPushButton:hover { background-color: #e84d5a; font-weight: bolder; color: white; } "
        "QPushButton:hover:pressed { background-color: #bf2633; font-weight: 900; } ";
    if (!_deletable) {
            styleSheet = "QPushButton { font-size: 10px; border: none; font-weight: bold; background-color: white; border-radius: 10px; border-width: thin; } "
            "QPushButton:hover { background-color: #4287f5; font-weight: bolder; color: white; } "
            "QPushButton:hover:pressed { background-color: #2268d6; font-weight: 900; } ";
    }
    wid->setStyleSheet(styleSheet);
    wid->setFixedSize(160, 40);
    if(fInfo.suffix() == "jpg" || fInfo.suffix() == "png" || fInfo.suffix() == "bmp") {
        wid->setIcon(QIcon(filename));
        wid->setIconSize(QSize(30, 30));
    }

    BoxShadowEffect* centerShadowEffect = new BoxShadowEffect();
    centerShadowEffect->setBlurRadius(30.0);
    centerShadowEffect->setDistance(3.0);
    centerShadowEffect->setColor(QColor(0, 0, 0, 20));

    wid->setGraphicsEffect(centerShadowEffect);
    return wid;
}

void AttachmentListWidget::showEvent(QShowEvent* ev) {
    QRect basedGeometry = parentWidget()->geometry();
    int targetH = basedGeometry.bottomLeft().x() + _offsetX;
    int targetV = basedGeometry.bottomLeft().y() - 70 + _offsetY;
    SendMenu* castexpr = qobject_cast<SendMenu*>(parentWidget());
    if (castexpr != nullptr && castexpr->content() != nullptr && castexpr->content()->toolbar() != nullptr) {
        stackUnder(castexpr->content()->toolbar());
    }
    setGeometry(targetH, targetV, basedGeometry.width(), 80);
}

QFileInfoList AttachmentListWidget::attachments() {
    return _attachments;
}

#include "moc_AttachmentListWidget.cpp"