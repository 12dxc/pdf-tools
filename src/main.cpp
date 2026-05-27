#include <QApplication>
#include <QMainWindow>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("pdf-tools");
    app.setOrganizationName("pdf-tools");
    app.setApplicationVersion("1.0.0");

    QMainWindow window;
    window.setWindowTitle("pdf-tools");
    window.resize(1024, 720);

    auto *central = new QLabel("pdf-tools", &window);
    central->setAlignment(Qt::AlignCenter);
    window.setCentralWidget(central);

    window.show();
    return app.exec();
}
