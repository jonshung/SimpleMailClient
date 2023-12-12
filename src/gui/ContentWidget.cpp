#include "ContentWidget.h"

ContentWidget::ContentWidget(QWidget* parent) : QWidget(parent) {
    QGridLayout* mainLayout = new QGridLayout();
    setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 20, 65);

    _body = new QTextEdit();
    QFont font = _body->font();
    _body->document()->setDocumentMargin(qreal(10));
    _body->setMinimumSize(parent->geometry().width() * 0.85, parent->geometry().height() * 0.65);
    _body->setStyleSheet("font-size: 10px; border: none;");
    ContentWidget::connect(_body, &QTextEdit::currentCharFormatChanged, this, &ContentWidget::trackFont);
    ContentWidget::connect(_body, &QTextEdit::cursorPositionChanged, this, &ContentWidget::trackCursor);

    _toolbar = new QToolBar(parent);
    _attachmentList = new AttachmentListWidget(parent);
    ContentWidget::connect(this, &ContentWidget::attachmentSignal, _attachmentList, &AttachmentListWidget::add);

    mainLayout->addWidget(_body, 0, 0, Qt::AlignTop);
    initToolbar();
}

QToolBar* ContentWidget::toolbar() {
    return _toolbar;
}

void ContentWidget::initToolbar() {
    if (_toolbar == nullptr) return;
    BoxShadowEffect* centerShadowEffect = new BoxShadowEffect();
    _toolbar->setContentsMargins(20, 0, 0, 0);

    _toolbar->setStyleSheet("QToolButton { width: 25px; height: 25px; margin-left: 10px; border: none; border-radius: 5px; } "
        "QToolButton:hover { background-color: #efefefef; } "
        "QToolButton:pressed { background-color: #e3e3e3e3; } "
        "QToolButton:checked { background-color: #e3e3e3e3; } "
        "QToolBar::separator { padding-left: 5; padding-right: 5; width: 1; } "
        "QToolBar { background: white; border: none; border-radius: 5px; } ");

    centerShadowEffect->setBlurRadius(30.0);
    centerShadowEffect->setDistance(10.0);
    centerShadowEffect->setColor(QColor(0, 0, 0, 35));

    _toolbar->setAutoFillBackground(true);
    _toolbar->setGraphicsEffect(centerShadowEffect);
    _toolbar->setIconSize(QSize(16, 16));

    _undo = new QToolButton();
    QAction* undoAction = new QAction(_undo);
    undoAction->setIcon(QIcon(":/resources/undo.png"));
    undoAction->setToolTip("Undo (Ctrl-Z)");
    _undo->setDefaultAction(undoAction);
    ContentWidget::connect(undoAction, &QAction::triggered, body(), &QTextEdit::undo);

    _redo = new QToolButton();
    QAction* redoAction = new QAction(_redo);
    redoAction->setIcon(QIcon(":/resources/redo.png"));
    redoAction->setToolTip("Redo (Ctrl-Z)");
    _redo->setDefaultAction(redoAction);
    ContentWidget::connect(redoAction, &QAction::triggered, body(), &QTextEdit::redo);

    initFontChooser();
    initSizeChooser();

    _bold = new QToolButton();
    QAction* boldAction = new QAction(_bold);
    boldAction->setIcon(QIcon(":/resources/bold.png"));
    boldAction->setToolTip("Bold (Ctrl-B)");
    boldAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
    _bold->setDefaultAction(boldAction);
    ContentWidget::connect(boldAction, &QAction::triggered, this, &ContentWidget::bold);
    _bold->setCheckable(true);

    _italic = new QToolButton();
    QAction* italicAction = new QAction(_italic);
    italicAction->setIcon(QIcon(":/resources/italic.png"));
    italicAction->setToolTip("Italic (Ctrl-I)");
    italicAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    _italic->setDefaultAction(italicAction);
    ContentWidget::connect(italicAction, &QAction::triggered, this, &ContentWidget::italic);
    _italic->setCheckable(true);

    _underline = new QToolButton();
    QAction* underlineAction = new QAction(_underline);
    underlineAction->setIcon(QIcon(":/resources/underline.png"));
    underlineAction->setToolTip("Underline (Ctrl-U)");
    _underline->setStyleSheet("QToolButton { margin-right: 10px; }");
    underlineAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_U));
    _underline->setDefaultAction(underlineAction);
    ContentWidget::connect(underlineAction, &QAction::triggered, this, &ContentWidget::underline);
    _underline->setCheckable(true);

    initAlignChooser();

    _outdent = new QToolButton();
    QAction* outdentAction = new QAction(_outdent);
    outdentAction->setIcon(QIcon(":/resources/outdent.png"));
    outdentAction->setToolTip("Increase indent (Ctrl-[)");
    outdentAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
    _outdent->setDefaultAction(outdentAction);
    ContentWidget::connect(outdentAction, &QAction::triggered, this, &ContentWidget::outdent);

    _indent = new QToolButton();
    QAction* indentAction = new QAction(_indent);
    indentAction->setIcon(QIcon(":/resources/indent.png"));
    indentAction->setToolTip("Increase indent (Ctrl-])");
    indentAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
    _indent->setDefaultAction(indentAction);
    _indent->setStyleSheet("QToolButton { margin-right: 10px; }");
    ContentWidget::connect(indentAction, &QAction::triggered, this, &ContentWidget::indent);

    _attachment = new QToolButton();
    QAction* attachmentAction = new QAction(_attachment);
    attachmentAction->setIcon(QIcon(":/resources/attachment.png"));
    attachmentAction->setToolTip("Add an attachment");
    _attachment->setDefaultAction(attachmentAction);
    _attachment->setStyleSheet("QToolButton { margin-right: 5px; }");
    ContentWidget::connect(attachmentAction, &QAction::triggered, this, &ContentWidget::attachment);

    _submit = new QToolButton();
    QAction* submitAction = new QAction(_submit);
    submitAction->setToolTip("Send mail");
    submitAction->setText("&Send");
    _submit->setDefaultAction(submitAction);
    _submit->setStyleSheet("QToolButton { font-size: 14px; font-weight: bold; margin-left: 20px; color: white; background-color: #1a73e8; } "
                           "QToolButton:hover { background-color: #2e7de6; } "
                           "QToolButton:pressed { background-color: #1e66c7 }");
    _submit->setFixedSize(130, 30);
    ContentWidget::connect(submitAction, &QAction::triggered, qobject_cast<SendMenu*>(parentWidget()), &SendMenu::submit);

    _toolbar->addWidget(_undo);
    _toolbar->addWidget(_redo);
    _toolbar->addSeparator();
    _toolbar->addWidget(_fontChooser);
    _toolbar->addSeparator();
    _toolbar->addWidget(_sizeChooser);
    _toolbar->addSeparator();
    _toolbar->addWidget(_bold);
    _toolbar->addWidget(_italic);
    _toolbar->addWidget(_underline);
    _toolbar->addSeparator();
    _toolbar->addWidget(_alignChooser);
    _toolbar->addSeparator();
    _toolbar->addWidget(_outdent);
    _toolbar->addWidget(_indent);
    _toolbar->addSeparator();
    _toolbar->addWidget(_attachment);
    _toolbar->addWidget(_submit);

    _toolbar->raise();
}


