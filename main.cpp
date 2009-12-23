#include <QtGui/QApplication>
#include "whee.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    whee foo(argc, argv);
    foo.show();
    return app.exec();
}
