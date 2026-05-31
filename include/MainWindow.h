#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QToolBar>
#include <QMap>
#include "DataTypes.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class GraphWidget;
class ConfigWindow;
class QLabel;
class QPushButton;
class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const ConnectionConfig& cfg, QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void toggleTheme();

private slots:
    void onToggleVoie();
    void updateMetrics();
    void appendLog(const QString& level, const QString& msg);
    void openConfig();

private:
    void setupVoieBar();
    void setupSidebar();
    void setupCharts();
    void setupMenuBar();
    void setupStatusBar();
    void setupLogView();

    Ui::MainWindow* ui = nullptr;

    // Graphiques
    GraphWidget* m_graphTime   = nullptr;
    GraphWidget* m_graphFFT    = nullptr;
    GraphWidget* m_graphLevels = nullptr;

    // Fenêtre config
    ConfigWindow* m_configWindow = nullptr;

    // Timers
    QTimer* m_metricTimer = nullptr;
    QTimer* m_logTimer    = nullptr;

    ConnectionConfig m_connConfig;

    QMap<QString,bool>         m_voies;
    QMap<QString,QPushButton*> m_voieBtns;

    int m_logMsgIdx = 0;
    static const QList<QPair<QString,QString>> LOG_MESSAGES;
};