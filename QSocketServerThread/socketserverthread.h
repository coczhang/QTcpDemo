#ifndef SOCKETSERVERTHREAD_H
#define SOCKETSERVERTHREAD_H

#include <QObject>
#include <QThread>

#include <tcpsocketserver.h>

class SocketServerThread : public QObject
{
    Q_OBJECT
public:
    explicit SocketServerThread(QObject *parent = 0);
    ~SocketServerThread();

signals:
    void startConnectListen();

public slots:

public:
    void startSocketServer();

private:
    TcpSocketServer *m_pSocketServer;
    QThread *m_pSocketThread;
};

#endif
