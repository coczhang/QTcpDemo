#ifndef SOCKETCLIENTTHREAD_H
#define SOCKETCLIENTTHREAD_H

#include <QObject>
#include <QThread>

#include "tcpsocketclient.h"

class SocketClientThread : public QObject
{
    Q_OBJECT
public:
    explicit SocketClientThread(QObject *parent = 0);
    ~SocketClientThread();

signals:
    void connectServer();

public slots:

public:
    void startSocketClient();

private:
    TcpSocketClient *m_pSocketClient;
    QThread *m_pSocketThread;
};

#endif
