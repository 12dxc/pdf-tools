#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QComboBox;
class QSplitter;
class QDockWidget;
class QStackedWidget;
class QToolBar;
class PdfView;
class TocWidget;
class SearchWidget;
class BookmarkWidget;

/**
 * @brief 应用程序主窗口
 *
 * 管理菜单栏、工具栏、状态栏和侧边面板，
 * 协调各子组件之间的交互。
 */
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
    void goToPage();
    void printDocument();
    void toggleDarkTheme();
    void about();

    PdfView *m_view = nullptr;

    // Side panel
    QDockWidget *m_sideDock = nullptr;
    QStackedWidget *m_sideStack = nullptr;
    QToolBar *m_sideToolBar = nullptr;
    TocWidget *m_tocWidget = nullptr;
    SearchWidget *m_searchWidget = nullptr;
    BookmarkWidget *m_bookmarkWidget = nullptr;

    // Status bar widgets
    QLabel *m_pageLabel = nullptr;
    QLabel *m_zoomLabel = nullptr;

    // Toolbar widgets
    QComboBox *m_zoomCombo = nullptr;

    // UI state
    bool m_darkTheme = false;
    // Current file
    QString m_currentPath;

    // Recent files
    QStringList m_recentFiles;
    static constexpr int MaxRecentFiles = 5;
};

#endif // MAINWINDOW_H
