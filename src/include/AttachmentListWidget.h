#ifndef _ATTACHMENT_LIST_WIDGET_H_
#define _ATTACHMENT_LIST_WIDGET_H_

#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QAction>
#include <QtWidgets/QPushButton>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <QDesktopServices>

#include "BoxShadowEffect.h"
#include "SendMenu.h"

class AttachmentListWidget : public QWidget {
    Q_OBJECT
private:
    QHBoxLayout* _Mlayout;
    QFileInfoList _attachments;
    qreal _totalSize;
    bool _deletable;
    int _offsetX;
    int _offsetY;

private:
    QPushButton* getAttachmentWidget(const QString&);
    void showEvent(QShowEvent*);
    void execDel(QWidget*);
    void execShow(QWidget*);

private slots:
    void delSig();
public slots:
    void add(const QFileInfo&);

public:
    AttachmentListWidget(QWidget*, bool = true, int = 0, int = 0);
    void delAll();

    QFileInfoList attachments();
    virtual ~AttachmentListWidget() {}
};

#endif