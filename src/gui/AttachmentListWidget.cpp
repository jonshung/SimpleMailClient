#include "AttachmentListWidget.h"


AttachmentListWidget::AttachmentListWidget(QWidget* parent) : QWidget(parent) {
    setMaximumHeight(90);
    _Mlayout = new QHBoxLayout();
    setLayout(_Mlayout);
    _Mlayout->setContentsMargins(25, 0, 0, 10);

    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::blue);
    setAutoFillBackground(true);
    setPalette(p);
    show();
}

void AttachmentListWidget::add(const QString& filename) {
    _attachments.append(filename);
    QPushButton* wid = getAttachmentWidget(filename);
    wid->setStatusTip("Remove " + wid->text());
    AttachmentListWidget::connect(wid, &QPushButton::clicked, this, &AttachmentListWidget::execDel);
    _Mlayout->addWidget(wid, 0, Qt::AlignLeft);
}

void AttachmentListWidget::execDel() {
    QObject* obj = sender();
    QPushButton* wid = qobject_cast<QPushButton*>(obj);
    if (wid == nullptr) return;

    QMessageBox confirm(this);
    confirm.setText("Do you want to delete " + wid->text() + "?");
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    confirm.setDefaultButton(QMessageBox::Cancel);

    int ret = confirm.exec();
    if(ret == QMessageBox::Cancel) return;

    QVariant vl = wid->property("filename");
    if(!vl.isValid() || vl.isNull()) return;

    int idx = _Mlayout->indexOf(wid);
    _Mlayout->removeWidget(wid);
    wid->hide();
    delete wid;
    if (idx != -1 && idx < _attachments.size()) {
        _attachments.remove(idx);
    }
}

QPushButton* AttachmentListWidget::getAttachmentWidget(const QString& filename) {
    QPushButton* wid = new QPushButton();
    QFileInfo fInfo(filename);
    wid->setProperty("filename", filename);
    wid->setText(fInfo.fileName());
    wid->setStyleSheet("QPushButton { font-size: 10px; border: none; background-color: white; border-radius: 10px;} "
                       "QPushButton:hover { background-color: #e84d5a; font-weight: bold; color: white; } "
                       "QPushButton:hover:pressed { background-color: #bf2633; font-weight: 900; } ");
    wid->setFixedSize(160, 40);
    if(fInfo.suffix() == "jpg" || fInfo.suffix() == "png") {
        wid->setIcon(QIcon(filename));
        wid->setIconSize(QSize(30, 30));
    }

    BoxShadowEffect* centerShadowEffect = new BoxShadowEffect();
    centerShadowEffect->setBlurRadius(30.0);
    centerShadowEffect->setDistance(6.0);
    centerShadowEffect->setColor(QColor(0, 0, 0, 65));

    wid->setGraphicsEffect(centerShadowEffect);
    return wid;
}


void AttachmentListWidget::showEvent(QShowEvent* ev) {
    QRect basedGeometry = parentWidget()->geometry();
    int targetH = basedGeometry.bottomLeft().x();
    int targetV = basedGeometry.bottomLeft().y() - 70;
    SendMenu* castexpr = qobject_cast<SendMenu*>(parentWidget());
    if (castexpr == nullptr || castexpr->content() == nullptr || castexpr->content()->toolbar() == nullptr) return;
    stackUnder(castexpr->content()->toolbar());
    setGeometry(0, targetV, basedGeometry.width(), 80);
}

QStringList AttachmentListWidget::attachments() {
    return _attachments;
}

#include "moc_AttachmentListWidget.cpp"