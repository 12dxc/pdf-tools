#include <QApplication>
#include <QSettings>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("pdf-tools");
    app.setOrganizationName("pdf-tools");
    app.setApplicationVersion("1.0.0");

    // 使用 INI 格式存储设置，跨平台兼容
    QSettings::setDefaultFormat(QSettings::IniFormat);

    MainWindow window;

    // 恢复上次窗口位置和大小
    QSettings settings;
    if (settings.contains("window/geometry"))
        window.restoreGeometry(settings.value("window/geometry").toByteArray());
    if (settings.contains("window/state"))
        window.restoreState(settings.value("window/state").toByteArray());

    window.show();
    return app.exec();
}
