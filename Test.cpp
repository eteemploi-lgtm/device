#include "GraphWidget.h"
#include "StyleManager.h"

#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QRandomGenerator>
#include <QtMath>

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
    for (auto& k : COLORS.keys()) m_voies[k] = true;
    m_colors = COLORS;

    // Buffer spectrogramme : 300 colonnes x 100 lignes
    m_spectroBuffer.assign(300, std::vector<double>(100, 0.0));

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GraphWidget::onTimer);
    m_timer->start(33);
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
    if (m_type == Spectrogram)
        updateSpectroBuffer();
    update();
}

// ─── Mise à jour buffer spectrogramme (défilement gauche) ────────────────────
void GraphWidget::updateSpectroBuffer()
{
    int cols = m_spectroBuffer.size();
    int rows = m_spectroBuffer[0].size();

    // Décaler tout vers la gauche
    for (int x = 0; x < cols - 1; x++)
        m_spectroBuffer[x] = m_spectroBuffer[x+1];

    // Nouvelle colonne à droite
    for (int y = 0; y < rows; y++) {
        double freq = double(y) / rows;
        double energy =
            0.75 * qExp(-qPow((freq - 0.40) / 0.10, 2))
          + 0.35 * qExp(-qPow((freq - 0.20) / 0.07, 2))
          + 0.20 * qExp(-qPow((freq - 0.65) / 0.05, 2))
          + 0.05 * QRandomGenerator::global()->generateDouble()
          + 0.08 * qSin(m_T * 0.03 + freq * 10.0);
        m_spectroBuffer[cols-1][y] = qBound(0.0, energy, 1.0);
    }
}

// ─── Couleur palette thermique ────────────────────────────────────────────────
static QColor thermalColor(double v)
{
    v = qBound(0.0, v, 1.0);
    if      (v < 0.20) return QColor::fromHsvF(0.67, 1.0, v * 5.0);
    else if (v < 0.40) return QColor::fromHsvF(0.55, 1.0, 1.0);
    else if (v < 0.60) return QColor::fromHsvF(0.38, 1.0, 1.0);
    else if (v < 0.80) return QColor::fromHsvF(0.17, 1.0, 1.0);
    else               return QColor::fromHsvF(0.0,  1.0, 1.0);
}

// ─── Paint ───────────────────────────────────────────────────────────────────
void GraphWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, m_type != Levels && m_type != Spectrogram);

    QColor bg = isDark() ? QColor("#0d1117") : QColor("#f8fafc");
    p.fillRect(rect(), bg);

    switch (m_type) {
        case TimeDomain:  drawTimeDomain(p);  break;
        case FFT:         drawFFT(p);         break;
        case Levels:      drawLevels(p);      break;
        case Spectrogram: drawSpectrogram(p); break;
    }
}

// ─── Grille ───────────────────────────────────────────────────────────────────
void GraphWidget::drawGrid(QPainter& p, int cols, int rows,
                            int offL, int offR, int offT, int offB)
{
    QColor gc = isDark() ? QColor("#1f2937") : QColor("#e2e8f0");
    p.setPen(QPen(gc, 0.5));
    int W = width()  - offL - offR;
    int H = height() - offT - offB;
    for (int i = 1; i < rows; i++) {
        int y = offT + H*i/rows;
        p.drawLine(offL, y, offL+W, y);
    }
    for (int i = 1; i < cols; i++) {
        int x = offL + W*i/cols;
        p.drawLine(x, offT, x, offT+H);
    }
}

// ─── Labels axes ─────────────────────────────────────────────────────────────
void GraphWidget::drawAxisLabels(QPainter& p,
    const QStringList& xLabels, const QStringList& yLabels,
    int offL, int offR, int offT, int offB)
{
    QColor tc = isDark() ? QColor("#6e7681") : QColor("#718096");
    p.setPen(tc);
    p.setFont(QFont("Courier New", 8));

    int W = width()  - offL - offR;
    int H = height() - offT - offB;

    // Labels X (bas)
    for (int i = 0; i < xLabels.size(); i++) {
        int x = offL + W * i / (xLabels.size()-1);
        p.drawText(x - 16, height() - 1, xLabels[i]);
    }
    // Labels Y (gauche)
    for (int i = 0; i < yLabels.size(); i++) {
        int y = offT + H * i / (yLabels.size()-1);
        p.drawText(1, y + 4, yLabels[i]);
    }
}

