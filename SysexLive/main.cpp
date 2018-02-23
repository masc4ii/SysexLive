#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if QT_VERSION >= 0x050000
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    MainWindow w;
    w.show();

    return a.exec();
}
