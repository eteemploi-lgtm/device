void MainWindow::exportCSV()
{
    QString path = QFileDialog::getSaveFileName(
        this, "Exporter données", "acoustimeter_data.csv",
        "CSV (*.csv);;Tous (*.*)");
    if(path.isEmpty()) return;

    QFile f(path);
    if(!f.open(QIODevice::WriteOnly|QIODevice::Text)){
        QMessageBox::warning(this,"Erreur","Impossible d'ouvrir le fichier.");
        appendLog("err","Export CSV échoué");
        return;
    }

    QTextStream out(&f);
    out << "# ACOUSTIMETER — Export données\n";
    out << "# Date : "
        << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << "\n";
    out << "# Chassis : " << m_connConfig.chassis << " NI PXIe-8301\n\n";

    out << "Voie,Niveau_dB,Statut\n";
    out << "H1,14.2,ACQ\n";
    out << "H2,-3.2,ACQ\n";
    out << "H3,6.8,BRUIT\n";
    out << "H4,-10.5,DECO\n";
    out << "A1,17.1,ACTIF\n";
    out << "A2,-5.3,STANDBY\n\n";

    out << "Sonde,Type,Temperature_C\n";
    out << "T1,K,24.3\n";
    out << "T2,J,47.8\n";
    out << "T3,K,21.1\n\n";

    out << "Voie,Tension_V\n";
    out << "V_Bus_48V,48.2\n";
    out << "V_Alim_12V,12.1\n\n";

    out << "Voie,Courant_A\n";
    out << "I_AMP1,3.8\n";
    out << "I_AMP2,0.4\n\n";

    out << "Parametre,Valeur\n";
    out << "Gite_deg,127.5\n";
    out << "Site_deg,32.0\n";
    out << "Sv_moyen_dB,-18.4\n";

    f.close();

    appendLog("ok", "Export CSV OK → " + QFileInfo(path).fileName());
    QMessageBox::information(this, "Export réussi",
        "Fichier exporté :\n" + path);
}


