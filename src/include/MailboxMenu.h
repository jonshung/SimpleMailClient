#ifndef _MAILBOX_MENU_H_
#define _MAILBOX_MENU_H_

#include <QtWidgets/QWidget>

class MailboxMenu : public QWidget {
    Q_OBJECT
public:
    MailboxMenu(QWidget* = nullptr);
    virtual ~MailboxMenu() {}
};
#endif