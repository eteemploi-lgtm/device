#include "GraphWidget.h"
#include "StyleManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QRandomGenerator>
#include <QtMath>

static const QMap<QString,QColor> COLORS = {
    {"h1",QColor("#00d4ff")},{"h2",QColor("#00e676")},
    {"h3",QColor("#ffaa00")},{"h4",QColor("#ff6b9d")},
    {"a1",QColor("#c792ea")},{"a2",QColor("#89ddff")},
};

GraphWidget::GraphWidget(GraphType type, QWidget* parent)
    : QWidget(parent), m_type(type)
{
    for (auto& k : COLORS.keys()) m_voies[k] = true;
    m_colors = COLORS;
    m_spectroBuffer.assign(300, std::vector<double>(100, 0.0));
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GraphWidget::onTimer);
    m_timer->start(33);
    setMinimumSize(200, 80);
}

void GraphWidget::setVoieActive(const QString& key, bool active) { m_voies[key]=active; }
bool GraphWidget::isDark() const { return StyleManager::instance().isDark(); }

void GraphWidget::onTimer() {
    m_T += 1.0;
    if (m_type==Spectrogram) updateSpectroBuffer();
    update();
}

void GraphWidget::updateSpectroBuffer() {
    int cols=m_spectroBuffer.size(), rows=m_spectroBuffer[0].size();
    for (int x=0;x<cols-1;x++) m_spectroBuffer[x]=m_spectroBuffer[x+1];
    for (int y=0;y<rows;y++) {
        double f=double(y)/rows;
        double e=0.75*qExp(-qPow((f-0.40)/0.10,2))
                +0.35*qExp(-qPow((f-0.20)/0.07,2))
                +0.20*qExp(-qPow((f-0.65)/0.05,2))
                +0.05*QRandomGenerator::global()->generateDouble()
                +0.08*qSin(m_T*0.03+f*10.0);
        m_spectroBuffer[cols-1][y]=qBound(0.0,e,1.0);
    }
}

static QColor thermal(double v) {
    v=qBound(0.0,v,1.0);
    if      (v<0.20) return QColor::fromHsvF(0.67,1.0,v*5.0);
    else if (v<0.40) return QColor::fromHsvF(0.55,1.0,1.0);
    else if (v<0.60) return QColor::fromHsvF(0.38,1.0,1.0);
    else if (v<0.80) return QColor::fromHsvF(0.17,1.0,1.0);
    else             return QColor::fromHsvF(0.0, 1.0,1.0);
}

void GraphWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, m_type!=Levels&&m_type!=Spectrogram);
    p.fillRect(rect(), isDark()?QColor("#0d1117"):QColor("#f8fafc"));
    switch(m_type){
        case TimeDomain:  drawTimeDomain(p);  break;
        case FFT:         drawFFT(p);         break;
        case Levels:      drawLevels(p);      break;
        case Spectrogram: drawSpectrogram(p); break;
        case MiniPlot:    drawMiniPlot(p);    break;
    }
}

void GraphWidget::drawGrid(QPainter& p,int cols,int rows,int oL,int oR,int oT,int oB){
    QColor gc=isDark()?QColor("#1a2030"):QColor("#e2e8f0");
    p.setPen(QPen(gc,0.5));
    int W=width()-oL-oR, H=height()-oT-oB;
    for(int i=1;i<rows;i++) p.drawLine(oL,oT+H*i/rows,oL+W,oT+H*i/rows);
    for(int i=1;i<cols;i++) p.drawLine(oL+W*i/cols,oT,oL+W*i/cols,oT+H);
}

