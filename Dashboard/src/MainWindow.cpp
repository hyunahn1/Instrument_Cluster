/**
 * @file MainWindow.cpp
 * @brief Main Dashboard Window Implementation
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#include "MainWindow.h"
#include "SpeedometerWidget.h"
#include "RpmGauge.h"
#include "BatteryWidget.h"
#include "SerialReader.h"
#include "DataProcessor.h"

#include <QDateTime>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_speedometer(nullptr)
    , m_rpmGauge(nullptr)
    , m_batteryWidget(nullptr)
    , m_directionLabel(nullptr)
    , m_timeLabel(nullptr)
    , m_maxSpeedLabel(nullptr)
    , m_resetButton(nullptr)
    , m_serialReader(nullptr)
    , m_pythonProcess(nullptr)
    , m_dataProcessor(nullptr)
    , m_maxSpeed(0.0f)
    , m_elapsedTimer(nullptr)
    , m_startTime(0)
{
    // Set fixed window size
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setWindowTitle("PiRacer Dashboard");
    
    // Initialize components
    m_dataProcessor = new DataProcessor(this);
    m_serialReader = new SerialReader(this);
    
    // Setup UI
    setupUI();
    setupConnections();
    setupPythonBridge();
    applyStyles();
    
    // Start elapsed timer
    m_startTime = QDateTime::currentMSecsSinceEpoch();
    m_elapsedTimer = new QTimer(this);
    connect(m_elapsedTimer, &QTimer::timeout, this, &MainWindow::updateElapsedTime);
    m_elapsedTimer->start(1000);  // Update every second
    
    qDebug() << "Dashboard initialized successfully";
}

MainWindow::~MainWindow()
{
    // Cleanup Python process
    if (m_pythonProcess) {
        m_pythonProcess->terminate();
        m_pythonProcess->waitForFinished(3000);
    }
}

void MainWindow::setupUI()
{
    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Main horizontal layout
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // === LEFT PANEL ===
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(LEFT_PANEL_WIDTH);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setAlignment(Qt::AlignCenter);
    
    // RPM Gauge (placeholder)
    m_rpmGauge = new RpmGauge(this);
    m_rpmGauge->setFixedSize(250, 200);
    leftLayout->addWidget(m_rpmGauge);
    
    leftLayout->addStretch();
    
    // === CENTER PANEL ===
    QWidget *centerPanel = new QWidget();
    centerPanel->setFixedWidth(CENTER_PANEL_WIDTH);
    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setAlignment(Qt::AlignCenter);
    
    // Main Speedometer
    m_speedometer = new SpeedometerWidget(this);
    m_speedometer->setFixedSize(500, 350);
    centerLayout->addWidget(m_speedometer);
    
    // === RIGHT PANEL ===
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(RIGHT_PANEL_WIDTH);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    rightLayout->setSpacing(20);
    
    // Direction Label
    m_directionLabel = new QLabel("F");
    m_directionLabel->setAlignment(Qt::AlignCenter);
    m_directionLabel->setStyleSheet(
        "QLabel {"
        "   font-family: 'Roboto';"
        "   font-size: 48pt;"
        "   font-weight: bold;"
        "   color: #00FF88;"
        "}"
    );
    rightLayout->addWidget(m_directionLabel);
    
    QLabel *dirText = new QLabel("Forward");
    dirText->setAlignment(Qt::AlignCenter);
    dirText->setStyleSheet("color: #7A8A9E; font-size: 14pt;");
    rightLayout->addWidget(dirText);
    
    rightLayout->addSpacing(30);
    
    // Battery Widget
    m_batteryWidget = new BatteryWidget(this);
    m_batteryWidget->setFixedSize(250, 120);
    rightLayout->addWidget(m_batteryWidget);
    
    rightLayout->addSpacing(30);
    
    // Time Label
    m_timeLabel = new QLabel("00:00:00");
    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->setStyleSheet(
        "QLabel {"
        "   font-family: 'Roboto Mono';"
        "   font-size: 28pt;"
        "   color: #E8F0FF;"
        "}"
    );
    rightLayout->addWidget(m_timeLabel);
    
    rightLayout->addStretch();
    
    // Add panels to main layout
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(centerPanel);
    mainLayout->addWidget(rightPanel);
    
    // === BOTTOM INFO BAR ===
    QWidget *bottomBar = new QWidget(this);
    bottomBar->setGeometry(0, WINDOW_HEIGHT - 50, WINDOW_WIDTH, 50);
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomBar);
    
    // Max Speed Label
    m_maxSpeedLabel = new QLabel("Max Speed: 0.0 km/h");
    m_maxSpeedLabel->setStyleSheet(
        "QLabel {"
        "   font-family: 'Roboto';"
        "   font-size: 20pt;"
        "   color: #FFD700;"
        "}"
    );
    bottomLayout->addWidget(m_maxSpeedLabel);
    
    bottomLayout->addStretch();
    
    // Reset Button
    m_resetButton = new QPushButton("RESET");
    m_resetButton->setFixedSize(130, 40);
    m_resetButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #333333AA;"
        "   border: 1px solid white;"
        "   border-radius: 5px;"
        "   color: #E8F0FF;"
        "   font-family: 'Roboto';"
        "   font-size: 18pt;"
        "   font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "   background-color: #555555;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #0088FF;"
        "}"
    );
    bottomLayout->addWidget(m_resetButton);
}

void MainWindow::setupConnections()
{
    // Serial data connection
    connect(m_serialReader, &SerialReader::speedDataReceived,
            this, &MainWindow::onSpeedDataReceived);
    
    // Reset button
    connect(m_resetButton, &QPushButton::clicked,
            this, &MainWindow::onResetButtonClicked);
}

void MainWindow::setupPythonBridge()
{
    m_pythonProcess = new QProcess(this);
    
    // Connect signals
    connect(m_pythonProcess, &QProcess::readyReadStandardOutput,
            this, &MainWindow::onPythonDataReceived);
    
    connect(m_pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
                qWarning() << "Python bridge exited with code:" << exitCode;
            });
    
    // Start Python bridge
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidateScripts = {
        QDir::cleanPath(appDir + "/python/piracer_bridge.py"),
        QDir::cleanPath(appDir + "/../python/piracer_bridge.py"),
        QDir::cleanPath(appDir + "/../../python/piracer_bridge.py"),
        QDir::cleanPath(QDir::currentPath() + "/python/piracer_bridge.py")
    };

    QString pythonScript;
    for (const QString &candidate : candidateScripts) {
        if (QFileInfo::exists(candidate)) {
            pythonScript = candidate;
            break;
        }
    }

    if (pythonScript.isEmpty()) {
        qWarning() << "Python bridge script not found. Running without battery data.";
        return;
    }

    m_pythonProcess->start("python3", QStringList() << pythonScript);
    
    if (!m_pythonProcess->waitForStarted(3000)) {
        qWarning() << "Failed to start Python bridge!";
        qWarning() << "Running without battery data...";
    } else {
        qDebug() << "Python bridge started successfully";
    }
}

void MainWindow::applyStyles()
{
    // Apply dark background
    setStyleSheet(
        "QMainWindow {"
        "   background-color: #0A0E1A;"
        "}"
        "QWidget {"
        "   background-color: #0A0E1A;"
        "   color: #E8F0FF;"
        "}"
    );
}

void MainWindow::onSpeedDataReceived(float pulsePerSec)
{
    // Convert pulse/s to km/h
    float speedKmh = m_dataProcessor->pulseToKmh(pulsePerSec);
    float rpm = m_dataProcessor->pulseToRPM(pulsePerSec);
    
    // Update widgets
    m_speedometer->setSpeed(speedKmh);
    m_rpmGauge->setRPM(rpm);
    
    // Update max speed
    if (speedKmh > m_maxSpeed) {
        m_maxSpeed = speedKmh;
        m_maxSpeedLabel->setText(QString("Max Speed: %1 km/h")
                                 .arg(m_maxSpeed, 0, 'f', 1));
    }
}

void MainWindow::onPythonDataReceived()
{
    m_pythonStdoutBuffer += QString::fromUtf8(m_pythonProcess->readAllStandardOutput());

    while (m_pythonStdoutBuffer.contains('\n')) {
        const int newlinePos = m_pythonStdoutBuffer.indexOf('\n');
        const QString jsonLine = m_pythonStdoutBuffer.left(newlinePos).trimmed();
        m_pythonStdoutBuffer = m_pythonStdoutBuffer.mid(newlinePos + 1);

        if (jsonLine.isEmpty()) {
            continue;
        }

        // Parse one JSON object per line
        QJsonDocument doc = QJsonDocument::fromJson(jsonLine.toUtf8());
        if (!doc.isObject()) {
            continue;
        }

        QJsonObject obj = doc.object();
        QJsonObject battery = obj["battery"].toObject();
        QString direction = obj["direction"].toString();

        // Update battery widget
        float voltage = battery["voltage"].toDouble();
        float percent = battery["percent"].toDouble();
        m_batteryWidget->setBattery(percent, voltage);

        // Update direction if available
        if (!direction.isEmpty()) {
            m_directionLabel->setText(direction);
        }
    }
}

void MainWindow::onResetButtonClicked()
{
    // Reset session timer to now
    m_startTime = QDateTime::currentMSecsSinceEpoch();
    updateElapsedTime();

    // Reset max speed record
    m_maxSpeed = 0.0f;
    m_maxSpeedLabel->setText("Max Speed: 0.0 km/h");
    
    // Visual feedback (TODO: add flash animation)
    qDebug() << "Session reset (time + max speed)";
}

void MainWindow::updateElapsedTime()
{
    qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_startTime;
    int seconds = (elapsed / 1000) % 60;
    int minutes = (elapsed / 60000) % 60;
    int hours = (elapsed / 3600000);
    
    m_timeLabel->setText(QString("%1:%2:%3")
                         .arg(hours, 2, 10, QChar('0'))
                         .arg(minutes, 2, 10, QChar('0'))
                         .arg(seconds, 2, 10, QChar('0')));
}
