#include "tcpserverdemo.h"

#include <QDateTime>
#include <QDebug>

TcpServerDemo::TcpServerDemo(QWidget *parent)
    : QWidget(parent)
{
    initClientLayout();
    initClientConnect();
}

TcpServerDemo::~TcpServerDemo()
{
    if(m_pTcpSocket->isOpen())
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();
    }

    if (m_pTcpSocket != NULL)
    {
        delete m_pTcpSocket;
        m_pTcpSocket = NULL;
    }

    //主动和客户端端断开连接
    if(m_pTcpServer != NULL)
    {
        delete m_pTcpSocket;
        m_pTcpSocket = NULL;
    }
}

void TcpServerDemo::initClientLayout()
{
    this->setWindowTitle("服务端");
    this->setFixedSize(this->width(), this->height());

    m_pTextRead = new QTextEdit(this);
    m_pTextRead->setGeometry(10, 5, this->width() - 20, this->height() / 2 - 40);
    m_pTextRead->setReadOnly(true);

    m_pTextWrite = new QTextEdit(this);
    m_pTextWrite->setGeometry(10, this->height() / 2 - 30, this->width() - 20, this->height() / 2 - 40);

    m_pButtonSend = new QPushButton("Send", this);
    m_pButtonSend->setGeometry(10, this->height() - 50, 80, 30);

    m_pButtonClose = new QPushButton("Close", this);
    m_pButtonClose->setGeometry(this->width() - 90, this->height() - 50, 80, 30);
}

void TcpServerDemo::initClientConnect()
{
    //监听套接字
    m_pTcpServer = new QTcpServer(this); //指定父对象，让Qt自动回收内存控件
    m_pTcpServer->listen(QHostAddress::Any, CONNECT_PORT);

    connect(m_pTcpServer, &QTcpServer::newConnection, this, &TcpServerDemo::acceptSocketConnect);

    connect(m_pButtonSend, &QPushButton::clicked, this, &TcpServerDemo::btnSendClicked);
    connect(m_pButtonClose, &QPushButton::clicked, this, &TcpServerDemo::btnCloseClicked);
}

QString TcpServerDemo::getCurrentTime()
{
    QTime time = QTime::currentTime();
    return (QString("[当前时间:%1:%2:%3]").arg(time.hour()).arg(time.minute()).arg(time.second()));
}

void TcpServerDemo::btnSendClicked()
{
    if (NULL == m_pTcpSocket)
    {
        QString str = QString("没有客户端与服务端连接......%1").arg(getCurrentTime());
        m_pTextRead->append(str);
        return;
    }
    QString strText = m_pTextWrite->toPlainText();
    //给对方发送数据，使用套接字tcpSocket
    m_pTcpSocket->write(strText.toUtf8().data());
}

void TcpServerDemo::btnCloseClicked()
{
    if(NULL == m_pTcpSocket)
    {
        QString str = QString("没有客户端与服务端连接......%1").arg(getCurrentTime());
        m_pTextRead->append(str);
        return;
    }

    //主动和客户端端断开连接
    if(m_pTcpSocket->isOpen())
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();
    }

    if (m_pTcpSocket != NULL)
    {
        delete m_pTcpSocket;
        m_pTcpSocket = NULL;
    }
}

void TcpServerDemo::acceptSocketConnect()
{
    QString strDisplay;
    //取出建立好链接的套接字
    m_pTcpSocket = m_pTcpServer->nextPendingConnection();
    if (NULL == m_pTcpSocket)
    {
        strDisplay = QString("客户端连接失败%3").arg(getCurrentTime());
        m_pTextRead->append(strDisplay);
        return;
    }

    //获取对方的IP和端口
    QString strIP = m_pTcpSocket->peerAddress().toString();
    quint16 nPort = m_pTcpSocket->peerPort();
    strDisplay = QString("[%1:%2]:连接成功%3").arg(strIP).arg(nPort).arg(getCurrentTime());
    m_pTextRead->append(strDisplay);
    sendReadyMsg(m_pTcpSocket);

    connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &TcpServerDemo::socketReadyRead);
    connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &TcpServerDemo::socketDisconnected);
}

void TcpServerDemo::sendReadyMsg(QTcpSocket *tcpSocket)
{
    PduHeader pdu_header;
    pdu_header.length = sizeof(pdu_header) - sizeof(pdu_header.length);
    pdu_header.payload_type = PDU_READY;

    if (!socketWriteData(tcpSocket, (char *)&pdu_header, sizeof(pdu_header))) {
        qDebug() << "send ready msg error";
        return;
    }
}

bool TcpServerDemo::socketWriteData(QTcpSocket *tcpSocket, const char *buffer, int length)
{
    int send_size = 0;
    while (true) {
        if (tcpSocket->state() != QAbstractSocket::ConnectedState) {
            qDebug() << "socket_write_data error, socket state:" << tcpSocket->state();
            return false;
        }

        int write_size = tcpSocket->write(buffer + send_size, length - send_size);
        if (write_size == -1) {
            qDebug() << "socket write data error, write failed";
            return false;
        }
        send_size += write_size;
        if (send_size < length) {
            tcpSocket->waitForBytesWritten(1000);
            qDebug() << "socket write data incompletely, maybe has problem";
        } else {
            break;
        }
    }

    return true;
}

void TcpServerDemo::socketDisconnected()
{
    //主动和客户端端断开连接
    if(m_pTcpSocket->isOpen())
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();

        QString str =QString("客户端断开连接......%1").arg(getCurrentTime());
        m_pTextRead->append(str);
    }

    if (m_pTcpSocket != NULL)
    {
        delete m_pTcpSocket;
        m_pTcpSocket = NULL;
    }

}

void TcpServerDemo::socketReadyRead()
{
    if (m_pTcpSocket == NULL) {
        qDebug() << "m_pTcpSocket NULL";
        return;
    }

    QByteArray buffer = m_pTcpSocket->readAll();
    if (buffer.isEmpty()) {
        qDebug() << "socket ready read fail, no data to read";
        return;
    }

    m_socket_buffer.append(buffer);

    while (true) {
        int buffer_size = m_socket_buffer.size();
        if (buffer_size < sizeof(PduHeader)) {
            break;
        }

        PduHeader *pdu_header = (PduHeader *)m_socket_buffer.data();
        int pdu_size = pdu_header->length + sizeof(pdu_header->length);
        if (buffer_size < pdu_size) {
            break;
        }

        if (pdu_header->payload_type == PDU_FRAME_REQ) {
            QByteArray buffer(pdu_header->data, pdu_size - sizeof(PduHeader));
            if (buffer.size() > 0) {
                qDebug() << "buffer:" << buffer;
            }
            sendDataMsg(m_pTcpSocket);
            QString str = QString("%1 %2").arg(QString("PDU_FRAME_REQ")).arg(getCurrentTime());
            m_pTextRead->append(str);
        }
        m_socket_buffer.remove(0, pdu_size);
    }
}

void TcpServerDemo::sendDataMsg(QTcpSocket *tcp_socket)
{
    QString buffer = "coczhang|127.0.0.1|8888";

    PduHeader pdu_header;
    pdu_header.length = sizeof(pdu_header) + buffer.size() - sizeof(pdu_header.length);
    pdu_header.payload_type = PDU_FRAME_DATA;

    QByteArray msg;
    msg.append((char *)&pdu_header, sizeof(pdu_header));
    msg.append(buffer.toLatin1());

    if (!socketWriteData(tcp_socket, msg.data(), msg.size())) {
        qDebug() << "send data msg error";
    }
}