// ─── Signal temporel ─────────────────────────────────────────────────────────
void GraphWidget::drawTimeDomain(QPainter& p)
{
    const int OL=38, OR=6, OT=6, OB=18;
    drawGrid(p, 8, 4, OL, OR, OT, OB);

    QStringList xLbls = {"0","0.25","0.5","0.75","1 ms"};
    QStringList yLbls = {"+1V","0","-1V"};
    // Axe Y vertical
    QColor tc = isDark() ? QColor("#6e7681") : QColor("#718096");
    p.setPen(tc);
    p.setFont(QFont("Courier New", 8));
    int H = height()-OT-OB;
    p.drawText(2, OT+4,        "+1V");
    p.drawText(2, OT+H/2+4,    " 0V");
    p.drawText(2, OT+H-2,      "-1V");
    // Axe X bas
    int W = width()-OL-OR;
    QStringList xl = {"0","0.25","0.50","0.75","1ms"};
    for (int i=0; i<xl.size(); i++) {
        int x = OL + W*i/(xl.size()-1);
        p.drawText(x-10, height()-2, xl[i]);
    }
    // Unité
    p.setPen(isDark() ? QColor("#444c56") : QColor("#a0aec0"));
    p.drawText(2, OT-1, "V");

    // Courbes
    static const double phases[] = {0, 0.8, 1.6, 2.4};
    static const double freqs[]  = {1.0, 1.3, 0.9, 1.6};
    static const double amps[]   = {0.25, 0.19, 0.28, 0.15};

    for (int vi = 1; vi <= 4; vi++) {
        QString key = QString("h%1").arg(vi);
        if (!m_voies.value(key, false)) continue;

        QColor col = m_colors.value(key);
        p.setPen(QPen(col, 1.5));

        QPainterPath path;
        double yBase = OT + H * (0.12 + (vi-1)*0.19) + H*0.06;

        for (int x = 0; x < W; x++) {
            double tt = (double(x)/W)*6*M_PI + m_T*0.04 + phases[vi-1];
            double mod = 0.7 + 0.3*qSin(m_T*0.015 + vi);
            double y = yBase - qSin(tt*freqs[vi-1]) * H*amps[vi-1] * mod;
            x == 0 ? path.moveTo(OL+x, y) : path.lineTo(OL+x, y);
        }
        p.drawPath(path);

        // Label voie
        p.setPen(col);
        p.setFont(QFont("Courier New", 9, QFont::Bold));
        p.drawText(OL+4, int(yBase - H*amps[vi-1] - 2), key.toUpper());
    }
}

// ─── FFT ─────────────────────────────────────────────────────────────────────
void GraphWidget::drawFFT(QPainter& p)
{
    const int OL=40, OR=6, OT=6, OB=18;
    drawGrid(p, 8, 4, OL, OR, OT, OB);

    int W = width()-OL-OR;
    int H = height()-OT-OB;

    // Axes
    QColor tc = isDark() ? QColor("#6e7681") : QColor("#718096");
    p.setPen(tc);
    p.setFont(QFont("Courier New", 8));
    // Y : dB
    p.drawText(2, OT+4,        " 0dB");
    p.drawText(2, OT+H/4+4,   "-20");
    p.drawText(2, OT+H/2+4,   "-40");
    p.drawText(2, OT+3*H/4+4, "-60");
    p.drawText(2, OT+H-2,     "-80");
    // X : kHz
    QStringList fLabels = {"0","12","24","36","48","60","72","84","96kHz"};
    for (int i=0; i<fLabels.size(); i++) {
        int x = OL + W*i/(fLabels.size()-1);
        p.drawText(x-10, height()-2, fLabels[i]);
    }

    // Barres FFT
    const int BINS = 64;
    double bw = double(W) / BINS;
    for (int i = 0; i < BINS; i++) {
        double noise = QRandomGenerator::global()->generateDouble() * 0.06;
        double peak  = (i==38) ? 0.90+noise : (i==19) ? 0.42+noise : (i==57) ? 0.28+noise : noise;
        double env   = qExp(-qPow((i-38.0)/11.0,2))*0.55 + noise;
        double bh    = (peak+env) * H * 0.88;

        QColor col = (i==38) ? QColor("#00d4ff") : (i==19) ? QColor("#00e676") : QColor("#444c56");
        p.fillRect(QRectF(OL+i*bw, OT+H-bh, bw-1, bh), col);
    }

    // Étiquette pic
    p.setPen(QColor("#00d4ff"));
    p.setFont(QFont("Courier New", 9, QFont::Bold));
    int xPeak = OL + int(38*W/64);
    p.drawText(xPeak-4, OT+int(H*0.08), "38.4 kHz");
}

