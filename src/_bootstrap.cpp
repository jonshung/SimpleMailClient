#include <QtWidgets/QApplication>

#include "MainWindow.h"
#include "test_unit.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("KietHung_PhuongUyen_DuyBao");
    QCoreApplication::setApplicationName("Mail Client");
    QCoreApplication::setApplicationVersion("1.0");

    MainWindow _mainWin;
    _mainWin.show();
    //evalTest();
    //exit(EXIT_SUCCESS);

    return app.exec();
}