// ─── Signal temporel — voies bien séparées ───────────────────────────────────
void GraphWidget::drawTimeDomain(QPainter& p) {
    const int OL=38,OR=6,OT=8,OB=18;
    drawGrid(p,8,4,OL,OR,OT,OB);
    int W=width()-OL-OR, H=height()-OT-OB;

    // Compter les voies actives hydro
    QList<int> activeVoies;
    for(int vi=1;vi<=4;vi++)
        if(m_voies.value(QString("h%1").arg(vi),false)) activeVoies<<vi;
    if(activeVoies.isEmpty()) return;

    int nv = activeVoies.size();
    double slotH = double(H) / nv;   // hauteur allouée par voie
    double ampFrac = 0.35;           // amplitude = 35% du slot

    // Labels axes
    QColor tc=isDark()?QColor("#6e7681"):QColor("#718096");
    p.setPen(tc); p.setFont(QFont("Courier New",8));
    QStringList xl={"0","0.25","0.50","0.75","1ms"};
    for(int i=0;i<xl.size();i++)
        p.drawText(OL+W*i/(xl.size()-1)-10,height()-2,xl[i]);

    static const double ph[]={0,0.8,1.6,2.4};
    static const double fr[]={1.0,1.3,0.9,1.6};

    for(int idx=0;idx<nv;idx++){
        int vi = activeVoies[idx];
        QString key=QString("h%1").arg(vi);
        QColor col=m_colors.value(key);

        // Centre vertical de cette voie
        double yCenter = OT + slotH*(idx+0.5);
        double amplitude = slotH * ampFrac;

        p.setPen(QPen(col,1.5));
        QPainterPath path;
        for(int x=0;x<W;x++){
            double tt=(double(x)/W)*6*M_PI + m_T*0.04 + ph[vi-1];
            double y = yCenter - qSin(tt*fr[vi-1]) * amplitude
                       * (0.8 + 0.2*qSin(m_T*0.015+vi));
            x==0?path.moveTo(OL+x,y):path.lineTo(OL+x,y);
        }
        p.drawPath(path);

        // Label voie + valeur
        p.setPen(col);
        p.setFont(QFont("Courier New",9,QFont::Bold));
        p.drawText(OL+4, int(yCenter - amplitude - 2), key.toUpper());

        // Ligne de base pointillée
        p.setPen(QPen(col.darker(200), 0.5, Qt::DotLine));
        p.drawLine(OL, int(yCenter), OL+W, int(yCenter));
    }
}

// ─── FFT — pic propre, bruit réduit ──────────────────────────────────────────
void GraphWidget::drawFFT(QPainter& p) {
    const int OL=40,OR=6,OT=6,OB=18;
    drawGrid(p,8,4,OL,OR,OT,OB);
    int W=width()-OL-OR, H=height()-OT-OB;

    QColor tc=isDark()?QColor("#6e7681"):QColor("#718096");
    p.setPen(tc); p.setFont(QFont("Courier New",8));
    p.drawText(2,OT+4," 0dB");
    p.drawText(2,OT+H/4+4,"-20");
    p.drawText(2,OT+H/2+4,"-40");
    p.drawText(2,OT+3*H/4+4,"-60");
    p.drawText(2,OT+H-2,"-80");
    QStringList fl={"0","12","24","36","48","60","72","84","96k"};
    for(int i=0;i<fl.size();i++)
        p.drawText(OL+W*i/(fl.size()-1)-8,height()-2,fl[i]);

    const int BINS=64;
    double bw=double(W)/BINS;

    for(int i=0;i<BINS;i++){
        // Bruit de fond faible
        double noise = QRandomGenerator::global()->generateDouble()*0.03;
        // Pics principaux
        double peak = 0.0;
        if(i==38) peak=0.92;       // 38.4 kHz — pic principal
        else if(i==19) peak=0.38;  // harmonique
        else if(i==57) peak=0.22;  // 2ème harmonique
        // Enveloppe douce autour du pic principal
        double env = qExp(-qPow((i-38.0)/14.0,2))*0.18;
        double val = qBound(0.0, peak+env+noise, 1.0);
        double bh  = val*H*0.92;

        QColor col;
        if(i==38)      col=QColor("#00d4ff");
        else if(i==19) col=QColor("#00e676");
        else if(i==57) col=QColor("#ffaa00");
        else           col=QColor(isDark()?"#2a3550":"#a0aec0");

        p.fillRect(QRectF(OL+i*bw, OT+H-bh, bw-1, bh), col);
    }

    // Étiquette pic
    p.setPen(QColor("#00d4ff"));
    p.setFont(QFont("Courier New",9,QFont::Bold));
    p.drawText(OL+int(38*W/64)-4, OT+int(H*0.06), "38.4 kHz");
}

