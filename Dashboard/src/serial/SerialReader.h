/**
 * @file SerialReader.h
 * @brief Arduino Serial Communication Reader
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>

/**
 * @class SerialReader
 * @brief Reads speed data from Arduino via serial port
 * 
 * Features:
 * - Auto-detect Arduino port
 * - Parse speed data (pulse/s)
 * - Auto-reconnection on disconnect
 */
class SerialReader : public QObject
{
    Q_OBJECT

public:
    explicit SerialReader(QObject *parent = nullptr);
    ~SerialReader();
    
    bool isConnected() const;
    QString currentPort() const;
    
signals:
    void speedDataReceived(float pulsePerSec);
    void connectionStatusChanged(bool connected);
    
private slots:
    void onReadyRead();
    void onErrorOccurred(QSerialPort::SerialPortError error);
    void attemptReconnect();
    
private:
    bool connectToArduino();
    QString findArduinoPort();
    void parseData(const QString &line);
    
    QSerialPort *m_serialPort;
    QTimer *m_reconnectTimer;
    QString m_buffer;
    bool m_isConnected;
};

#endif // SERIALREADER_H
