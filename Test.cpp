void GraphWidget::drawSpectrogram(QPainter& p)
{
    int W = width(), H = height();
    QColor bg = isDark() ? QColor("#0d1117") : QColor("#f8fafc");
    p.fillRect(rect(), bg);

    // Axe fréquence vertical, temps horizontal
    // Simulation : gradient de couleur HSV selon intensité
    for (int x = 0; x < W; x++) {
        for (int y = 0; y < H; y++) {
            double freq  = 1.0 - double(y) / H;
            double time  = double(x) / W;

            // Simulation d'énergie acoustique
            double energy =
                0.6 * qExp(-qPow((freq - 0.40) / 0.12, 2))  // pic principal 38 kHz
              + 0.3 * qExp(-qPow((freq - 0.20) / 0.08, 2))  // harmonique
              + 0.15 * qExp(-qPow((freq - 0.65) / 0.06, 2)) // bruit
              + 0.04 * (QRandomGenerator::global()->generateDouble())
              + 0.1  * qSin(time * 12 + m_T * 0.02 + freq * 8) * 0.05;

            energy = qBound(0.0, energy, 1.0);

            // Palette thermique : noir → bleu → cyan → vert → jaune → rouge
            QColor col;
            if      (energy < 0.2) col = QColor::fromHsvF(0.67, 1.0, energy * 5.0);
            else if (energy < 0.4) col = QColor::fromHsvF(0.55, 1.0, 1.0);
            else if (energy < 0.6) col = QColor::fromHsvF(0.38, 1.0, 1.0);
            else if (energy < 0.8) col = QColor::fromHsvF(0.17, 1.0, 1.0);
            else                   col = QColor::fromHsvF(0.0,  1.0, 1.0);

            p.fillRect(x, y, 1, 1, col);
        }
    }

    // Ligne du pic principal (38.4 kHz)
    int yPeak = int(H * (1.0 - 0.40));
    p.setPen(QPen(QColor(255,255,255,120), 1, Qt::DashLine));
    p.drawLine(0, yPeak, W, yPeak);
    p.setPen(QColor(255,255,255,180));
    p.setFont(QFont("Courier New", 9));
    p.drawText(4, yPeak - 3, "38.4 kHz");

    // Axes labels
    p.setPen(QColor("#8b949e"));
    p.setFont(QFont("Courier New", 8));
    p.drawText(4, 12, "96 kHz");
    p.drawText(4, H - 4, "0 Hz");
}



void MainWindow::setupCharts()
{
    auto* grid = qobject_cast<QGridLayout*>(ui->chartsContainer->layout());

    auto makeBox = [](const QString& title, GraphWidget* gw) {
        auto* box = new QGroupBox(title);
        auto* l = new QVBoxLayout(box);
        l->setContentsMargins(4,18,4,4);
        l->addWidget(gw);
        return box;
    };

    m_graphTime   = new GraphWidget(GraphWidget::TimeDomain,  this);
    m_graphFFT    = new GraphWidget(GraphWidget::FFT,         this);
    m_graphLevels = new GraphWidget(GraphWidget::Levels,      this);
    m_graphSpec   = new GraphWidget(GraphWidget::Spectrogram, this);

    grid->addWidget(makeBox("Signal temporel — hydrophones", m_graphTime),   0, 0);
    grid->addWidget(makeBox("Spectrogramme — fréq. / temps", m_graphSpec),   0, 1);
    grid->addWidget(makeBox("FFT",                           m_graphFFT),    1, 0);
    grid->addWidget(makeBox("Niveaux — dB re 1µPa",         m_graphLevels), 1, 1);

    grid->setRowStretch(0, 1);
    grid->setRowStretch(1, 1);
}



switch (m_type) {
    case TimeDomain:  drawTimeDomain(p); break;
    case FFT:         drawFFT(p);        break;
    case Levels:      drawLevels(p);     break;
    case Spectrogram: drawSpectrogram(p); break;
}
