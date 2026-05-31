#pragma once
#include <QWidget>
#include <QTimer>
#include <QMap>

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    enum GraphType { TimeDomain, FFT, Levels };

    explicit GraphWidget(GraphType type, QWidget* parent = nullptr);

    void setVoieActive(const QString& key, bool active);
    bool isDark() const;

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private slots:
    void onTimer();

private:
    void drawTimeDomain(QPainter& p);
    void drawFFT(QPainter& p);
    void drawLevels(QPainter& p);
    void drawGrid(QPainter& p, int cols, int rows);

    GraphType m_type;
    QTimer*   m_timer;
    double    m_T = 0.0;

    // Voies actives : "h1","h2","h3","h4","a1","a2"
    QMap<QString,bool>    m_voies;
    QMap<QString,QColor>  m_colors;
};
