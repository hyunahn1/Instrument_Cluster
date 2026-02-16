/**
 * @file MainWindow.h
 * @brief Main Dashboard Window
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QProcess>
#include <QString>

// Forward declarations
class SpeedometerWidget;
class RpmGauge;
class BatteryWidget;
class SerialReader;
class DataProcessor;

/**
 * @class MainWindow
 * @brief Main window containing the entire dashboard
 * 
 * Layout: 1200×400 pixels
 * - Left Panel (300px): RPM gauge + auxiliary battery
 * - Center Panel (600px): Main speedometer
 * - Right Panel (300px): Direction, battery, time
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSpeedDataReceived(float pulsePerSec);
    void onPythonDataReceived();
    void onResetButtonClicked();
    void updateElapsedTime();
    
private:
    void setupUI();
    void setupConnections();
    void setupPythonBridge();
    void applyStyles();
    
    // Widgets
    SpeedometerWidget *m_speedometer;
    RpmGauge *m_rpmGauge;
    BatteryWidget *m_batteryWidget;
    
    // Info labels
    QLabel *m_directionLabel;
    QLabel *m_timeLabel;
    QLabel *m_maxSpeedLabel;
    QPushButton *m_resetButton;
    
    // Communication
    SerialReader *m_serialReader;
    QProcess *m_pythonProcess;
    DataProcessor *m_dataProcessor;
    QString m_pythonStdoutBuffer;
    
    // Statistics
    float m_maxSpeed;
    QTimer *m_elapsedTimer;
    qint64 m_startTime;
    
    // Constants
    static constexpr int WINDOW_WIDTH = 1200;
    static constexpr int WINDOW_HEIGHT = 400;
    static constexpr int LEFT_PANEL_WIDTH = 300;
    static constexpr int CENTER_PANEL_WIDTH = 600;
    static constexpr int RIGHT_PANEL_WIDTH = 300;
};

#endif // MAINWINDOW_H
