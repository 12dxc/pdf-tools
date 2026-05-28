#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QComboBox;
class QSplitter;
class QDockWidget;
class QStackedWidget;
class PdfView;
class TocWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void createMenuBar();
    void createToolBar();
    void createStatusBar();
    void createCentralWidget();
    void createSidePanel();
    void setupConnections();

    void openFile();
    void openRecentFile();
    void updateRecentFiles();
    void about();

    PdfView *m_view = nullptr;
    QSplitter *m_splitter = nullptr;

    // Side panel
    QDockWidget *m_sideDock = nullptr;
    QStackedWidget *m_sideStack = nullptr;
    TocWidget *m_tocWidget = nullptr;

    // Status bar widgets
    QLabel *m_pageLabel = nullptr;
    QLabel *m_zoomLabel = nullptr;

    // Toolbar widgets
    QComboBox *m_zoomCombo = nullptr;

    // Recent files
    QStringList m_recentFiles;
    static constexpr int MaxRecentFiles = 5;
};

#endif // MAINWINDOW_H
