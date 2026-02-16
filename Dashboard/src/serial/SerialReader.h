/**
 * @file SerialReader.h
 * @brief Arduino Serial Communication Reader
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <QObject>
#include <QTimer>
#include <QSocketNotifier>

/**
 * @class SerialReader
 * @brief Reads speed data from can0 (SocketCAN)
 * 
 * Features:
 * - Read raw CAN frames from can0
 * - Parse speed data from CAN ID 0x123
 * - Auto-reconnection on disconnect
 */
class SerialReader : public QObject
{
    Q_OBJECT

public:
    explicit SerialReader(QObject *parent = nullptr);
    ~SerialReader();
    
    bool isConnected() const;
    QString currentPort() const;  // kept for compatibility, returns "can0" when connected
    
signals:
    void speedDataReceived(float pulsePerSec);
    void connectionStatusChanged(bool connected);
    
private slots:
    void onCanReadyRead();
    void attemptReconnect();
    
private:
    bool connectToCan();
    void closeCan();
    
    static constexpr quint32 SPEED_CAN_ID = 0x123;

    int m_canSocket;
    QSocketNotifier *m_canNotifier;
    QTimer *m_reconnectTimer;
    bool m_isConnected;
};

#endif // SERIALREADER_H
