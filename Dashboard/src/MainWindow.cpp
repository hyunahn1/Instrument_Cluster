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
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QEasingCurve>
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
    , m_lastCenterMode("")
    , m_centerModeOpacity(nullptr)
    , m_centerModeAnim(nullptr)
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
    centralWidget->setObjectName("dashboardRoot");
    setCentralWidget(centralWidget);
    
    // Main horizontal layout
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // === LEFT PANEL ===
    QWidget *leftPanel = new QWidget();
    leftPanel->setObjectName("leftPanel");
    leftPanel->setFixedWidth(LEFT_PANEL_WIDTH);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    leftLayout->setContentsMargins(0, 44, 0, 0);
    
    // RPM Gauge (placeholder)
    m_rpmGauge = new RpmGauge(this);
    m_rpmGauge->setFixedSize(236, 192);
    leftLayout->addWidget(m_rpmGauge, 0, Qt::AlignHCenter);

    leftLayout->addSpacing(12);

    // Sport chrono style lap-time clock (below RPM gauge)
    QWidget *chronoWidget = new QWidget();
    chronoWidget->setObjectName("chronoWidget");
    chronoWidget->setFixedSize(112, 112);
    chronoWidget->setStyleSheet(
        "QWidget#chronoWidget {"
        "   background-color: #111823;"
        "   border: 2px solid #5A6D86;"
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
        "   color: #C8B07A;"
        "   font-family: 'Roboto Condensed';"
        "   font-size: 8pt;"
        "   font-weight: 700;"
        "   letter-spacing: 1.4px;"
        "}"
    );
    chronoLayout->addWidget(chronoTitle);

    m_timeLabel = new QLabel("00:00:00");
    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->setStyleSheet(
        "QLabel {"
        "   font-family: 'Roboto Mono';"
        "   font-size: 12pt;"
        "   font-weight: bold;"
        "   color: #F3F8FF;"
        "}"
    );
    chronoLayout->addStretch();
    chronoLayout->addWidget(m_timeLabel);
    chronoLayout->addStretch();

    // Move RESET next to the lap-time clock as a small "crown" button.
    QWidget *chronoRow = new QWidget();
    QHBoxLayout *chronoRowLayout = new QHBoxLayout(chronoRow);
    chronoRowLayout->setContentsMargins(8, 0, 0, 0);
    chronoRowLayout->setSpacing(10);
    chronoRowLayout->addWidget(chronoWidget);

    m_resetButton = new QPushButton("↻");
    m_resetButton->setToolTip("Reset session");
    m_resetButton->setFixedSize(46, 46);
    m_resetButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #16263A;"
        "   border: 1px solid #3F6288;"
        "   border-radius: 23px;"
        "   color: #DDEBFF;"
        "   font-family: 'Roboto Mono';"
        "   font-size: 18pt;"
        "   font-weight: 700;"
        "}"
        "QPushButton:hover {"
        "   background-color: #243A55;"
        "   border: 1px solid #00D4FF;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #112034;"
        "}"
    );
    chronoRowLayout->addWidget(m_resetButton, 0, Qt::AlignVCenter);
    leftLayout->addWidget(chronoRow, 0, Qt::AlignHCenter);

    leftLayout->addStretch();
    
    // === CENTER PANEL ===
    QWidget *centerPanel = new QWidget();
    centerPanel->setObjectName("centerPanel");
    centerPanel->setFixedWidth(CENTER_PANEL_WIDTH);
    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setAlignment(Qt::AlignCenter);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(0);

    // Main Speedometer
    m_speedometer = new SpeedometerWidget(this);
    m_speedometer->setFixedSize(520, 340);
    centerLayout->addWidget(m_speedometer);
    
    // === RIGHT PANEL ===
    QWidget *rightPanel = new QWidget();
    rightPanel->setObjectName("rightPanel");
    rightPanel->setFixedWidth(RIGHT_PANEL_WIDTH);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    rightLayout->setContentsMargins(0, 56, 0, 8);
    rightLayout->setSpacing(10);

    QLabel *dirTitle = new QLabel("DRIVE MODE");
    dirTitle->setAlignment(Qt::AlignCenter);
    dirTitle->setStyleSheet(
        "QLabel {"
        "   color: #7EA2C9;"
        "   font-family: 'Roboto Condensed';"
        "   font-size: 11pt;"
        "   font-weight: 700;"
        "   letter-spacing: 2px;"
        "}"
    );
    rightLayout->addWidget(dirTitle, 0, Qt::AlignHCenter);

    QWidget *directionPanel = new QWidget();
    directionPanel->setObjectName("directionPanel");
    directionPanel->setFixedSize(240, 58);
    directionPanel->setStyleSheet(
        "QWidget#directionPanel {"
        "   background-color: #0D1728;"
        "   border: 1px solid #27466B;"
        "   border-radius: 10px;"
        "}"
    );
    QHBoxLayout *directionLayout = new QHBoxLayout(directionPanel);
    directionLayout->setContentsMargins(6, 6, 6, 6);
    directionLayout->setSpacing(5);

    m_forwardLabel = new QLabel("F");
    m_parkingLabel = new QLabel("P");
    m_backwardLabel = new QLabel("R");
    m_forwardLabel->setAlignment(Qt::AlignCenter);
    m_parkingLabel->setAlignment(Qt::AlignCenter);
    m_backwardLabel->setAlignment(Qt::AlignCenter);
    m_forwardLabel->setFixedWidth(54);
    m_parkingLabel->setFixedWidth(104);
    m_backwardLabel->setFixedWidth(54);
    m_centerModeOpacity = new QGraphicsOpacityEffect(this);
    m_centerModeOpacity->setOpacity(1.0);
    m_parkingLabel->setGraphicsEffect(m_centerModeOpacity);

    directionLayout->addWidget(m_forwardLabel);
    directionLayout->addWidget(m_parkingLabel);
    directionLayout->addWidget(m_backwardLabel);
    rightLayout->addWidget(directionPanel, 0, Qt::AlignHCenter);
    rightLayout->addSpacing(12);
    
    // Max speed card (supercar badge style)
    QWidget *maxSpeedCard = new QWidget();
    maxSpeedCard->setObjectName("maxSpeedCard");
    maxSpeedCard->setFixedSize(228, 78);
    maxSpeedCard->setStyleSheet(
        "QWidget#maxSpeedCard {"
        "   background-color: #0E1626;"
        "   border: 1px solid #2A466A;"
        "   border-radius: 10px;"
        "}"
    );

    QHBoxLayout *maxSpeedOuter = new QHBoxLayout(maxSpeedCard);
    maxSpeedOuter->setContentsMargins(10, 0, 10, 0);
    maxSpeedOuter->setSpacing(0);

    QWidget *maxSpeedBody = new QWidget();
    QVBoxLayout *maxSpeedLayout = new QVBoxLayout(maxSpeedBody);
    maxSpeedLayout->setContentsMargins(0, 8, 0, 8);
    maxSpeedLayout->setSpacing(0);
    maxSpeedLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QLabel *maxTitle = new QLabel("V-MAX");
    maxTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    maxTitle->setStyleSheet(
        "QLabel {"
        "   color: #8DA5C2;"
        "   font-family: 'Roboto Condensed';"
        "   font-size: 9pt;"
        "   font-weight: 700;"
        "   letter-spacing: 1.8px;"
        "}"
    );
    maxSpeedLayout->addWidget(maxTitle);

    QWidget *maxValueRow = new QWidget();
    QHBoxLayout *maxValueLayout = new QHBoxLayout(maxValueRow);
    maxValueLayout->setContentsMargins(0, 0, 0, 0);
    maxValueLayout->setSpacing(6);
    maxValueLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    m_maxSpeedLabel = new QLabel("0.0");
    m_maxSpeedLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_maxSpeedLabel->setStyleSheet(
        "QLabel {"
        "   color: #00D4FF;"
        "   font-family: 'Roboto Mono';"
        "   font-size: 20pt;"
        "   font-weight: bold;"
        "}"
    );
    QLabel *maxUnitLabel = new QLabel("km/h");
    maxUnitLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    maxUnitLabel->setStyleSheet(
        "QLabel {"
        "   color: #7FA2C6;"
        "   font-family: 'Roboto';"
        "   font-size: 9pt;"
        "   padding-bottom: 3px;"
        "}"
    );
    maxValueLayout->addWidget(m_maxSpeedLabel, 0);
    maxValueLayout->addWidget(maxUnitLabel, 0);
    maxSpeedLayout->addWidget(maxValueRow);

    maxSpeedOuter->addWidget(maxSpeedBody, 1);
    rightLayout->addWidget(maxSpeedCard, 0, Qt::AlignHCenter);

    // Keep battery on the right, but move it down to lap-time-like height.
    rightLayout->addSpacing(38);
    m_batteryWidget = new BatteryWidget(this);
    m_batteryWidget->setFixedSize(220, 88);
    rightLayout->addWidget(m_batteryWidget, 0, Qt::AlignHCenter);

    rightLayout->addStretch();

    updateDirectionIndicators();
    
    // Add panels with outer stretches so both widgets and panel backgrounds
    // move toward the center as a single cluster.
    mainLayout->addStretch(1);
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(centerPanel);
    mainLayout->addWidget(rightPanel);
    mainLayout->addStretch(1);
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
    // Base style is provided by dynamic theme builder.
    applyDynamicBackgroundTheme("P");
}

