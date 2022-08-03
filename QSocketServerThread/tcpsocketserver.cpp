#include "tcpsocketserver.h"

#include <QDebug>

TcpSocketServer::TcpSocketServer():m_pTcpSocket(NULL),m_nCount(0)
{

}

TcpSocketServer::~TcpSocketServer()
{

}

void TcpSocketServer::socketListen()
{
    if (this->isListening()) {
        qDebug() << "sender server is listening";

        return;
    }

    bool ret = this->listen(QHostAddress::Any, 8888);
    if (ret) {
        qDebug() << "sender server start success";
    } else {
        qDebug() << "sender start fail";
    }
}

void TcpSocketServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* tcpSocket = new QTcpSocket();
    if ( tcpSocket != NULL) {
        if (! tcpSocket->setSocketDescriptor(socketDescriptor)) {
            qDebug() << "socket error code:" <<  tcpSocket->error();

            return;
        }

        if (NULL == m_pTcpSocket) {
            m_socketBuffer.clear();
            m_pTcpSocket = tcpSocket;
            connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &TcpSocketServer::socketReadyRead);
            connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &TcpSocketServer::socketDisconnected);
            sendReadyMsg(tcpSocket);
        } else {
            sendBusyMsg(tcpSocket);
        }
    }
}

void TcpSocketServer::sendBusyMsg(QTcpSocket *tcpSocket)
{
    PduHeader pdu_header;
    pdu_header.length = sizeof(pdu_header) - sizeof(pdu_header.length);
    pdu_header.payload_type = PDU_BUSY;

    if (!socketWriteData(tcpSocket, (char *)&pdu_header, sizeof(pdu_header))) {
        qDebug() << "send busy msg error";
        return;
    }

    qDebug() << "send busy msg";
}

void TcpSocketServer::socketReadyRead()
{
    if (m_pTcpSocket == NULL) {
        qDebug() << "tcp socket is NULL";
        return;
    }

    QByteArray buffer = m_pTcpSocket->readAll();
    if (buffer.isEmpty()) {
        qDebug() << "socket ready read fail, no data to read";
        return;
    }

    m_socketBuffer.append(buffer);

    while (true) {
        int buffer_size = m_socketBuffer.size();
        if (buffer_size < sizeof(PduHeader)) {
            break;
        }

        PduHeader *pdu_header = (PduHeader *)m_socketBuffer.data();
        int pdu_size = pdu_header->length + sizeof(pdu_header->length);
        if (buffer_size < pdu_size) {
            break;
        }

        if (pdu_header->payload_type == PDU_FRAME_REQ) {
            qDebug() << "PDU_FRAME_REQ";
            sendDataMsg(m_pTcpSocket);
        }

        m_socketBuffer.remove(0, pdu_size);
    }
}

void TcpSocketServer::sendDataMsg(QTcpSocket *tcpSocket)
{
    QByteArray buffer = "coczhang";

    QString info = QString("%1 %2").arg(QString(buffer)).arg(m_nCount++);
    PduHeader pdu_header;
    pdu_header.length = sizeof(pdu_header) + info.size() - sizeof(pdu_header.length);
    pdu_header.payload_type = PDU_FRAME_DATA;

    QByteArray msg;
    msg.append((char *)&pdu_header, sizeof(pdu_header));
    msg.append(info);

    if (!socketWriteData(tcpSocket, msg.data(), msg.size())) {
        qDebug() << "send data msg error";
    }
}

void TcpSocketServer::socketDisconnected()
{
    if (m_pTcpSocket->isOpen()) {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();
    }

    if (m_pTcpSocket != NULL) {
        delete m_pTcpSocket;
        m_pTcpSocket = NULL;
    }
}

void TcpSocketServer::sendReadyMsg(QTcpSocket *tcpSocket)
{
    PduHeader pdu_header;
    pdu_header.length = sizeof(pdu_header) - sizeof(pdu_header.length);
    pdu_header.payload_type = PDU_READY;

    if (!socketWriteData(tcpSocket, (char *)&pdu_header, sizeof(pdu_header))) {
        qDebug() << "send ready msg error";
        return;
    }
}

bool TcpSocketServer::socketWriteData(QTcpSocket *tcpSocket, const char *buffer, int length)
{
    int sendSize = 0;
    while (true) {
        if (tcpSocket->state() != QAbstractSocket::ConnectedState) {
            qDebug() << "socket write data error, socket state:" << tcpSocket->state();
            return false;
        }

        int writeSize = tcpSocket->write(buffer + sendSize, length - sendSize);
        if (writeSize == -1) {
            qDebug() << "socket write data error, write failed";
            return false;
        }
        sendSize += writeSize;
        if (sendSize < length) {
            tcpSocket->waitForBytesWritten(1000);
            qDebug() << "socket_write_data incompletely, maybe has problem";
        } else {
            break;
        }
    }

    return true;
}


