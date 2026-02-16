/**
 * @file SerialReader.cpp
 * @brief Serial Reader Implementation
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#include "SerialReader.h"
#include <QSerialPortInfo>
#include <QRegularExpression>
#include <QDebug>

SerialReader::SerialReader(QObject *parent)
    : QObject(parent)
    , m_serialPort(nullptr)
    , m_reconnectTimer(nullptr)
    , m_isConnected(false)
{
    m_serialPort = new QSerialPort(this);
    
    // Connect signals
    connect(m_serialPort, &QSerialPort::readyRead,
            this, &SerialReader::onReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred,
            this, &SerialReader::onErrorOccurred);
    
    // Setup reconnect timer
    m_reconnectTimer = new QTimer(this);
    connect(m_reconnectTimer, &QTimer::timeout,
            this, &SerialReader::attemptReconnect);
    
    // Try to connect
    if (!connectToArduino()) {
        qWarning() << "Arduino not found. Will retry every 5 seconds...";
        m_reconnectTimer->start(5000);
    }
}

SerialReader::~SerialReader()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
}

bool SerialReader::isConnected() const
{
    return m_isConnected;
}

QString SerialReader::currentPort() const
{
    return m_serialPort->portName();
}

bool SerialReader::connectToArduino()
{
    QString portName = findArduinoPort();
    if (portName.isEmpty()) {
        return false;
    }
    
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    
    if (m_serialPort->open(QIODevice::ReadOnly)) {
        m_isConnected = true;
        m_reconnectTimer->stop();
        emit connectionStatusChanged(true);
        qDebug() << "Connected to Arduino on" << portName;
        return true;
    }
    
    qWarning() << "Failed to open" << portName << ":" << m_serialPort->errorString();
    return false;
}

QString SerialReader::findArduinoPort()
{
    // List all available serial ports
    const auto ports = QSerialPortInfo::availablePorts();
    
    for (const QSerialPortInfo &info : ports) {
        // Look for Arduino-like devices
        QString portName = info.portName();
        
        // Common Arduino port patterns
        if (portName.startsWith("ttyUSB") ||
            portName.startsWith("ttyACM") ||
            portName.startsWith("cu.usbserial") ||
            portName.startsWith("cu.usbmodem")) {
            
            qDebug() << "Found potential Arduino port:" << info.portName();
            qDebug() << "  Description:" << info.description();
            qDebug() << "  Manufacturer:" << info.manufacturer();
            
            return info.portName();
        }
    }
    
    return QString();
}

void SerialReader::onReadyRead()
{
    // Read available data
    QByteArray data = m_serialPort->readAll();
    m_buffer += QString::fromUtf8(data);
    
    // Process complete lines
    while (m_buffer.contains('\n')) {
        int newlinePos = m_buffer.indexOf('\n');
        QString line = m_buffer.left(newlinePos).trimmed();
        m_buffer = m_buffer.mid(newlinePos + 1);
        
        if (!line.isEmpty()) {
            parseData(line);
        }
    }
}

void SerialReader::parseData(const QString &line)
{
    // Expected format: "Pulses: 42 | Speed: 84.00 pulse/s | Time: 12.34 s"
    
    // Extract speed using regex
    QRegularExpression re(R"(Speed:\s+([\d.]+)\s+pulse/s)");
    QRegularExpressionMatch match = re.match(line);
    
    if (match.hasMatch()) {
        float pulsePerSec = match.captured(1).toFloat();
        emit speedDataReceived(pulsePerSec);
    }
}

void SerialReader::onErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError ||
        error == QSerialPort::DeviceNotFoundError) {
        
        qWarning() << "Serial port error:" << m_serialPort->errorString();
        
        if (m_serialPort->isOpen()) {
            m_serialPort->close();
        }
        
        m_isConnected = false;
        emit connectionStatusChanged(false);
        
        // Start reconnect attempts
        if (!m_reconnectTimer->isActive()) {
            m_reconnectTimer->start(5000);
        }
    }
}

void SerialReader::attemptReconnect()
{
    qDebug() << "Attempting to reconnect to Arduino...";
    connectToArduino();
}
