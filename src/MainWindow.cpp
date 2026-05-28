#include "MainWindow.h"
#include "PdfView.h"
#include "TocWidget.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QComboBox>
#include <QSplitter>
#include <QDockWidget>
#include <QStackedWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QCloseEvent>
#include <QSettings>
#include <QApplication>
#include <QAction>
#include <QStyle>
#include <QFileInfo>

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

    auto *closeAction = fileMenu->addAction(tr("&Close"));
    closeAction->setShortcut(QKeySequence::Close);
    connect(closeAction, &QAction::triggered, m_view, &PdfView::closeDocument);

    auto *exitAction = fileMenu->addAction(tr("E&xit"));
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // View menu
    auto *viewMenu = menuBar()->addMenu(tr("&View"));
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

    m_sideStack = new QStackedWidget;

    m_tocWidget = new TocWidget;
    m_sideStack->addWidget(m_tocWidget);

    m_sideDock->setWidget(m_sideStack);
    addDockWidget(Qt::LeftDockWidgetArea, m_sideDock);
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

        m_tocWidget->setDocument(m_view->document());
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
        setWindowTitle("pdf-tools");
        m_pageLabel->setText(tr("Page 1 / 1"));
        m_tocWidget->setDocument(nullptr);
        m_sideDock->hide();
    });

    connect(m_tocWidget, &TocWidget::pageRequested, m_view, &PdfView::goToPage);
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

void MainWindow::about()
{
    QMessageBox::about(this, tr("About pdf-tools"),
        tr("<b>pdf-tools</b> v%1<br><br>"
           "A PDF reader built with Qt %2 and C++17.<br><br>"
           "Licensed under GPL v3.")
        .arg(QApplication::applicationVersion(),
             qVersion()));
}
