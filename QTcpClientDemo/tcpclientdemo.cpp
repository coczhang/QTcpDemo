#include "tcpclientdemo.h"

#include <QDateTime>
#include <QHostAddress>
#include <QDebug>

TcpClientDemo::TcpClientDemo(QWidget *parent)
    : QWidget(parent)
{
    initClientLayout();
    initClientConnect();
}

TcpClientDemo::~TcpClientDemo()
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
}

void TcpClientDemo::initClientLayout()
{
    this->setWindowTitle("客户端");
    this->setFixedSize(this->width(), this->height());

    m_pLabelIp = new QLabel("服务器IP：", this);
    m_pLabelIp->setGeometry(20, 12, 80, 30);

    m_pLineIp = new QLineEdit("127.0.0.1", this);
    m_pLineIp->setGeometry(120, 13, 400, 30);
    m_pLineIp->setDisabled(true);

    m_pLabelPort = new QLabel("服务器端口：", this);
    m_pLabelPort->setGeometry(20, 50, 82 ,30);

    m_pLinePort = new QLineEdit("8888", this);
    m_pLinePort->setGeometry(120, 50, 400, 30);
    m_pLinePort->setDisabled(true);

    m_pButtonConnect = new QPushButton("Connect", this);
    m_pButtonConnect->setGeometry(540, 35, 80, 30);

    m_pTextRead = new QTextEdit(this);
    m_pTextRead->setGeometry(20, 100, 600, 150);
    m_pTextRead->setReadOnly(true);

    m_pTextWrite = new QTextEdit(this);
    m_pTextWrite->setGeometry(20, 260, 600, 150);

    m_pButtonSend = new QPushButton("Send", this);
    m_pButtonSend->setGeometry(10, this->height() - 40, 80, 30);

    m_pButtonClose = new QPushButton("Close", this);
    m_pButtonClose->setGeometry(this->width()- 90, this->height() - 40, 80, 30);
}

void TcpClientDemo::initClientConnect()
{
    m_bConnect = false;

    m_pTcpSocket = new QTcpSocket(this);

    connect(m_pTcpSocket, &QTcpSocket::connected, this, &TcpClientDemo::socketConnected);
    connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &TcpClientDemo::socketReadyRead);
    connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &TcpClientDemo::socketDisconnected);

    connect(m_pButtonConnect, &QPushButton::clicked, this, &TcpClientDemo::btnConnectClicked);
    connect(m_pButtonSend, &QPushButton::clicked, this, &TcpClientDemo::btnSendClicked);
    connect(m_pButtonClose, &QPushButton::clicked, this, &TcpClientDemo::btnCloseClicked);
}

void TcpClientDemo::btnConnectClicked()
{
    if(m_bConnect)
    {
        QString str = QString("已连接服务端......%1").arg(getCurrentTime());
        m_pTextRead->append(str);

        return;
    }

    //获取服务器IP和端口
    QString strIP = m_pLineIp->text();
    quint16 nPort = (m_pLinePort->text()).toUInt();

    //主动和服务器建立连接
    //m_pTcpSocket->abort();
    m_pTcpSocket->connectToHost(QHostAddress(strIP), nPort);
}

void TcpClientDemo::btnSendClicked()
{
    if(!m_bConnect)
    {
        QString str = QString("请先连接服务端......%1").arg(getCurrentTime());
        m_pTextRead->append(str);

        return;
    }

    //获取编辑框内容
    QString strText = m_pTextWrite->toPlainText();
    //发送数据
    sendDataMsg(strText);
}

void TcpClientDemo::sendDataMsg(QString buffer)
{
    PduHeader pdu_header;
    pdu_header.length = sizeof(pdu_header) + buffer.size() - sizeof(pdu_header.length);
    pdu_header.payload_type = PDU_FRAME_DATA;

    QByteArray msg;
    msg.append((char *)&pdu_header, sizeof(pdu_header));
    msg.append(buffer.toLatin1());

    if (!socketWriteData(msg.data(), msg.size())) {
        qDebug() << "send data msg error";
    }
}

void TcpClientDemo::btnCloseClicked()
{
    if (!m_bConnect)
    {
        QString str = QString("服务端未连接......%1").arg(getCurrentTime());
        m_pTextRead->append(str);

        return;
    }

    if(m_pTcpSocket->isOpen())
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();

        m_bConnect = false;
        QString str = QString("客户端断开连接......%1").arg(getCurrentTime());
        m_pTextRead->append(str);
    }
}

void TcpClientDemo::socketConnected()
{
    m_bConnect = true;
    QString str = QString("成功和服务端建立连接......%1").arg(getCurrentTime());
    m_pTextRead->append(str);
}

void TcpClientDemo::socketDisconnected()
{
    if(m_pTcpSocket->isOpen())
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();

        m_bConnect = false;
        QString str = QString("客户端断开连接......%1").arg(getCurrentTime());
        m_pTextRead->append(str);
    }
}

void TcpClientDemo::socketReadyRead()
{
    QByteArray buffer = m_pTcpSocket->readAll();
    if (buffer.isEmpty()) {
        qDebug() << "socket ready read no data to read";
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

        if (pdu_header->payload_type == PDU_READY) {
            QString strInfo = QString("%1 %2").arg(QString("PDU_READY")).arg(getCurrentTime());
            m_pTextRead->append(strInfo);
            send_req_msg();
        }

        if (pdu_header->payload_type == PDU_FRAME_DATA) {
             QByteArray buffer(pdu_header->data, pdu_size - sizeof(PduHeader));
             QString strInfo = QString("%1 %2").arg(QString(buffer)).arg(getCurrentTime());
             m_pTextRead->append(strInfo);
             send_req_msg();
        }
        m_socket_buffer.remove(0, pdu_size);
    }
}

void TcpClientDemo::send_req_msg()
{
    PduHeader pdu_header;
    pdu_header.length = sizeof(pdu_header) - sizeof(pdu_header.length);
    pdu_header.payload_type = PDU_FRAME_REQ;

    if (!socketWriteData((char *)&pdu_header, sizeof(pdu_header))) {
        qDebug() << "send req msg error";
        return;
    }
}

bool TcpClientDemo::socketWriteData(const char *buffer, int length)
{
    int send_size = 0;
    while (true) {
        if (m_pTcpSocket->state() != QAbstractSocket::ConnectedState) {
            qDebug() << "socket write data error, socket state:" << m_pTcpSocket->state();
            return false;
        }

        int write_size = m_pTcpSocket->write(buffer + send_size, length - send_size);
        if (write_size == -1) {
            qDebug() << "socket write data error, write failed";
            return false;
        }
        send_size += write_size;
        if (send_size < length) {
            m_pTcpSocket->waitForBytesWritten(1000);
            qDebug() << "socket write data incompletely, maybe has problem";
        } else {
            break;
        }
    }

    return true;
}

QString TcpClientDemo::getCurrentTime()
{
    QTime time = QTime::currentTime();
    return (QString("[当前时间:%1:%2:%3]").arg(time.hour()).arg(time.minute()).arg(time.second()));
}