void ContentWidget::initFontChooser() {
    _fontChooser = new QToolButton();
    _fontChooser->setToolTip("Font family");
    _fontChooser->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
    _fontChooser->setStyleSheet("QToolButton { font-size: 13px; padding-left: 0; padding-right: 20px; margin-right: 10px; } "
                                "QToolButton::menu-indicator { bottom: 0px; width: 20px; height: 20px; }");
    _fontChooser->setFixedSize(100, 20);
    QMenu* fontMenu = new QMenu();
    fontMenu->setWindowFlags(fontMenu->windowFlags() | Qt::FramelessWindowHint);
    fontMenu->installEventFilter(this);
    _fontChooser->setMenu(fontMenu);

    QStringList fontFamilies = QFontDatabase::families();
    for (const QString& fontInfo : fontFamilies) {
        QAction* fontAction = new QAction();
        fontAction->setData(fontInfo);
        fontAction->setText(fontInfo);
        fontAction->setFont(QFont(fontInfo, 10));
        fontMenu->addAction(fontAction);
        QMenu::connect(fontMenu, &QMenu::triggered, this, &ContentWidget::chooseFont);
    }
}

void ContentWidget::initSizeChooser() {
    _sizeChooser = new QToolButton();
    _sizeChooser->setToolTip("Font size");
    _sizeChooser->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
    _sizeChooser->setStyleSheet("QToolButton { font-size: 13px; padding-left: 0; padding-right: 20px; margin-right: 10px; } "
                                "QToolButton::menu-indicator { bottom: 0px; width: 20px; height: 20px; }");
    _sizeChooser->setFixedSize(100, 20);
    QMenu* sizeMenu = new QMenu();
    sizeMenu->setWindowFlags(sizeMenu->windowFlags() | Qt::FramelessWindowHint);
    sizeMenu->installEventFilter(this);
    _sizeChooser->setMenu(sizeMenu);

    for(auto [ sizeString, sizeInt ] : fontSizes.asKeyValueRange()) {
        QAction* sizeAction = new QAction();
        sizeAction->setData(sizeInt);
        sizeAction->setText(sizeString);
        sizeAction->setFont(QFont("Arial", 10));
        sizeMenu->addAction(sizeAction);
        QMenu::connect(sizeMenu, &QMenu::triggered, this, &ContentWidget::chooseSize);
    }
}

