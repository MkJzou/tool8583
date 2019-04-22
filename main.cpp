#include "widget.h"

#include <exception>

#include <QApplication>
#include <QDebug>

#include "config.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    try
    {
        config::config.read();
    }
    catch (std::exception &err)
    {
        qDebug() << err.what();
    }


    return a.exec();
}
