#include "MainWindow.h"
#include "PdfView.h"
#include "TocWidget.h"
#include "SearchWidget.h"
#include "BookmarkWidget.h"

#include <QPdfDocument>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSplitter>
#include <QDockWidget>
#include <QStackedWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QCloseEvent>
#include <QSettings>
#include <QApplication>
#include <QAction>
#include <QStyle>
#include <QStyleFactory>
#include <QPalette>
#include <QFileInfo>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("pdf-tools");
    resize(1024, 720);

    createMenuBar();
    createToolBar();
    createStatusBar();
    createSidePanel();
    createCentralWidget();
    setupConnections();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_currentPath.isEmpty())
        m_bookmarkWidget->saveBookmarks(m_currentPath);
    QSettings settings;
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        for (const auto &url : event->mimeData()->urls()) {
            if (url.toLocalFile().endsWith(".pdf", Qt::CaseInsensitive)) {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    for (const auto &url : event->mimeData()->urls()) {
        QString path = url.toLocalFile();
        if (path.endsWith(".pdf", Qt::CaseInsensitive)) {
            m_view->loadFile(path);
            break;
        }
    }
}

void MainWindow::createMenuBar()
{
    // File menu
    auto *fileMenu = menuBar()->addMenu(tr("&File"));

    auto *openAction = fileMenu->addAction(tr("&Open..."));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    m_recentFiles.clear();
    for (int i = 0; i < MaxRecentFiles; ++i) {
        auto *action = fileMenu->addAction(QString());
        action->setVisible(false);
        connect(action, &QAction::triggered, this, &MainWindow::openRecentFile);
    }

    fileMenu->addSeparator();

    auto *printAction = fileMenu->addAction(tr("&Print..."));
    printAction->setShortcut(QKeySequence::Print);
    printAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    connect(printAction, &QAction::triggered, this, &MainWindow::printDocument);

    fileMenu->addSeparator();

    auto *closeAction = fileMenu->addAction(tr("&Close"));
    closeAction->setShortcut(QKeySequence::Close);
    connect(closeAction, &QAction::triggered, m_view, &PdfView::closeDocument);

    auto *exitAction = fileMenu->addAction(tr("E&xit"));
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // View menu
    auto *viewMenu = menuBar()->addMenu(tr("&View"));

    auto *gotoAction = viewMenu->addAction(tr("&Go to Page..."));
    gotoAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    connect(gotoAction, &QAction::triggered, this, &MainWindow::goToPage);

    viewMenu->addSeparator();

    auto *zoomInAction = viewMenu->addAction(tr("Zoom &In"));
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction, &QAction::triggered, this, [this] {
        m_view->setZoom(m_view->zoomFactor() + 15.0);
    });

    auto *zoomOutAction = viewMenu->addAction(tr("Zoom &Out"));
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction, &QAction::triggered, this, [this] {
        m_view->setZoom(m_view->zoomFactor() - 15.0);
    });

    viewMenu->addSeparator();

    auto *fitWidthAction = viewMenu->addAction(tr("Fit &Width"));
    connect(fitWidthAction, &QAction::triggered, m_view, &PdfView::fitToWidth);

    auto *fitPageAction = viewMenu->addAction(tr("Fit &Page"));
    connect(fitPageAction, &QAction::triggered, m_view, &PdfView::fitToPage);

    viewMenu->addSeparator();

    auto *selectModeAction = viewMenu->addAction(tr("Text &Select"));
    selectModeAction->setCheckable(true);
    selectModeAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_S));
    connect(selectModeAction, &QAction::toggled, m_view, &PdfView::setSelectMode);

    viewMenu->addSeparator();

    auto *searchAction = viewMenu->addAction(tr("&Search..."));
    searchAction->setShortcut(QKeySequence::Find);
    connect(searchAction, &QAction::triggered, this, [this] {
        m_sideDock->show();
        auto actions = m_sideToolBar->actions();
        if (actions.size() >= 2) {
            actions[1]->trigger();
        }
    });

    viewMenu->addSeparator();

    auto *toggleTocAction = viewMenu->addAction(tr("Toggle &Side Panel"));
    toggleTocAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T));
    connect(toggleTocAction, &QAction::triggered, this, [this] {
        m_sideDock->setVisible(!m_sideDock->isVisible());
    });

    auto *darkThemeAction = viewMenu->addAction(tr("&Dark Theme"));
    darkThemeAction->setCheckable(true);
    darkThemeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_D));
    connect(darkThemeAction, &QAction::triggered, this, &MainWindow::toggleDarkTheme);

    // Help menu
    auto *helpMenu = menuBar()->addMenu(tr("&Help"));
    auto *aboutAction = helpMenu->addAction(tr("&About pdf-tools"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createToolBar()
{
    auto *toolbar = addToolBar(tr("Main"));
    toolbar->setIconSize(QSize(20, 20));
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    auto *openAction = toolbar->addAction(
        style()->standardIcon(QStyle::SP_DialogOpenButton), tr("Open"));
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    toolbar->addSeparator();

    auto *prevAction = toolbar->addAction(
        style()->standardIcon(QStyle::SP_ArrowLeft), tr("Prev"));
    connect(prevAction, &QAction::triggered, m_view, &PdfView::goPrevPage);

    auto *nextAction = toolbar->addAction(
        style()->standardIcon(QStyle::SP_ArrowRight), tr("Next"));
    connect(nextAction, &QAction::triggered, m_view, &PdfView::goNextPage);

    toolbar->addSeparator();

    auto *selectAction = toolbar->addAction(
        style()->standardIcon(QStyle::SP_FileDialogContentsView), tr("Select"));
    selectAction->setCheckable(true);
    selectAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_S));
    connect(selectAction, &QAction::toggled, m_view, &PdfView::setSelectMode);

    toolbar->addSeparator();

    auto *bookmarkAction = toolbar->addAction(
        style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Bookmark"));
    bookmarkAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
    connect(bookmarkAction, &QAction::triggered, this, [this] {
        QString title = tr("Page %1").arg(m_view->currentPage() + 1);
        m_bookmarkWidget->addBookmark(title, m_view->currentPage());
        m_bookmarkWidget->saveBookmarks(m_currentPath);
    });

    toolbar->addSeparator();

    m_zoomCombo = new QComboBox(this);
    m_zoomCombo->setEditable(true);
    m_zoomCombo->setInsertPolicy(QComboBox::NoInsert);
    QStringList zoomLevels;
    for (int z = 25; z <= 500; z += 25)
        zoomLevels << QString::number(z) + "%";
    m_zoomCombo->addItems(zoomLevels);
    m_zoomCombo->setCurrentText("100%");
    m_zoomCombo->setFixedWidth(90);
    toolbar->addWidget(m_zoomCombo);
}

void MainWindow::createStatusBar()
{
    m_pageLabel = new QLabel(tr("Page 1 / 1"), this);
    statusBar()->addPermanentWidget(m_pageLabel);

    m_zoomLabel = new QLabel(tr("100%"), this);
    statusBar()->addPermanentWidget(m_zoomLabel);
}

void MainWindow::createCentralWidget()
{
    m_view = new PdfView(this);
    setCentralWidget(m_view);
}

void MainWindow::createSidePanel()
{
    m_sideDock = new QDockWidget(tr("Navigation"), this);
    m_sideDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    m_sideDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    auto *sideContainer = new QWidget;
    auto *sideLayout = new QVBoxLayout(sideContainer);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    m_sideToolBar = new QToolBar;
    m_sideToolBar->setIconSize(QSize(16, 16));
    m_sideToolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    sideLayout->addWidget(m_sideToolBar);

    m_sideStack = new QStackedWidget;

    m_tocWidget = new TocWidget;
    m_sideStack->addWidget(m_tocWidget);

    m_searchWidget = new SearchWidget;
    m_sideStack->addWidget(m_searchWidget);

    m_bookmarkWidget = new BookmarkWidget;
    m_sideStack->addWidget(m_bookmarkWidget);

    sideLayout->addWidget(m_sideStack, 1);
    m_sideDock->setWidget(sideContainer);
    addDockWidget(Qt::LeftDockWidgetArea, m_sideDock);

    auto *tocAction = m_sideToolBar->addAction(tr("TOC"));
    tocAction->setCheckable(true);
    tocAction->setChecked(true);

    auto *searchAction = m_sideToolBar->addAction(tr("Search"));
    searchAction->setCheckable(true);

    auto *bookmarkAction = m_sideToolBar->addAction(tr("Bookmarks"));
    bookmarkAction->setCheckable(true);

    auto switchTo = [this](int index) {
        m_sideStack->setCurrentIndex(index);
        auto actions = m_sideToolBar->actions();
        for (int i = 0; i < actions.size(); ++i)
            actions[i]->setChecked(i == index);
        if (index == 1)
            m_searchWidget->focusSearch();
    };

    connect(tocAction, &QAction::triggered, this, [switchTo] { switchTo(0); });
    connect(searchAction, &QAction::triggered, this, [switchTo] { switchTo(1); });
    connect(bookmarkAction, &QAction::triggered, this, [switchTo] { switchTo(2); });
}

void MainWindow::setupConnections()
{
    connect(m_view, &PdfView::pageChanged, this, [this](int page) {
        m_pageLabel->setText(tr("Page %1 / %2").arg(page + 1).arg(m_view->pageCount()));
    });

    connect(m_view, &PdfView::zoomChanged, this, [this](double percent) {
        QString text = QString::number(qRound(percent)) + "%";
        m_zoomLabel->setText(text);
        m_zoomCombo->setCurrentText(text);
    });

    connect(m_zoomCombo, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        QString num = text;
        num.remove('%');
        bool ok;
        double val = num.toDouble(&ok);
        if (ok && val >= 25.0 && val <= 500.0)
            m_view->setZoom(val);
    });

    connect(m_view, &PdfView::documentLoaded, this, [this](const QString &path) {
        setWindowTitle(tr("pdf-tools - %1").arg(QFileInfo(path).fileName()));

        m_currentPath = path;
        m_tocWidget->setDocument(m_view->document());
        m_searchWidget->setDocument(m_view->document());
        m_bookmarkWidget->loadBookmarks(path);
        m_sideDock->show();

        QSettings settings;
        QStringList files = settings.value("recent/files").toStringList();
        files.removeAll(path);
        files.prepend(path);
        while (files.size() > MaxRecentFiles)
            files.removeLast();
        settings.setValue("recent/files", files);
        m_recentFiles = files;
        updateRecentFiles();
    });

    connect(m_view, &PdfView::documentClosed, this, [this] {
        if (!m_currentPath.isEmpty())
            m_bookmarkWidget->saveBookmarks(m_currentPath);
        setWindowTitle("pdf-tools");
        m_pageLabel->setText(tr("Page 1 / 1"));
        m_currentPath.clear();
        m_tocWidget->setDocument(nullptr);
        m_searchWidget->setDocument(nullptr);
        m_bookmarkWidget->clear();
        m_sideDock->hide();
    });

    connect(m_tocWidget, &TocWidget::pageRequested, m_view, &PdfView::goToPage);
    connect(m_searchWidget, &SearchWidget::pageRequested, m_view, &PdfView::goToPage);
    connect(m_bookmarkWidget, &BookmarkWidget::pageRequested, m_view, &PdfView::goToPage);

    connect(m_view, &PdfView::textSelected, this, [this](const QString &text) {
        statusBar()->showMessage(tr("Selected: %1 chars").arg(text.size()), 3000);
    });
}

