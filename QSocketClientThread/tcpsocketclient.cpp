#include "tcpsocketclient.h"
#include <QDebug>

TcpSocketClient::TcpSocketClient(QObject *parent) : QObject(parent)
{

}

TcpSocketClient::~TcpSocketClient()
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

void TcpSocketClient::socketConnected()
{
    qDebug() << "socket connected success";
}

void TcpSocketClient::socketDisconnected()
{
    if (m_pTcpSocket->isOpen()) {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();
    }

    if (m_pTcpSocket != NULL) {
        delete m_pTcpSocket;
        m_pTcpSocket = NULL;
    }

    qDebug() << "socket_disconnected";
}

void TcpSocketClient::socketReadyRead()
{
    QByteArray buffer = m_pTcpSocket->readAll();
    if (buffer.isEmpty()) {
        qDebug() << "socket ready read no data to read";
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

        if (pdu_header->payload_type == PDU_READY) {
            qDebug() << "PDU_READY";
            sendReqMsg();
        }

        if (pdu_header->payload_type == PDU_FRAME_DATA) {
            QByteArray buffer(pdu_header->data, pdu_size - sizeof(PduHeader));
            qDebug() << "PDU_FRAME_DATA" << ":" << buffer;

            sendReqMsg();
        }
        m_socketBuffer.remove(0, pdu_size);
    }
}

void TcpSocketClient::sendReqMsg()
{
    PduHeader pdu_header;
    pdu_header.length = sizeof(pdu_header) - sizeof(pdu_header.length);
    pdu_header.payload_type = PDU_FRAME_REQ;

    if (!socketWriteData((char *)&pdu_header, sizeof(pdu_header))) {
        qDebug() << "send req msg error";
        return;
    }
}

bool TcpSocketClient::socketWriteData(const char *buffer, int length)
{
    int sendSize = 0;
    while (true) {
        if (m_pTcpSocket->state() != QAbstractSocket::ConnectedState) {
            qDebug() << "socket write data error, socket state:" << m_pTcpSocket->state();
            return false;
        }

        int writeSize = m_pTcpSocket->write(buffer + sendSize, length - sendSize);
        if (writeSize == -1) {
            qDebug() << "socket write data error, write failed";
            return false;
        }
        sendSize += writeSize;
        if (sendSize < length) {
            m_pTcpSocket->waitForBytesWritten(1000);
            qDebug() << "socket write data incompletely, maybe has problem";
        } else {
            break;
        }
    }

    return true;
}

void TcpSocketClient::initSocketConnect()
{
    m_pTcpSocket = new QTcpSocket;

    connect(m_pTcpSocket, &QTcpSocket::connected, this, &TcpSocketClient::socketConnected);
    connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &TcpSocketClient::socketReadyRead);
    connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &TcpSocketClient::socketDisconnected);
}

void TcpSocketClient::startConnectServer()
{
    initSocketConnect();

    m_pTcpSocket->connectToHost("127.0.0.1", 8888);
    m_pTcpSocket->waitForConnected();
}

