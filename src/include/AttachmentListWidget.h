#ifndef _ATTACHMENT_LIST_WIDGET_H_
#define _ATTACHMENT_LIST_WIDGET_H_

#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QAction>
#include <QtWidgets/QPushButton>
#include <QFileInfo>
#include <QMessageBox>

#include "BoxShadowEffect.h"
#include "SendMenu.h"

class AttachmentListWidget : public QWidget {
    Q_OBJECT
private:
    QHBoxLayout* _Mlayout;
    QStringList _attachments;

private:
    QPushButton* getAttachmentWidget(const QString&);
    void showEvent(QShowEvent*);

signals:
    void delAttachment(int);

private slots:
    void execDel();

public slots:
    void add(const QString&);

public:
    AttachmentListWidget(QWidget*);

    QStringList attachments();
    virtual ~AttachmentListWidget() {}
};

#endif