#include "ContentWidget.h"

ContentWidget::ContentWidget(QWidget* parent) : QWidget(parent) {
    QGridLayout* mainLayout = new QGridLayout();
    setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 0, 55);

    _body = new QTextEdit();
    QFont font = _body->font();
    _body->setFontPointSize(10);    // default
    _body->document()->setDocumentMargin(qreal(10));
    _body->setBaseSize(parent->geometry().width() * 0.85, parent->geometry().height() * 0.85);
    _body->setStyleSheet("font-size: 10px; border: none;");

    _toolbar = new QToolBar(parent);
    mainLayout->addWidget(_body, 0, 0);
    initToolbar();
}

void ContentWidget::initToolbar() {
    if (_toolbar == nullptr) return;
    BoxShadowEffect* centerShadowEffect = new BoxShadowEffect();
    _toolbar->setContentsMargins(20, 0, 0, 0);

    centerShadowEffect->setBlurRadius(30.0);
    centerShadowEffect->setDistance(10.0);
    centerShadowEffect->setColor(QColor(0, 0, 0, 35));

    _toolbar->setAutoFillBackground(true);
    _toolbar->setGraphicsEffect(centerShadowEffect);
    _toolbar->setIconSize(QSize(20, 20));
    QAction* _undo = new QAction(this);
    //_undo->setText("Undo");
    _undo->setIcon(QIcon(":/resources/undo.png"));
    QAction* _redo = new QAction(this);
    //_redo->setText("Redo");
    _redo->setIcon(QIcon(":/resources/redo.png"));

    ContentWidget::connect(_undo, &QAction::triggered, _body, &QTextEdit::undo);
    ContentWidget::connect(_redo, &QAction::triggered, _body, &QTextEdit::redo);

    _toolbar->addAction(_undo);
    _toolbar->addAction(_redo);
    QWidget* sep1 = _toolbar->widgetForAction(_toolbar->addSeparator());

    _toolbar->setStyleSheet("QToolButton { width: 25px; height: 25px; margin-left: 10px; border: none; border-radius: 5px; } "
                            "QToolButton:hover { background-color: #e3e3e3; } "
                            "QToolButton:pressed { background-color: #bdbdbd; } "
                            "QToolBar::separator { padding-left: 20; padding-right: 20; width: 1; } "
                            "QToolBar { background: white; border: none; border-radius: 10px; } ");
    _toolbar->raise();
}

void ContentWidget::showEvent(QShowEvent* ev) {
    BoxShadowEffect* centerShadowEffect = (BoxShadowEffect*)_toolbar->graphicsEffect();
    QRect basedGeometry = _body->geometry();
    int centerH = basedGeometry.center().x() - (basedGeometry.width() - centerShadowEffect->distance() * 5) / 2;
    int centerV = this->geometry().bottom() - 30 - centerShadowEffect->distance() * 2;
    _toolbar->setGeometry(centerH, centerV, basedGeometry.width() - centerShadowEffect->distance() * 5, 30);
    //this->resize(basedGeometry.width() + centerShadowEffect->distance() * 2, basedGeometry.height());
}

#include "moc_ContentWidget.cpp"