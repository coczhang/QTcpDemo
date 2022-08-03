#include "socketclientthread.h"

SocketClientThread::SocketClientThread(QObject *parent) : QObject(parent)
{
    m_pSocketClient = new TcpSocketClient;
    m_pSocketThread = new QThread(this);
    m_pSocketClient->moveToThread(m_pSocketThread);

    connect(this, &SocketClientThread::connectServer, m_pSocketClient, &TcpSocketClient::startConnectServer);
}

SocketClientThread::~SocketClientThread()
{
    if (m_pSocketClient != NULL) {
        delete m_pSocketClient;
        m_pSocketClient = NULL;
    }

    if (m_pSocketThread->isRunning()) {
        m_pSocketThread->wait();
    }
}

void SocketClientThread::startSocketClient()
{
    m_pSocketThread->start();
    emit connectServer();
}