void ContentWidget::initAlignChooser() {
    _alignChooser = new QToolButton();
    _alignChooser->setToolTip("Align");
    _alignChooser->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
    _alignChooser->setStyleSheet("QToolButton { padding-left: 0; padding-left: 0; padding-right: 20px; margin-right: 10px; } "
                                "QToolButton::menu-indicator { bottom: 3px; width: 20px; height: 20px; }");
    _alignChooser->setIcon(QIcon(":/resources/align-left.png"));
    QMenu* alignMenu = new QMenu();
    alignMenu->setToolTipsVisible(true);
    alignMenu->setContentsMargins(5, 5, -85, 5);
    alignMenu->setWindowFlags(alignMenu->windowFlags() | Qt::FramelessWindowHint);
    alignMenu->installEventFilter(this);
    _alignChooser->setMenu(alignMenu);

    QAction* alignLeft = new QAction();
    alignLeft->setToolTip("Align left (Ctrl-Shift-L)");
    alignLeft->setIcon(QIcon(":/resources/align-left.png"));
    QShortcut* alignLeftShortCut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_L), body());
    ContentWidget::connect(alignLeftShortCut, &QShortcut::activated, this, &ContentWidget::setAlignLeft);
    ContentWidget::connect(alignLeft, &QAction::triggered, this, &ContentWidget::setAlignLeft);
    alignMenu->addAction(alignLeft);

    QAction* alignRight = new QAction();
    alignRight->setToolTip("Align right (Ctrl-Shift-R)");
    alignRight->setIcon(QIcon(":/resources/align-right.png"));
    QShortcut* alignRightShortCut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R), body());
    ContentWidget::connect(alignRightShortCut, &QShortcut::activated, this, &ContentWidget::setAlignRight);
    ContentWidget::connect(alignRight, &QAction::triggered, this, &ContentWidget::setAlignRight);
    alignMenu->addAction(alignRight);

    QAction* alignCenter = new QAction();
    alignCenter->setToolTip("Align center (Ctrl-Shift-E)");
    alignCenter->setIcon(QIcon(":/resources/align-center.png"));
    QShortcut* alignCenterShortCut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_E), body());
    ContentWidget::connect(alignCenterShortCut, &QShortcut::activated, this, &ContentWidget::setAlignCenter);
    ContentWidget::connect(alignCenter, &QAction::triggered, this, &ContentWidget::setAlignCenter);
    alignMenu->addAction(alignCenter);
}

void ContentWidget::showEvent(QShowEvent* ev) {
    BoxShadowEffect* centerShadowEffect = (BoxShadowEffect*)_toolbar->graphicsEffect();
    QRect basedGeometry = body()->geometry();
    int centerH = basedGeometry.center().x() - (basedGeometry.width() - centerShadowEffect->distance() * 5) / 2;
    int centerV = this->geometry().bottom() - 80 - centerShadowEffect->distance() * 2;
    _toolbar->setGeometry(centerH, centerV, basedGeometry.width() * 0.9 - centerShadowEffect->distance() * 5, 40);
    _fontChooser->setText(body()->font().family());

    _body->setFontPointSize(15);    // default
    _sizeChooser->setText(fontSizes.key(15));
    _body->setSizeIncrement(0, -80);
}

bool ContentWidget::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Show && (obj == _fontChooser->menu() || obj == _sizeChooser->menu() || obj == _alignChooser->menu())) {
        QMenu* castexpr = dynamic_cast<QMenu*>(obj);
        QPoint pos = castexpr->pos();
        pos.setY(pos.y() - castexpr->height() - 30);
        castexpr->move(pos);
        return true;
    }
    return false;
}

void ContentWidget::trackFont(const QTextCharFormat& ev) {
    if(ev.fontPointSize() < 11) {
        body()->setFontPointSize(15);
    }
    _fontChooser->setFont(ev.font().family());
    _fontChooser->setText(ev.font().family());

    _sizeChooser->setText(fontSizes.key(ev.font().pointSize(), "Normal"));
}

