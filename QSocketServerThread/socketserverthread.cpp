#include "socketserverthread.h"

SocketServerThread::SocketServerThread(QObject *parent) : QObject(parent)
{
    m_pSocketServer = new TcpSocketServer;
    m_pSocketThread = new QThread(this);
    m_pSocketServer->moveToThread(m_pSocketThread);

    connect(this, &SocketServerThread::startConnectListen, m_pSocketServer, &TcpSocketServer::socketListen);
}

SocketServerThread::~SocketServerThread()
{

}

void SocketServerThread::startSocketServer()
{
    m_pSocketThread->start();
    emit startConnectListen();
}

