/**
 * @file SerialReader.cpp
 * @brief Serial Reader Implementation
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#include "SerialReader.h"
#include <QDebug>
#include <cstring>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

SerialReader::SerialReader(QObject *parent)
    : QObject(parent)
    , m_canSocket(-1)
    , m_canNotifier(nullptr)
    , m_reconnectTimer(nullptr)
    , m_isConnected(false)
{
    // Setup reconnect timer
    m_reconnectTimer = new QTimer(this);
    connect(m_reconnectTimer, &QTimer::timeout,
            this, &SerialReader::attemptReconnect);
    
    // Try to connect can0
    if (!connectToCan()) {
        qWarning() << "can0 not available. Will retry every 2 seconds...";
        m_reconnectTimer->start(2000);
    }
}

SerialReader::~SerialReader()
{
    closeCan();
}

bool SerialReader::isConnected() const
{
    return m_isConnected;
}

QString SerialReader::currentPort() const
{
    return m_isConnected ? QStringLiteral("can0") : QString();
}

bool SerialReader::connectToCan()
{
    closeCan();

    m_canSocket = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (m_canSocket < 0) {
        qWarning() << "Failed to create CAN socket";
        return false;
    }

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, "can0", IFNAMSIZ - 1);
    if (::ioctl(m_canSocket, SIOCGIFINDEX, &ifr) < 0) {
        qWarning() << "Failed to resolve can0 interface index";
        closeCan();
        return false;
    }

    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (::bind(m_canSocket, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
        qWarning() << "Failed to bind CAN socket to can0";
        closeCan();
        return false;
    }

    m_canNotifier = new QSocketNotifier(m_canSocket, QSocketNotifier::Read, this);
    connect(m_canNotifier, &QSocketNotifier::activated, this, &SerialReader::onCanReadyRead);

    m_isConnected = true;
    m_reconnectTimer->stop();
    emit connectionStatusChanged(true);
    qDebug() << "Connected to can0";
    return true;
}

void SerialReader::closeCan()
{
    if (m_canNotifier) {
        m_canNotifier->setEnabled(false);
        m_canNotifier->deleteLater();
        m_canNotifier = nullptr;
    }
    if (m_canSocket >= 0) {
        ::close(m_canSocket);
        m_canSocket = -1;
    }
    m_isConnected = false;
}

void SerialReader::onCanReadyRead()
{
    if (m_canSocket < 0) {
        return;
    }

    struct can_frame frame;
    const ssize_t n = ::read(m_canSocket, &frame, sizeof(frame));
    if (n < static_cast<ssize_t>(sizeof(struct can_frame))) {
        return;
    }

    const quint32 canId = static_cast<quint32>(frame.can_id & CAN_EFF_MASK);
    if (canId != SPEED_CAN_ID || frame.can_dlc < 1) {
        return;
    }

    // candump 기준: can0 123 [8] 11 00 00 ...
    // 첫 바이트를 km/h 속도로 사용
    const float speedKmh = static_cast<float>(frame.data[0]);
    emit speedDataReceived(speedKmh);
}

void SerialReader::attemptReconnect()
{
    qDebug() << "Attempting to reconnect to can0...";
    if (connectToCan()) {
        qDebug() << "Reconnected to can0";
    }
}