void ContentWidget::trackCursor() {
    Qt::Alignment al = body()->textCursor().blockFormat().alignment();
    switch (al) {
    case Qt::AlignLeft: {
        _alignChooser->setIcon(QIcon(":/resources/align-left.png"));
        break;
    }
    case Qt::AlignRight: {
        _alignChooser->setIcon(QIcon(":/resources/align-right.png"));
        break;
    }
    case Qt::AlignHCenter: {
        _alignChooser->setIcon(QIcon(":/resources/align-center.png"));
        break;
    }
    }
}

void ContentWidget::chooseFont(QAction* vl) {
    if(!vl->data().isValid() || vl->data().isNull()) return;
    QTextCharFormat currentFormat = body()->currentCharFormat();
    currentFormat.setFontFamilies(QStringList(vl->data().toString()));
    body()->setCurrentCharFormat(currentFormat);
}

void ContentWidget::chooseSize(QAction* vl) {
    if (!vl->data().isValid() || vl->data().isNull()) return;
    QTextCharFormat currentFormat = body()->currentCharFormat();
    currentFormat.setFontPointSize(vl->data().toInt());
    body()->setCurrentCharFormat(currentFormat);
}

void ContentWidget::bold() {
    QTextCharFormat currentFormat = body()->currentCharFormat();
    QFont currentFont = currentFormat.font();
    currentFont.setBold(!currentFont.bold());
    currentFormat.setFont(currentFont);
    body()->setCurrentCharFormat(currentFormat);
    _bold->setChecked(!_bold->isChecked());
}

void ContentWidget::italic() {
    QTextCharFormat currentFormat = body()->currentCharFormat();
    QFont currentFont = currentFormat.font();
    currentFont.setItalic(!currentFont.italic());
    currentFormat.setFont(currentFont);
    body()->setCurrentCharFormat(currentFormat);
    _italic->setChecked(!_italic->isChecked());
}

void ContentWidget::underline() {
    QTextCharFormat currentFormat = body()->currentCharFormat();
    QFont currentFont = currentFormat.font();
    currentFont.setUnderline(!currentFont.underline());
    currentFormat.setFont(currentFont);
    body()->setCurrentCharFormat(currentFormat);
    _underline->setChecked(!_underline->isChecked());
}

void ContentWidget::setAlign(QAction* vl) {
    if (!vl->data().isValid() || vl->data().isNull()) return;
    ContentWidget::setAlign(vl->data().value<Qt::Alignment>());
}

void ContentWidget::setAlign(Qt::Alignment vl) {
    QTextCursor cursor = body()->textCursor();
    QTextBlockFormat fmt = cursor.blockFormat();
    fmt.setAlignment(vl);
    cursor.mergeBlockFormat(fmt);
    body()->setTextCursor(cursor);
    switch (vl) {
    case Qt::AlignLeft: {
        _alignChooser->setIcon(QIcon(":/resources/align-left.png"));
        break;
    }
    case Qt::AlignRight: {
        _alignChooser->setIcon(QIcon(":/resources/align-right.png"));
        break;
    }
    case Qt::AlignHCenter: {
        _alignChooser->setIcon(QIcon(":/resources/align-center.png"));
        break;
    }
    }
}

void ContentWidget::setAlignLeft() {
    ContentWidget::setAlign(Qt::AlignLeft);
}

void ContentWidget::setAlignRight() {
    ContentWidget::setAlign(Qt::AlignRight);
}

void ContentWidget::setAlignCenter() {
    ContentWidget::setAlign(Qt::AlignHCenter);
}

void ContentWidget::indent() {
    QTextCursor cursor = body()->textCursor();
    QTextBlockFormat fmt = cursor.blockFormat();
    fmt.setIndent(fmt.indent() + 4);
    cursor.mergeBlockFormat(fmt);
    body()->setTextCursor(cursor);
}

void ContentWidget::outdent() {
    QTextCursor cursor = body()->textCursor();
    QTextBlockFormat fmt = cursor.blockFormat();
    int off = fmt.indent() - 4;
    fmt.setIndent((off < 0) ? 0 : off);
    cursor.mergeBlockFormat(fmt);
    body()->setTextCursor(cursor);
}

void ContentWidget::attachment() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    qint64 totalSize = 0;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
    }
    for(QString strl : fileNames) {
        QFileInfo fInfo(strl);
        totalSize += fInfo.size();
        if(totalSize > 20 * 1024 * 1024) {
            QMessageBox warnBox;
            warnBox.setText("File size limit is 20MB");
            warnBox.exec();
            return;
        }
    }
    for(QString strl : fileNames) {
        emit attachmentSignal(strl);
    }
}


AttachmentListWidget* ContentWidget::attachmentListWidget() {
    return _attachmentList;
}

#include "moc_ContentWidget.cpp"