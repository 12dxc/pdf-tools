#include <QApplication>
#include <QSettings>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("pdf-tools");
    app.setOrganizationName("pdf-tools");
    app.setApplicationVersion("1.0.0");

    QSettings::setDefaultFormat(QSettings::IniFormat);

    MainWindow window;

    QSettings settings;
    if (settings.contains("window/geometry"))
        window.restoreGeometry(settings.value("window/geometry").toByteArray());
    if (settings.contains("window/state"))
        window.restoreState(settings.value("window/state").toByteArray());

    window.show();
    return app.exec();
}