// ─── Niveaux — barres bien visibles ──────────────────────────────────────────
void GraphWidget::drawLevels(QPainter& p) {
    const int OL=44,OR=8,OT=20,OB=22;
    int W=width()-OL-OR, H=height()-OT-OB;
    if(W<=0||H<=0) return;

    QColor bgBar=isDark()?QColor("#1a2030"):QColor("#e2e8f0");
    // Grille horizontale
    QColor gc=isDark()?QColor("#1a2030"):QColor("#e2e8f0");
    p.setPen(QPen(gc,0.5));
    for(int i=0;i<=4;i++) p.drawLine(OL,OT+H*i/4,OL+W,OT+H*i/4);

    // Axe Y dB
    QColor tc=isDark()?QColor("#6e7681"):QColor("#718096");
    p.setPen(tc); p.setFont(QFont("Courier New",8));
    QStringList db={"+20","+10"," 0","-10","-20"};
    for(int i=0;i<5;i++) p.drawText(2,OT+H*i/4+4,db[i]);

    // Voies actives
    QStringList active;
    for(auto& k : {"h1","h2","h3","h4","a1","a2"})
        if(m_voies.value(QString(k),false)) active<<QString(k);
    if(active.isEmpty()) return;

    // Valeurs de base simulées
    static const QMap<QString,double> BASE={
        {"h1",0.72},{"h2",0.58},{"h3",0.85},{"h4",0.25},
        {"a1",0.60},{"a2",0.42}};

    int n = active.size();
    int gap = 6;
    int totalGap = gap*(n+1);
    int barW = qMax(10, (W-totalGap)/n);
    barW = qMin(barW, 60);
    int totalBar = barW*n + gap*(n+1);
    int startX = OL + (W-totalBar)/2;

    for(int i=0;i<n;i++){
        const QString& k=active[i];
        int x = startX + gap + i*(barW+gap);
        double lv = BASE.value(k,0.5) + 0.06*qSin(m_T*0.07+i*1.3);
        lv = qBound(0.05, lv, 1.0);
        int bh = int(lv * H);

        // Fond de barre
        p.fillRect(x, OT, barW, H, bgBar);

        // Barre colorée avec dégradé
        QLinearGradient grad(0, OT+H-bh, 0, OT+H);
        QColor col = m_colors.value(k, QColor("#00d4ff"));
        grad.setColorAt(0, col);
        grad.setColorAt(1, col.darker(180));
        p.fillRect(x, OT+H-bh, barW, bh, grad);

        // Valeur dB au-dessus de la barre
        double dbVal = -20.0 + lv*40.0;
        p.setPen(col);
        p.setFont(QFont("Courier New",9,QFont::Bold));
        p.drawText(QRect(x-4, OT+H-bh-16, barW+8, 14),
                   Qt::AlignCenter,
                   QString::number(dbVal,'f',1));

        // Label voie en bas
        p.setFont(QFont("Courier New",9,QFont::Bold));
        p.drawText(QRect(x-4, OT+H+4, barW+8, 14),
                   Qt::AlignCenter, k.toUpper());
    }
}

// ─── Spectrogramme défilant ───────────────────────────────────────────────────
void GraphWidget::drawSpectrogram(QPainter& p) {
    const int OL=44,OR=22,OT=6,OB=18;
    int W=width()-OL-OR, H=height()-OT-OB;
    if(W<=0||H<=0) return;
    int cols=m_spectroBuffer.size(), rows=m_spectroBuffer[0].size();
    for(int x=0;x<W;x++){
        int ci=x*cols/W;
        for(int y=0;y<H;y++)
            p.fillRect(OL+x,OT+H-1-y,1,1,thermal(m_spectroBuffer[ci][y*rows/H]));
    }
    int yPk=OT+H-int(0.40*H);
    p.setPen(QPen(QColor(255,255,255,100),1,Qt::DashLine));
    p.drawLine(OL,yPk,OL+W,yPk);
    p.setPen(QColor(255,255,255,200));
    p.setFont(QFont("Courier New",8,QFont::Bold));
    p.drawText(OL+3,yPk-2,"38.4kHz");
    QColor tc=isDark()?QColor("#8b949e"):QColor("#4a5568");
    p.setPen(tc); p.setFont(QFont("Courier New",8));
    p.drawText(2,OT+4,"96k");
    p.drawText(2,OT+H/4+4,"72k");
    p.drawText(2,OT+H/2+4,"48k");
    p.drawText(2,OT+3*H/4+4,"24k");
    p.drawText(2,OT+H-2," 0Hz");
    p.drawText(OL,height()-2,"−t");
    p.drawText(OL+W-8,height()-2,"0s");
    int bx=OL+W+4, bw2=14;
    for(int y=0;y<H;y++)
        p.fillRect(bx,OT+y,bw2,1,thermal(1.0-double(y)/H));
    p.setPen(QColor("#6e7681"));
    p.setFont(QFont("Courier New",7));
    p.drawText(bx,OT+6,"Hi");
    p.drawText(bx,OT+H/2+4,"Mid");
    p.drawText(bx,OT+H-2,"Lo");
    p.setPen(QPen(QColor("#444c56"),1));
    p.drawRect(bx,OT,bw2,H);
}

