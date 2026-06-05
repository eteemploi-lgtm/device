void MainWindow::openConfig()
{
    try {
        if(!m_configWindow){
            m_configWindow = new ConfigWindow(nullptr);
            m_configWindow->setStyleSheet(
                StyleManager::instance().configStyleSheet());
            m_configWindow->setAttribute(Qt::WA_DeleteOnClose, false);
            m_configWindow->setWindowFlags(
                Qt::Window |
                Qt::WindowMinimizeButtonHint |
                Qt::WindowMaximizeButtonHint |
                Qt::WindowCloseButtonHint);
            m_configWindow->resize(1100, 750);

            // Centrer sur l'écran disponible
            QScreen* screen = QApplication::primaryScreen();
            if(QApplication::screens().size() > 1)
                screen = QApplication::screens().at(1);
            QRect sg = screen->availableGeometry();
            m_configWindow->move(
                sg.x() + (sg.width()  - 1100) / 2,
                sg.y() + (sg.height() - 750)  / 2);

            connect(m_configWindow, &ConfigWindow::configChanged,
                    this, &MainWindow::onConfigChanged);
            connect(m_configWindow, &QObject::destroyed,
                    this, [this](){ m_configWindow = nullptr; });
        }

        // Toujours lever la fenêtre existante, ne jamais en créer une 2ème
        m_configWindow->show();
        m_configWindow->raise();
        m_configWindow->activateWindow();

    } catch(...){ qDebug() << "Erreur ConfigWindow"; }
}


MainWindow::~MainWindow() {
    if (m_configWindow) { m_configWindow->close(); m_configWindow = nullptr; }
    delete ui;
}
