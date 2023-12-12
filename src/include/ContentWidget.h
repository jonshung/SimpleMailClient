#ifndef _CONTENT_WIDGET_H_
#define _CONTENT_WIDGET_H_

#include <QtWidgets/QWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QToolBar>
#include <QAction>
#include <QToolButton>
#include <QMenu>
#include <QEvent>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtGui/QFontDatabase>
#include <QtCore/QHash>
#include <QShortcut>
#include <QtWidgets/QFileDialog>
#include <utility>

#include "BoxShadowEffect.h"
#include "AttachmentListWidget.h"
#include "SendMenu.h"

class AttachmentListWidget;

class ContentWidget : public QWidget {
    Q_OBJECT
private:
    inline static const QHash fontSizes = {  std::make_pair<QString, int>("Small", 11), 
                                            std::make_pair<QString, int>("Normal", 15), 
                                            std::make_pair<QString, int>("Large", 20),
                                            std::make_pair<QString, int>("Very large", 36) };
    QTextEdit* _body;
    QToolBar* _toolbar;
    AttachmentListWidget* _attachmentList;

private:
    QToolButton* _undo;
    QToolButton* _redo;
    QToolButton* _fontChooser;
    QToolButton* _sizeChooser;
    QToolButton* _bold;
    QToolButton* _italic;
    QToolButton* _underline;
    QToolButton* _alignChooser;
    QToolButton* _indentIncrease;
    QToolButton* _indentDecrease;
    QToolButton* _indent;
    QToolButton* _outdent;
    QToolButton* _attachment;
    QToolButton* _submit;

signals:
    void attachmentSignal(const QString&);
private slots:
    void chooseSize(QAction*);
    void chooseFont(QAction*);

    void bold();
    void italic();
    void underline();

    void setAlign(Qt::Alignment);
    void setAlignLeft();
    void setAlignRight();
    void setAlignCenter();
    void setAlign(QAction*);

    void indent();
    void outdent();

    void attachment();

    void trackFont(const QTextCharFormat&);
    void trackCursor();

public:
    bool eventFilter(QObject*, QEvent*);
    void showEvent(QShowEvent*);
    ContentWidget(QWidget*);

    void initToolbar();
    void initFontChooser();
    void initSizeChooser();
    void initAlignChooser();

    QToolBar* toolbar();
    AttachmentListWidget* attachmentListWidget();

    inline QTextEdit* body() {     return _body;     }
    virtual ~ContentWidget() {}
};

#endif