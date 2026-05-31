#include <QApplication>
#include <QScreen>
#include "LoginWindow.h"
#include "MainWindow.h"
#include "StyleManager.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Acoustimeter");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Acoustique");

    // Thème initial : sombre
    StyleManager::instance().setTheme(StyleManager::Dark);
    app.setStyleSheet(StyleManager::instance().appStyleSheet());

    // ── Écran de connexion ──────────────────────────────────────────────────
    LoginWindow login;
    login.show();

    ConnectionConfig cfg;

    QObject::connect(&login, &LoginWindow::connectionEstablished,
                     [&cfg](const ConnectionConfig& c){ cfg = c; });

    if (login.exec() != QDialog::Accepted)
        return 0;

    // ── Fenêtre principale (Supervision) — écran primaire ──────────────────
    MainWindow mainWin(cfg);
    mainWin.setStyleSheet(StyleManager::instance().mainStyleSheet());
    mainWin.showMaximized();

    // ── Fenêtre Configuration — écran secondaire si disponible ─────────────
    // (La ConfigWindow est instanciée à la demande depuis MainWindow::openConfig)
    // En production avec deux écrans :
    //
    // if (QApplication::screens().size() > 1) {
    //     auto* screen2 = QApplication::screens().at(1);
    //     configWin.setGeometry(screen2->geometry());
    //     configWin.showMaximized();
    // }

    return app.exec();
}
