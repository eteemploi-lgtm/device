#include "GraphWidget.h"
#include "StyleManager.h"

#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QtMath>
#include <QRandomGenerator>

static const QMap<QString,QColor> COLORS = {
    {"h1", QColor("#00d4ff")},
    {"h2", QColor("#00e676")},
    {"h3", QColor("#ffaa00")},
    {"h4", QColor("#ff6b9d")},
    {"a1", QColor("#c792ea")},
    {"a2", QColor("#89ddff")},
};

GraphWidget::GraphWidget(GraphType type, QWidget* parent)
    : QWidget(parent), m_type(type)
{
    // Voies par défaut toutes actives
    for (auto& k : COLORS.keys()) m_voies[k] = true;
    m_colors = COLORS;

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GraphWidget::onTimer);
    m_timer->start(33); // ~30 fps
    setMinimumSize(200, 80);
}

void GraphWidget::setVoieActive(const QString& key, bool active)
{
    m_voies[key] = active;
}

bool GraphWidget::isDark() const
{
    return StyleManager::instance().isDark();
}

void GraphWidget::onTimer()
{
    m_T += 1.0;
    update();
}

void GraphWidget::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
}

// ─── Paint ───────────────────────────────────────────────────────────────────
void GraphWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, m_type != Levels);

    QColor bg = isDark() ? QColor("#0d1117") : QColor("#f8fafc");
    p.fillRect(rect(), bg);

    switch (m_type) {
        case TimeDomain: drawTimeDomain(p); break;
        case FFT:        drawFFT(p);        break;
        case Levels:     drawLevels(p);     break;
    }
}

void GraphWidget::drawGrid(QPainter& p, int cols, int rows)
{
    QColor gc = isDark() ? QColor("#1f2937") : QColor("#e2e8f0");
    p.setPen(QPen(gc, 0.5));
    int W = width(), H = height();
    for (int i = 1; i < rows; i++)
        p.drawLine(0, H*i/rows, W, H*i/rows);
    for (int i = 1; i < cols; i++)
        p.drawLine(W*i/cols, 0, W*i/cols, H);
}

// ─── Signal temporel multi-voies ─────────────────────────────────────────────
void GraphWidget::drawTimeDomain(QPainter& p)
{
    drawGrid(p, 8, 5);

    int W = width(), H = height();
    // Phases et fréquences propres à chaque hydro (simulation)
    static const double phases[] = {0, 0.8, 1.6, 2.4};
    static const double freqs[]  = {1.0, 1.3, 0.9, 1.6};
    static const double amps[]   = {0.25, 0.19, 0.28, 0.15};

    for (int vi = 1; vi <= 4; vi++) {
        QString key = QString("h%1").arg(vi);
        if (!m_voies.value(key, false)) continue;

        QColor col = m_colors.value(key);
        p.setPen(QPen(col, 1.5));

        QPainterPath path;
        double yBase = H * (0.12 + (vi-1) * 0.19) + H * 0.06;

        for (int x = 0; x < W; x++) {
            double tt = (double(x)/W) * 6*M_PI + m_T * 0.04 + phases[vi-1];
            double mod = 0.7 + 0.3 * qSin(m_T * 0.015 + vi);
            double y = yBase - qSin(tt * freqs[vi-1]) * H * amps[vi-1] * mod;
            if (x == 0) path.moveTo(x, y);
            else        path.lineTo(x, y);
        }
        p.drawPath(path);

        // Label voie
        p.setPen(col);
        p.setFont(QFont("Courier New", 8));
        p.drawText(4, int(yBase - H * amps[vi-1] - 2), key.toUpper());
    }
}

// ─── FFT ─────────────────────────────────────────────────────────────────────
void GraphWidget::drawFFT(QPainter& p)
{
    drawGrid(p, 8, 4);

    int W = width(), H = height();
    const int BINS = 64;
    double bw = double(W) / BINS;

    for (int i = 0; i < BINS; i++) {
        double noise = (QRandomGenerator::global()->generateDouble()) * 0.06;
        double peak  = (i == 38) ? 0.90 + noise
                     : (i == 19) ? 0.42 + noise
                     : (i == 57) ? 0.28 + noise : noise;
        double env   = qExp(-qPow((i-38.0)/11.0, 2)) * 0.55 + noise;
        double bh    = (peak + env) * H * 0.88;

        QColor col = (i == 38) ? QColor("#00d4ff")
                   : (i == 19) ? QColor("#00e676")
                   :              QColor("#444c56");
        p.fillRect(QRectF(i*bw, H-bh, bw-1, bh), col);
    }

    // Étiquette pic principal
    p.setPen(QColor("#00d4ff"));
    p.setFont(QFont("Courier New", 8));
    int xPeak = int(38 * W / BINS);
    p.drawText(xPeak - 4, int(H * 0.08), "38.4 kHz");
}

// ─── Niveaux (barres) ────────────────────────────────────────────────────────
void GraphWidget::drawLevels(QPainter& p)
{
    int W = width(), H = height();
    QColor bgBar = isDark() ? QColor("#1f2937") : QColor("#e2e8f0");

    static const QMap<QString,double> BASE = {
        {"h1",0.75},{"h2",0.62},{"h3",0.88},{"h4",0.12},{"a1",0.55},{"a2",0.45}
    };

    QStringList active;
    for (auto& k : COLORS.keys())
        if (m_voies.value(k, false)) active << k;

    if (active.isEmpty()) return;

    int barW   = qMin(36, (W - 20) / active.size() - 6);
    int spacing = (W - 20 - active.size()*barW) / active.size();

    for (int i = 0; i < active.size(); i++) {
        const QString& k = active[i];
        int x = 10 + i * (barW + spacing);
        double lv = BASE.value(k, 0.5) + 0.04 * qSin(m_T * 0.07 + i);
        int bh = int(lv * (H - 24));

        // Fond barre
        p.fillRect(x, 8, barW, H-24, bgBar);
        // Barre niveaux
        QColor col = m_colors.value(k);
        col.setAlphaF(0.9);
        p.fillRect(x, H - bh - 16, barW, bh, col);

        // Labels
        p.setPen(m_colors.value(k));
        p.setFont(QFont("Courier New", 8));
        p.drawText(QRect(x, H-14, barW, 12), Qt::AlignCenter, k.toUpper());

        double db = -8.0 + lv * 28.0;
        p.drawText(QRect(x, H - bh - 26, barW, 12),
                   Qt::AlignCenter, QString::number(db, 'f', 1));
    }
}
