#pragma once
#include <QWidget>
#include <QTimer>
#include <QMap>
#include <vector>
#include <deque>

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    enum GraphType { TimeDomain, FFT, Levels, Spectrogram, MiniPlot };
    explicit GraphWidget(GraphType type, QWidget* parent = nullptr);
    void setVoieActive(const QString& key, bool active);
    bool isDark() const;
    void setMiniPlotColor(const QColor& c)     { m_miniColor = c; }
    void setMiniPlotUnit(const QString& u)      { m_miniUnit  = u; }
    void setMiniPlotRange(double lo, double hi) { m_miniLo=lo; m_miniHi=hi; }
    void setMiniPlotLabel(const QString& l)     { m_miniLabel = l; }
    void pushMiniValue(double v);
protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override { update(); }
private slots:
    void onTimer();
private:
    void drawTimeDomain(QPainter& p);
    void drawFFT(QPainter& p);
    void drawLevels(QPainter& p);
    void drawSpectrogram(QPainter& p);
    void drawMiniPlot(QPainter& p);
    void drawGrid(QPainter& p,int cols,int rows,
                  int oL=0,int oR=0,int oT=0,int oB=0);
    void updateSpectroBuffer();

    GraphType m_type;
    QTimer*   m_timer;
    double    m_T = 0.0;
    QMap<QString,bool>   m_voies;
    QMap<QString,QColor> m_colors;
    std::vector<std::vector<double>> m_spectroBuffer;

    // MiniPlot
    std::deque<double> m_miniData;
    QColor  m_miniColor  = QColor("#00d4ff");
    QString m_miniUnit   = "";
    QString m_miniLabel  = "";
    double  m_miniLo     = 0.0;
    double  m_miniHi     = 100.0;
    int     m_miniMaxPts = 120;
};