void MainWindow::applyDynamicBackgroundTheme(const QString &mode)
{
    QString tintCore = "#10223D";
    QString tintMid = "#0A1730";
    QString tintEdge = "#050E20";
    QString spotlightCore = "rgba(190,220,255,18)";
    QString spotlightOuter = "rgba(190,220,255,0)";

    // Subtle temperature shift by drive mode.
    if (mode == "F") {
        // Slightly cooler for forward mode.
        tintCore = "#10253D";
        tintMid = "#0B1B31";
        tintEdge = "#061022";
        spotlightCore = "rgba(130,210,255,24)";
        spotlightOuter = "rgba(130,210,255,0)";
    } else if (mode == "R") {
        // Slightly warmer for reverse mode.
        tintCore = "#1A1E35";
        tintMid = "#14162D";
        tintEdge = "#0B0D1C";
        spotlightCore = "rgba(255,125,140,18)";
        spotlightOuter = "rgba(255,125,140,0)";
    } else {
        // Neutral/warm parking.
        tintCore = "#162237";
        tintMid = "#11192B";
        tintEdge = "#090F1D";
        spotlightCore = "rgba(255,210,130,14)";
        spotlightOuter = "rgba(255,210,130,0)";
    }

    const QString fullStyle =
        "QMainWindow {"
        "   background-color: #030814;"
        "}"
        "QWidget#dashboardRoot {"
        "   background-color: qradialgradient("
        "       cx:0.52, cy:0.44, radius:0.95,"
        "       stop:0 " + tintCore + ","
        "       stop:0.38 " + tintMid + ","
        "       stop:0.72 " + tintEdge + ","
        "       stop:1 #030814"
        "   );"
        "   border-top: 1px solid rgba(140, 190, 255, 18);"
        "}"
        "QWidget#centerPanel {"
        "   background-color: qradialgradient("
        "       cx:0.5, cy:0.48, radius:0.65,"
        "       stop:0 " + spotlightCore + ","
        "       stop:1 " + spotlightOuter +
        "   );"
        "}"
        "QWidget#leftPanel, QWidget#rightPanel {"
        "   background: transparent;"
        "}"
        "QWidget {"
        "   background: transparent;"
        "   color: #E8F0FF;"
        "}";
    setStyleSheet(fullStyle);
}

