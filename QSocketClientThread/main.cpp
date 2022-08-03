#include <QCoreApplication>

#include "socketclientthread.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SocketClientThread clientThread;
    clientThread.startSocketClient();

    return a.exec();
}