// ─── MiniPlot sidebar ────────────────────────────────────────────────────────
void GraphWidget::pushMiniValue(double v) {
    m_miniData.push_back(v);
    while((int)m_miniData.size()>m_miniMaxPts) m_miniData.pop_front();
}

void GraphWidget::drawMiniPlot(QPainter& p) {
    const int OL=38,OR=4,OT=18,OB=16;
    int W=width()-OL-OR, H=height()-OT-OB;
    if(W<=0||H<=0||m_miniData.empty()) return;

    // Fond
    QColor gc=isDark()?QColor("#1a2030"):QColor("#e2e8f0");
    p.setPen(QPen(gc,0.5));
    for(int i=1;i<4;i++) p.drawLine(OL,OT+H*i/4,OL+W,OT+H*i/4);

    double cur=m_miniData.back();

    // Valeur courante — grande et colorée
    p.setPen(m_miniColor);
    p.setFont(QFont("Courier New",12,QFont::Bold));
    p.drawText(QRect(0,2,OL-3,OT+2),
               Qt::AlignRight|Qt::AlignVCenter,
               QString::number(cur,'f',1));

    // Unité
    p.setPen(isDark()?QColor("#8b949e"):QColor("#718096"));
    p.setFont(QFont("Courier New",9));
    p.drawText(2,OT-3,m_miniUnit);

    // Label
    p.setPen(isDark()?QColor("#6e7681"):QColor("#a0aec0"));
    p.setFont(QFont("Courier New",8));
    p.drawText(OL,height()-2,m_miniLabel);

    // Min/Max axe Y
    p.setFont(QFont("Courier New",7));
    p.drawText(2,OT+7,   QString::number(m_miniHi,'f',0));
    p.drawText(2,OT+H-1, QString::number(m_miniLo,'f',0));

    // Courbe défilante
    p.setPen(QPen(m_miniColor,1.5));
    QPainterPath path;
    int n=m_miniData.size();
    for(int i=0;i<n;i++){
        double nx=OL+double(i)/(m_miniMaxPts-1)*W;
        double ny=OT+H-(m_miniData[i]-m_miniLo)/(m_miniHi-m_miniLo)*H;
        ny=qBound(double(OT),ny,double(OT+H));
        i==0?path.moveTo(nx,ny):path.lineTo(nx,ny);
    }
    p.drawPath(path);

    // Remplissage sous la courbe
    if(n>1){
        QPainterPath fill=path;
        fill.lineTo(OL+double(n-1)/(m_miniMaxPts-1)*W,OT+H);
        fill.lineTo(OL,OT+H);
        fill.closeSubpath();
        QColor fc=m_miniColor; fc.setAlphaF(0.15);
        p.fillPath(fill,fc);
    }

    // Point courant
    double cx=OL+double(n-1)/(m_miniMaxPts-1)*W;
    double cy=OT+H-(cur-m_miniLo)/(m_miniHi-m_miniLo)*H;
    cy=qBound(double(OT),cy,double(OT+H));
    p.setPen(Qt::NoPen); p.setBrush(m_miniColor);
    p.drawEllipse(QPointF(cx,cy),4,4);
}
