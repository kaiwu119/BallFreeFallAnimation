#include "widget.h"
#include <QApplication>

#include "wind.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //    Widget w;
    //    w.show();



    wind w;

    w.show();
    return a.exec();
}
