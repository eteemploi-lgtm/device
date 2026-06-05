void MainWindow::onConfigChanged(const SystemConfig& cfg)
{
    // Mettre à jour UNIQUEMENT la barre des voies
    // sans toucher aux graphiques en cours
    auto* layout = qobject_cast<QHBoxLayout*>(ui->voieBar->layout());
    if(!layout) return;

    // Sauvegarder l'état actuel des voies
    QMap<QString,bool> savedVoies = m_voies;

    // Nettoyer la barre
    QLayoutItem* item;
    while((item=layout->takeAt(0))!=nullptr){
        if(item->widget()) item->widget()->deleteLater();
        delete item;
    }
    m_voieBtns.clear();

    // Couleurs
    QStringList hColors={"#00d4ff","#00e676","#ffaa00",
                          "#ff6b9d","#ff9944","#44ffaa"};
    QStringList aColors={"#c792ea","#89ddff","#ffcc44","#44ccff"};

    struct VoieDef { QString key,label,color; };
    QList<VoieDef> hydros, amplis;

    for(int i=0;i<cfg.hydrophones.size();i++)
        hydros.append({QString("h%1").arg(i+1),
                       QString("H%1").arg(i+1),
                       hColors[i%hColors.size()]});
    for(int i=0;i<cfg.amplifiers.size();i++)
        amplis.append({QString("a%1").arg(i+1),
                       QString("A%1").arg(i+1),
                       aColors[i%aColors.size()]});

    auto addLbl=[&](const QString& t){
        auto* l=new QLabel(t);
        l->setStyleSheet("color:#6e7681;font-size:13px;font-weight:bold;");
        layout->addWidget(l);
    };
    auto addBtn=[&](const VoieDef& d){
        auto* btn=new QPushButton(d.label);
        btn->setCheckable(true);

        // Restaurer l'état précédent si la voie existait déjà
        bool wasOn = savedVoies.value(d.key, true);
        btn->setChecked(wasOn);
        m_voies[d.key] = wasOn;

        btn->setFixedHeight(26); btn->setMinimumWidth(44);
        btn->setStyleSheet(QString(
            "QPushButton{border:1px solid %1;color:%1;background:transparent;"
            "font-size:13px;font-weight:bold;border-radius:2px;padding:2px 8px;}"
            "QPushButton:checked{background:%2;}"
            "QPushButton:!checked{color:#444c56;border-color:#444c56;}"
        ).arg(d.color,d.color+"22"));
        btn->setProperty("voieKey",d.key);
        connect(btn,&QPushButton::toggled,this,&MainWindow::onToggleVoie);
        layout->addWidget(btn);
        m_voieBtns[d.key]=btn;
    };
    auto addSep=[&](){
        auto* s=new QFrame(); s->setFrameShape(QFrame::VLine);
        s->setStyleSheet("color:#30363d;"); layout->addWidget(s);
    };

    if(!hydros.isEmpty()){ addLbl("HYDROS :"); for(auto& d:hydros) addBtn(d); }
    if(!amplis.isEmpty()){ addSep(); addLbl("AMPLIS :"); for(auto& d:amplis) addBtn(d); }
    addSep();
    auto* info=new QLabel(
        "Fs <span style='color:#00d4ff;font-weight:bold'>96 kHz</span>"
        " &nbsp;|&nbsp; MLI "
        "<span style='color:#00e676;font-weight:bold'>38.4 kHz</span>");
    info->setTextFormat(Qt::RichText);
    info->setStyleSheet("font-size:13px;color:#8b949e;");
    layout->addWidget(info);
    layout->addStretch();

    // Mettre à jour les graphiques SANS les recréer
    // juste mettre à jour les voies actives
    for(auto& d : hydros){
        bool on = m_voies.value(d.key, true);
        if(m_graphTime)   m_graphTime->setVoieActive(d.key, on);
        if(m_graphLevels) m_graphLevels->setVoieActive(d.key, on);
        if(m_graphSpec)   m_graphSpec->setVoieActive(d.key, on);
    }

    appendLog("ok", QString("Config mise à jour — %1 hydros, %2 amplis")
              .arg(cfg.hydrophones.size())
              .arg(cfg.amplifiers.size()));
}