// ─── Niveaux ─────────────────────────────────────────────────────────────────
void GraphWidget::drawLevels(QPainter& p)
{
    const int OL=44, OR=6, OT=6, OB=20;
    int W = width()-OL-OR;
    int H = height()-OT-OB;
    QColor bgBar = isDark() ? QColor("#1f2937") : QColor("#e2e8f0");

    // Grille horizontale
    QColor gc = isDark() ? QColor("#1f2937") : QColor("#e2e8f0");
    p.setPen(QPen(gc, 0.5));
    for (int i=1; i<5; i++) p.drawLine(OL, OT+H*i/4, OL+W, OT+H*i/4);

    // Axe Y dB
    QColor tc = isDark() ? QColor("#6e7681") : QColor("#718096");
    p.setPen(tc);
    p.setFont(QFont("Courier New", 8));
    QStringList dbLbls = {"+20","+10"," 0","-10","-20"};
    for (int i=0; i<5; i++)
        p.drawText(2, OT + H*i/4 + 4, dbLbls[i]);

    static const QMap<QString,double> BASE = {
        {"h1",0.75},{"h2",0.62},{"h3",0.88},{"h4",0.12},{"a1",0.55},{"a2",0.45}
    };

    QStringList active;
    for (auto& k : COLORS.keys())
        if (m_voies.value(k, false)) active << k;
    if (active.isEmpty()) return;

    int barW = qMin(40, W/active.size() - 8);
    int spacing = (W - active.size()*barW) / (active.size()+1);

    for (int i = 0; i < active.size(); i++) {
        const QString& k = active[i];
        int x = OL + spacing + i*(barW+spacing);
        double lv = BASE.value(k,0.5) + 0.04*qSin(m_T*0.07+i);
        int bh = int(lv*(H-4));

        p.fillRect(x, OT+4, barW, H-4, bgBar);
        QColor col = m_colors.value(k);
        col.setAlphaF(0.9);
        p.fillRect(x, OT+H-bh, barW, bh, col);

        // Valeur dB
        double db = -20.0 + lv*40.0;
        p.setPen(m_colors.value(k));
        p.setFont(QFont("Courier New", 8, QFont::Bold));
        p.drawText(QRect(x, OT+H-bh-14, barW, 12),
                   Qt::AlignCenter, QString::number(db,'f',1));
        // Label voie
        p.drawText(QRect(x, height()-16, barW, 14),
                   Qt::AlignCenter, k.toUpper());
    }
}

// ─── Spectrogramme défilant ───────────────────────────────────────────────────
void GraphWidget::drawSpectrogram(QPainter& p)
{
    const int OL=44, OR=22, OT=6, OB=18;
    int W = width()-OL-OR;
    int H = height()-OT-OB;
    if (W<=0 || H<=0) return;

    int cols = m_spectroBuffer.size();
    int rows = m_spectroBuffer[0].size();

    // Dessin pixel par pixel
    for (int x=0; x<W; x++) {
        int ci = x * cols / W;
        for (int y=0; y<H; y++) {
            int ri = y * rows / H;
            double v = m_spectroBuffer[ci][ri];
            p.fillRect(OL+x, OT+H-1-y, 1, 1, thermalColor(v));
        }
    }

    // Ligne pic 38.4 kHz (freq=0.40 → y=0.40*H)
    int yPeak = OT + H - int(0.40*H);
    p.setPen(QPen(QColor(255,255,255,100), 1, Qt::DashLine));
    p.drawLine(OL, yPeak, OL+W, yPeak);
    p.setPen(QColor(255,255,255,200));
    p.setFont(QFont("Courier New", 8, QFont::Bold));
    p.drawText(OL+3, yPeak-2, "38.4kHz");

    // Axes
    QColor tc = isDark() ? QColor("#8b949e") : QColor("#4a5568");
    p.setPen(tc);
    p.setFont(QFont("Courier New", 8));
    // Y : fréquences
    p.drawText(2, OT+4,        "96k");
    p.drawText(2, OT+H/4+4,    "72k");
    p.drawText(2, OT+H/2+4,    "48k");
    p.drawText(2, OT+3*H/4+4,  "24k");
    p.drawText(2, OT+H-2,      " 0Hz");
    // X : temps
    p.drawText(OL,     height()-2, "−t");
    p.drawText(OL+W-8, height()-2, "0s");

    // Légende couleur (barre verticale à droite)
    int bx = OL+W+4, bw2=14;
    for (int y=0; y<H; y++) {
        double v = 1.0 - double(y)/H;
        p.fillRect(bx, OT+y, bw2, 1, thermalColor(v));
    }
    p.setPen(QColor("#6e7681"));
    p.setFont(QFont("Courier New", 7));
    p.drawText(bx, OT+6,     "Hi");
    p.drawText(bx, OT+H/2+4, "Mid");
    p.drawText(bx, OT+H-2,   "Lo");
    // Bordure légende
    p.setPen(QPen(QColor("#444c56"), 1));
    p.drawRect(bx, OT, bw2, H);
}