void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("Open PDF"), QString(), tr("PDF Files (*.pdf)"));
    if (!path.isEmpty())
        m_view->loadFile(path);
}

void MainWindow::openRecentFile()
{
    auto *action = qobject_cast<QAction *>(sender());
    if (action)
        m_view->loadFile(action->data().toString());
}

void MainWindow::updateRecentFiles()
{
    auto actions = menuBar()->actions().first()->menu()->actions();
    for (int i = 0; i < MaxRecentFiles; ++i) {
        if (i < m_recentFiles.size()) {
            QString text = tr("&%1 %2").arg(i + 1).arg(
                QFileInfo(m_recentFiles[i]).fileName());
            actions[i + 1]->setText(text);
            actions[i + 1]->setData(m_recentFiles[i]);
            actions[i + 1]->setVisible(true);
        } else {
            actions[i + 1]->setVisible(false);
        }
    }
}

void MainWindow::printDocument()
{
    if (m_view->pageCount() == 0) {
        QMessageBox::information(this, tr("Print"), tr("No document open."));
        return;
    }

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Document"));
    if (dialog.exec() != QDialog::Accepted)
        return;

    statusBar()->showMessage(tr("Printing..."));

    QPainter painter;
    if (!painter.begin(&printer)) {
        statusBar()->clearMessage();
        return;
    }

    QPdfDocument *doc = m_view->document();
    int totalPages = m_view->pageCount();
    QSize pageSize = doc->pagePointSize(0).toSize();
    qreal scale = printer.logicalDpiX() * 1.0 / 72.0;

    for (int i = 0; i < totalPages; ++i) {
        if (i > 0)
            printer.newPage();

        QImage image = doc->render(i,
            QSize(qRound(pageSize.width() * scale),
                  qRound(pageSize.height() * scale)));

        QRectF targetRect = painter.viewport();
        QRectF sourceRect(0, 0, image.width(), image.height());
        painter.drawImage(targetRect, image, sourceRect);

        statusBar()->showMessage(tr("Printing page %1 / %2").arg(i + 1).arg(totalPages));
    }

    painter.end();
    statusBar()->showMessage(tr("Print complete."), 3000);
}

