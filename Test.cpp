void MainWindow::exportCSV()
{
    QString path = QFileDialog::getSaveFileName(
        this, "Exporter données", "acoustimeter_data.csv",
        "CSV (*.csv);;Tous (*.*)");
    if(path.isEmpty()) return;

    QFile f(path);
    if(!f.open(QIODevice::WriteOnly|QIODevice::Text)){
        appendLog("err","Export CSV échoué — impossible d'ouvrir le fichier");
        return;
    }
    QTextStream out(&f);

    // En-tête
    out << "# ACOUSTIMETER — Export données\n";
    out << "# Date : " << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << "\n";
    out << "# Châssis : " << m_connConfig.chassis << " · NI PXIe-8301\n\n";

    // Niveaux hydrophones (simulation)
    out << "Voie,Niveau_dB,Statut\n";
    QStringList voies={"H1","H2","H3","H4","A1","A2"};
    QList<double> niveaux={14.2,-3.2,6.8,-10.5,17.1,-5.3};
    QStringList statuts={"ACQ","ACQ","BRUIT↑","DÉCO","ACTIF","STANDBY"};
    for(int i=0;i<voies.size();i++)
        out<<voies[i]<<","<<niveaux[i]<<","<<statuts[i]<<"\n";

    out<<"\n";

    // Températures
    out<<"Sonde,Type,Temperature_C\n";
    out<<"T1,K,24.3\nT2,J,47.8\nT3,K,21.1\n\n";

    // Tensions
    out<<"Voie,Tension_V\n";
    out<<"V_Bus_48V,48.2\nV_Alim_12V,12.1\n\n";

    // Courants
    out<<"Voie,Courant_A\n";
    out<<"I_AMP1,3.8\nI_AMP2,0.4\n\n";

    // Mesure Sv
    out<<"Parametre,Valeur\n";
    out<<"Gite_deg,127.5\nSite_deg,32.0\nSv_moyen_dB,-18.4\n";

    f.close();
    appendLog("ok", "Export CSV → " + QFileInfo(path).fileName());
}


#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>


void exportCSV();

auto* aExport = mFile->addAction("Exporter données CSV...");
connect(aExport, &QAction::triggered, this, &MainWindow::exportCSV);

