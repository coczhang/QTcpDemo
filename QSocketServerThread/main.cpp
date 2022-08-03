#include <QCoreApplication>

#include <socketserverthread.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SocketServerThread serverThread;
    serverThread.startSocketServer();

    return a.exec();
}