void MainWindow::goToPage()
{
    if (m_view->pageCount() == 0) {
        QMessageBox::information(this, tr("Go to Page"), tr("No document open."));
        return;
    }
    bool ok = false;
    int page = QInputDialog::getInt(this, tr("Go to Page"),
        tr("Page number (1 - %1):").arg(m_view->pageCount()),
        m_view->currentPage() + 1, 1, m_view->pageCount(), 1, &ok);
    if (ok)
        m_view->goToPage(page - 1);
}

void MainWindow::toggleDarkTheme()
{
    m_darkTheme = !m_darkTheme;
    if (m_darkTheme) {
        QApplication::setStyle(QStyleFactory::create("Fusion"));
        QPalette p;
        p.setColor(QPalette::Window, QColor(53, 53, 53));
        p.setColor(QPalette::WindowText, Qt::white);
        p.setColor(QPalette::Base, QColor(35, 35, 35));
        p.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        p.setColor(QPalette::ToolTipBase, QColor(25, 25, 25));
        p.setColor(QPalette::ToolTipText, Qt::white);
        p.setColor(QPalette::Text, Qt::white);
        p.setColor(QPalette::Button, QColor(53, 53, 53));
        p.setColor(QPalette::ButtonText, Qt::white);
        p.setColor(QPalette::BrightText, Qt::red);
        p.setColor(QPalette::Link, QColor(42, 130, 218));
        p.setColor(QPalette::Highlight, QColor(42, 130, 218));
        p.setColor(QPalette::HighlightedText, Qt::black);
        p.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        QApplication::setPalette(p);
    } else {
        QApplication::setStyle(QStyleFactory::create("windows"));
        QApplication::setPalette(QApplication::style()->standardPalette());
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About pdf-tools"),
        tr("<b>pdf-tools</b> v%1<br><br>"
           "A PDF reader built with Qt %2 and C++17.<br><br>"
           "Licensed under GPL v3.")
        .arg(QApplication::applicationVersion(),
             qVersion()));
}
