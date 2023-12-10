#ifndef _CONTENT_WIDGET_H_
#define _CONTENT_WIDGET_H_

#include <QtWidgets/QWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QToolBar>
#include <QAction>

#include "BoxShadowEffect.h"

class ContentWidget : public QWidget {
    Q_OBJECT
private:
    QTextEdit* _body;
    QToolBar* _toolbar;
public:
    void showEvent(QShowEvent*);
    ContentWidget(QWidget*);
    void initToolbar();
    inline QTextEdit* body() {     return _body;     }
    virtual ~ContentWidget() {}
};

#endif