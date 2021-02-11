#include "discordbotui.h"
#include <QApplication>

//Written by rev_eng_e near end of 2020
//(C) 2021
// :)

int main(int argc,char *argv[])
{
    auto a=make<QApplication>(argc,argv);
    discordbotui::botui=make<discordbotui>();
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    discordbotui::botui->show();
    int retval=a->exec();
    discordbotui::botui.reset();
    return retval;
}