void MainWindow::animateCenterMode(const QString &newMode)
{
    if (!m_parkingLabel || !m_centerModeOpacity) {
        return;
    }

    if (m_lastCenterMode.isEmpty()) {
        m_lastCenterMode = newMode;
        m_parkingLabel->setText(newMode);
        m_centerModeOpacity->setOpacity(1.0);
        return;
    }

    if (newMode == m_lastCenterMode) {
        return;
    }

    m_lastCenterMode = newMode;
    m_parkingLabel->setText(newMode);

    if (m_centerModeAnim) {
        m_centerModeAnim->stop();
        m_centerModeAnim->deleteLater();
        m_centerModeAnim = nullptr;
    }

    const QRect endRect = m_parkingLabel->geometry();
    int direction = 0;
    if (newMode == "F") direction = -1;
    else if (newMode == "R") direction = 1;
    const QRect startRect = endRect.translated(direction * 16, 0);

    m_parkingLabel->setGeometry(startRect);
    m_centerModeOpacity->setOpacity(0.0);

    auto *slideAnim = new QPropertyAnimation(m_parkingLabel, "geometry");
    slideAnim->setDuration(180);
    slideAnim->setStartValue(startRect);
    slideAnim->setEndValue(endRect);
    slideAnim->setEasingCurve(QEasingCurve::OutCubic);

    auto *fadeAnim = new QPropertyAnimation(m_centerModeOpacity, "opacity");
    fadeAnim->setDuration(180);
    fadeAnim->setStartValue(0.0);
    fadeAnim->setEndValue(1.0);
    fadeAnim->setEasingCurve(QEasingCurve::OutCubic);

    m_centerModeAnim = new QParallelAnimationGroup(this);
    m_centerModeAnim->addAnimation(slideAnim);
    m_centerModeAnim->addAnimation(fadeAnim);
    connect(m_centerModeAnim, &QParallelAnimationGroup::finished, this, [this]() {
        if (m_centerModeAnim) {
            m_centerModeAnim->deleteLater();
            m_centerModeAnim = nullptr;
        }
    });
    m_centerModeAnim->start();
}

