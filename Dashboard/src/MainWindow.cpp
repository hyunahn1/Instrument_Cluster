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
    , m_forwardLabel(nullptr)
    , m_parkingLabel(nullptr)
    , m_backwardLabel(nullptr)
    , m_timeLabel(nullptr)
    , m_maxSpeedLabel(nullptr)
    , m_resetButton(nullptr)
    , m_serialReader(nullptr)
    , m_pythonProcess(nullptr)
    , m_dataProcessor(nullptr)
    , m_maxSpeed(0.0f)
    , m_currentSpeed(0.0f)
    , m_driveDirection("N")
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
    leftLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    leftLayout->setContentsMargins(0, 44, 0, 0);
    
    // RPM Gauge (placeholder)
    m_rpmGauge = new RpmGauge(this);
    m_rpmGauge->setFixedSize(250, 200);
    leftLayout->addWidget(m_rpmGauge);

    leftLayout->addSpacing(12);

    // Sport chrono style lap-time clock (below RPM gauge)
    QWidget *chronoWidget = new QWidget();
    chronoWidget->setObjectName("chronoWidget");
    chronoWidget->setFixedSize(112, 112);
    chronoWidget->setStyleSheet(
        "QWidget#chronoWidget {"
        "   background-color: #141D2C;"
        "   border: 2px solid #384C66;"
        "   border-radius: 56px;"
        "}"
    );
    QVBoxLayout *chronoLayout = new QVBoxLayout(chronoWidget);
    chronoLayout->setContentsMargins(10, 12, 10, 10);
    chronoLayout->setSpacing(0);

    QLabel *chronoTitle = new QLabel("LAP TIME");
    chronoTitle->setAlignment(Qt::AlignCenter);
    chronoTitle->setStyleSheet(
        "QLabel {"
        "   color: #7A8FA8;"
        "   font-family: 'Roboto';"
        "   font-size: 8pt;"
        "   letter-spacing: 1px;"
        "}"
    );
    chronoLayout->addWidget(chronoTitle);

    m_timeLabel = new QLabel("00:00:00");
    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->setStyleSheet(
        "QLabel {"
        "   font-family: 'Roboto Mono';"
        "   font-size: 14pt;"
        "   font-weight: bold;"
        "   color: #E8F0FF;"
        "}"
    );
    chronoLayout->addStretch();
    chronoLayout->addWidget(m_timeLabel);
    chronoLayout->addStretch();
    leftLayout->addWidget(chronoWidget, 0, Qt::AlignHCenter);

    leftLayout->addStretch();
    
    // === CENTER PANEL ===
    QWidget *centerPanel = new QWidget();
    centerPanel->setFixedWidth(CENTER_PANEL_WIDTH);
    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setAlignment(Qt::AlignCenter);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(0);

    // Main Speedometer
    m_speedometer = new SpeedometerWidget(this);
    m_speedometer->setFixedSize(470, 300);
    centerLayout->addWidget(m_speedometer);
    
    // === RIGHT PANEL ===
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(RIGHT_PANEL_WIDTH);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    rightLayout->setContentsMargins(22, 24, 10, 8);
    rightLayout->setSpacing(12);

    QLabel *dirTitle = new QLabel("DRIVE MODE");
    dirTitle->setAlignment(Qt::AlignCenter);
    dirTitle->setStyleSheet(
        "QLabel {"
        "   color: #6F8FB2;"
        "   font-family: 'Roboto';"
        "   font-size: 10pt;"
        "   letter-spacing: 1px;"
        "}"
    );
    rightLayout->addWidget(dirTitle);

    QWidget *directionPanel = new QWidget();
    directionPanel->setObjectName("directionPanel");
    directionPanel->setFixedSize(250, 54);
    directionPanel->setStyleSheet(
        "QWidget#directionPanel {"
        "   background-color: #111A2A;"
        "   border: 1px solid #1E3A5F;"
        "   border-radius: 8px;"
        "}"
    );
    QHBoxLayout *directionLayout = new QHBoxLayout(directionPanel);
    directionLayout->setContentsMargins(6, 6, 6, 6);
    directionLayout->setSpacing(6);

    m_forwardLabel = new QLabel("FORWARD");
    m_parkingLabel = new QLabel("PARKING");
    m_backwardLabel = new QLabel("BACKWARD");
    m_forwardLabel->setAlignment(Qt::AlignCenter);
    m_parkingLabel->setAlignment(Qt::AlignCenter);
    m_backwardLabel->setAlignment(Qt::AlignCenter);
    m_forwardLabel->setFixedWidth(75);
    m_parkingLabel->setFixedWidth(75);
    m_backwardLabel->setFixedWidth(75);

    directionLayout->addWidget(m_forwardLabel);
    directionLayout->addWidget(m_parkingLabel);
    directionLayout->addWidget(m_backwardLabel);
    rightLayout->addWidget(directionPanel);
    
    // Battery Widget
    m_batteryWidget = new BatteryWidget(this);
    m_batteryWidget->setFixedSize(220, 88);
    rightLayout->addWidget(m_batteryWidget);
    rightLayout->addSpacing(6);
    
    // Max speed card (tech-style digital panel)
    QWidget *maxSpeedCard = new QWidget();
    maxSpeedCard->setObjectName("maxSpeedCard");
    maxSpeedCard->setFixedSize(250, 78);
    maxSpeedCard->setStyleSheet(
        "QWidget#maxSpeedCard {"
        "   background-color: #111A2A;"
        "   border: 1px solid #1E3A5F;"
        "   border-radius: 10px;"
        "}"
    );

    QVBoxLayout *maxSpeedLayout = new QVBoxLayout(maxSpeedCard);
    maxSpeedLayout->setContentsMargins(12, 8, 12, 8);
    maxSpeedLayout->setSpacing(0);

    QLabel *maxTitle = new QLabel("MAX SPEED");
    maxTitle->setAlignment(Qt::AlignCenter);
    maxTitle->setStyleSheet(
        "QLabel {"
        "   color: #6F8FB2;"
        "   font-family: 'Roboto';"
        "   font-size: 10pt;"
        "   letter-spacing: 1px;"
        "}"
    );
    maxSpeedLayout->addWidget(maxTitle);

    m_maxSpeedLabel = new QLabel("0.0 km/h");
    m_maxSpeedLabel->setAlignment(Qt::AlignCenter);
    m_maxSpeedLabel->setStyleSheet(
        "QLabel {"
        "   color: #00D4FF;"
        "   font-family: 'Roboto Mono';"
        "   font-size: 19pt;"
        "   font-weight: bold;"
        "}"
    );
    maxSpeedLayout->addWidget(m_maxSpeedLabel);
    rightLayout->addWidget(maxSpeedCard);

    // Reset Button
    m_resetButton = new QPushButton("RESET");
    m_resetButton->setFixedSize(120, 34);
    m_resetButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #1C2433;"
        "   border: 1px solid #2F5E88;"
        "   border-radius: 8px;"
        "   color: #E8F0FF;"
        "   font-family: 'Roboto Mono';"
        "   font-size: 12pt;"
        "   font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "   background-color: #26354A;"
        "   border: 1px solid #00D4FF;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #0E1B2C;"
        "}"
    );
    rightLayout->addWidget(m_resetButton, 0, Qt::AlignHCenter);

    rightLayout->addStretch();

    updateDirectionIndicators();
    
    // Add panels to main layout
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(centerPanel);
    mainLayout->addWidget(rightPanel);
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
    // SerialReader now emits CAN speed directly in km/h.
    const float speedKmh = pulsePerSec;
    float rpm = 0.0f;
    if (m_dataProcessor->speedCalibration() > 0.0f) {
        const float estimatedPulsePerSec = speedKmh / m_dataProcessor->speedCalibration();
        rpm = m_dataProcessor->pulseToRPM(estimatedPulsePerSec);
    }
    m_currentSpeed = speedKmh;
    
    // Update widgets
    m_speedometer->setSpeed(speedKmh);
    m_rpmGauge->setRPM(rpm);
    updateDirectionIndicators();
    
    // Update max speed
    if (speedKmh > m_maxSpeed) {
        m_maxSpeed = speedKmh;
        m_maxSpeedLabel->setText(QString("%1 km/h")
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
        QString direction = obj["direction"].toString().trimmed().toUpper();
        // Update battery widget
        float voltage = battery["voltage"].toDouble();
        float percent = battery["percent"].toDouble();
        m_batteryWidget->setBattery(percent, voltage);

        // Update direction mode if available
        if (!direction.isEmpty()) {
            if (direction.startsWith("R")) {
                m_driveDirection = "R";
            } else if (direction.startsWith("F")) {
                m_driveDirection = "F";
            } else {
                m_driveDirection = "N";
            }
            updateDirectionIndicators();
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
    m_maxSpeedLabel->setText("0.0 km/h");
    
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

void MainWindow::applyDirectionIndicatorStyle(QLabel *label, bool active, const QString &activeColor)
{
    if (!label) {
        return;
    }

    if (active) {
        label->setStyleSheet(
            "QLabel {"
            "   background-color: " + activeColor + ";"
            "   color: #08121F;"
            "   border: 1px solid " + activeColor + ";"
            "   border-radius: 6px;"
            "   font-family: 'Roboto';"
            "   font-size: 7pt;"
            "   font-weight: bold;"
            "   padding: 3px 4px;"
            "}"
        );
    } else {
        label->setStyleSheet(
            "QLabel {"
            "   background-color: #1A2436;"
            "   color: #6F7F93;"
            "   border: 1px solid #2A3B50;"
            "   border-radius: 6px;"
            "   font-family: 'Roboto';"
            "   font-size: 7pt;"
            "   font-weight: 500;"
            "   padding: 3px 4px;"
            "}"
        );
    }
}

void MainWindow::updateDirectionIndicators()
{
    constexpr float PARKING_SPEED_THRESHOLD = 0.15f;

    const bool isParking = m_currentSpeed <= PARKING_SPEED_THRESHOLD;
    const bool isForward = !isParking && (m_driveDirection == "F");
    const bool isBackward = !isParking && (m_driveDirection == "R");

    applyDirectionIndicatorStyle(m_forwardLabel, isForward, "#00FF88");
    applyDirectionIndicatorStyle(m_parkingLabel, isParking, "#FFD34D");
    applyDirectionIndicatorStyle(m_backwardLabel, isBackward, "#FF5B6E");
}