bool MainWindow::updateDirectionFromSnapshot()
{
    QFileInfo directionInfo("/tmp/piracer_drive_mode.json");
    if (!directionInfo.exists()) {
        return false;
    }

    // Use snapshot only when it is fresh enough.
    if (directionInfo.lastModified().msecsTo(QDateTime::currentDateTime()) > 2000) {
        return false;
    }

    QFile directionFile(directionInfo.absoluteFilePath());
    if (!directionFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    const QByteArray payload = directionFile.readAll();
    directionFile.close();
    const QJsonDocument doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) {
        return false;
    }

    const QString direction = doc.object().value("direction").toString().trimmed().toUpper();
    if (direction.startsWith("R")) {
        m_driveDirection = "R";
        return true;
    }
    if (direction.startsWith("F")) {
        m_driveDirection = "F";
        return true;
    }
    if (direction.startsWith("N")) {
        m_driveDirection = "N";
        return true;
    }

    return false;
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
    updateDirectionFromSnapshot();
    
    // Update widgets
    m_speedometer->setSpeed(speedKmh);
    m_rpmGauge->setRPM(rpm);
    updateDirectionIndicators();
    
    // Update max speed
    if (speedKmh > m_maxSpeed) {
        m_maxSpeed = speedKmh;
        m_maxSpeedLabel->setText(QString("%1").arg(m_maxSpeed, 0, 'f', 1));
        // Pulse effect when a new max speed record is set.
        m_maxSpeedLabel->setStyleSheet(
            "QLabel {"
            "   color: #A7F6FF;"
            "   font-family: 'Roboto Mono';"
            "   font-size: 21pt;"
            "   font-weight: bold;"
            "}"
        );
        QTimer::singleShot(180, this, [this]() {
            if (!m_maxSpeedLabel) return;
            m_maxSpeedLabel->setStyleSheet(
                "QLabel {"
                "   color: #00D4FF;"
                "   font-family: 'Roboto Mono';"
                "   font-size: 20pt;"
                "   font-weight: bold;"
                "}"
            );
        });
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
        // Update battery widget
        float voltage = battery["voltage"].toDouble();
        float percent = battery["percent"].toDouble();
        m_batteryWidget->setBattery(percent, voltage);

        // Direction is controlled by the local drive-mode snapshot (X/B/Y).
        // Ignore bridge direction to avoid parking flicker during driving.
        if (updateDirectionFromSnapshot()) {
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
    m_maxSpeedLabel->setText("0.0");
    
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
            "   border-radius: 8px;"
            "   font-family: 'Roboto Condensed';"
            "   font-size: 7pt;"
            "   font-weight: bold;"
            "   letter-spacing: 1.2px;"
            "   padding: 4px 5px;"
            "}"
        );
    } else {
        label->setStyleSheet(
            "QLabel {"
            "   background-color: #1A2940;"
            "   color: #7A8FA8;"
            "   border: 1px solid #2D4867;"
            "   border-radius: 8px;"
            "   font-family: 'Roboto Condensed';"
            "   font-size: 7pt;"
            "   font-weight: 500;"
            "   letter-spacing: 1.2px;"
            "   padding: 4px 5px;"
            "}"
        );
    }
}

void MainWindow::updateDirectionIndicators()
{
    // Option C: center large current mode + side hint modes.
    QString current = "P";
    if (m_driveDirection == "F") {
        current = "F";
    } else if (m_driveDirection == "R") {
        current = "R";
    }

    QString leftHint = "F";
    QString rightHint = "R";
    QString activeColor = "#FFD34D";  // Parking default
    if (current == "F") {
        leftHint = "P";
        rightHint = "R";
        activeColor = "#00FF88";
    } else if (current == "R") {
        leftHint = "F";
        rightHint = "P";
        activeColor = "#FF5B6E";
    }

    m_forwardLabel->setText(leftHint);
    m_backwardLabel->setText(rightHint);
    animateCenterMode(current);
    applyDynamicBackgroundTheme(current);

    m_forwardLabel->setStyleSheet(
        "QLabel {"
        "   background-color: #1A2940;"
        "   color: #8FA6C2;"
        "   border: 1px solid #2D4867;"
        "   border-radius: 8px;"
        "   font-family: 'Roboto Condensed';"
        "   font-size: 10pt;"
        "   font-weight: 600;"
        "   padding: 2px 2px;"
        "}"
    );
    m_backwardLabel->setStyleSheet(
        "QLabel {"
        "   background-color: #1A2940;"
        "   color: #8FA6C2;"
        "   border: 1px solid #2D4867;"
        "   border-radius: 8px;"
        "   font-family: 'Roboto Condensed';"
        "   font-size: 10pt;"
        "   font-weight: 600;"
        "   padding: 2px 2px;"
        "}"
    );
    m_parkingLabel->setStyleSheet(
        "QLabel {"
        "   background-color: " + activeColor + ";"
        "   color: #08121F;"
        "   border: 1px solid " + activeColor + ";"
        "   border-radius: 8px;"
        "   font-family: 'Roboto Condensed';"
        "   font-size: 18pt;"
        "   font-weight: 800;"
        "   padding: 0px 2px;"
        "}"
    );
}
