#include "PdfView.h"

#include <QPdfDocument>
#include <QPdfPageRenderer>
#include <QPdfDocumentRenderOptions>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>
#include <QRubberBand>
#include <QPainter>
#include <QPen>

PdfView::PdfView(QWidget *parent)
    : QGraphicsView(parent)
{
    m_document = new QPdfDocument(this);
    m_renderer = new QPdfPageRenderer(this);
    m_renderer->setDocument(m_document);

    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    m_pageItem = new QGraphicsPixmapItem();
    m_scene->addItem(m_pageItem);

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setBackgroundBrush(QBrush(QColor(0x80, 0x80, 0x80)));

    connect(m_renderer, &QPdfPageRenderer::pageRendered, this,
            [this](int pageNumber, QSize, const QImage &image,
                   QPdfDocumentRenderOptions, quint64) {
        if (pageNumber == m_page && !image.isNull()) {
            m_pageItem->setPixmap(QPixmap::fromImage(image));
            m_scene->setSceneRect(m_pageItem->boundingRect());
            setZoom(m_zoom);
            emit pageChanged(m_page);
        }
    });
}

void PdfView::loadFile(const QString &path)
{
    auto err = m_document->load(path);
    if (err != QPdfDocument::Error::None) {
        closeDocument();
        return;
    }
    m_pageCount = m_document->pageCount();
    m_page = 0;
    renderCurrentPage();
    emit documentLoaded(path);
}

void PdfView::closeDocument()
{
    m_page = 0;
    m_pageCount = 0;
    m_pageItem->setPixmap(QPixmap());
    m_scene->setSceneRect(QRectF());
    emit documentClosed();
}

void PdfView::goNextPage()
{
    if (m_page < m_pageCount - 1) {
        ++m_page;
        renderCurrentPage();
    }
}

void PdfView::goPrevPage()
{
    if (m_page > 0) {
        --m_page;
        renderCurrentPage();
    }
}

void PdfView::goToPage(int page)
{
    if (page >= 0 && page < m_pageCount) {
        m_page = page;
        renderCurrentPage();
    }
}

void PdfView::setZoom(double percent)
{
    m_zoom = qBound(25.0, percent, 500.0);  // 限制缩放范围 25%-500%
    QGraphicsView::resetTransform();
    qreal scale = m_zoom / 100.0;
    QGraphicsView::scale(scale, scale);
    emit zoomChanged(m_zoom);
}

void PdfView::fitToWidth()
{
    if (!m_pageItem || m_pageItem->pixmap().isNull())
        return;
    fitInView(m_pageItem, Qt::IgnoreAspectRatio);
    m_zoom = qBound(25.0, transform().m11() * 100.0, 500.0);
    emit zoomChanged(m_zoom);
}

void PdfView::fitToPage()
{
    if (!m_pageItem || m_pageItem->pixmap().isNull())
        return;
    fitInView(m_pageItem, Qt::KeepAspectRatio);
    m_zoom = qBound(25.0, transform().m11() * 100.0, 500.0);
    emit zoomChanged(m_zoom);
}

void PdfView::renderCurrentPage()
{
    if (m_pageCount == 0)
        return;
    // 使用页面原始尺寸渲染，保持清晰度
    QSize pageSize = m_document->pagePointSize(m_page).toSize();
    m_renderer->requestPage(m_page, pageSize);
}

void PdfView::setSelectMode(bool on)
{
    m_selectMode = on;
    if (on) {
        setDragMode(QGraphicsView::NoDrag);
        setCursor(Qt::IBeamCursor);
    } else {
        setDragMode(QGraphicsView::ScrollHandDrag);
        setCursor(Qt::ArrowCursor);
    }
    emit selectModeChanged(on);
}

void PdfView::copySelection()
{
    if (!m_selectedText.isEmpty()) {
        QApplication::clipboard()->setText(m_selectedText);
    }
}

void PdfView::mousePressEvent(QMouseEvent *event)
{
    if (m_selectMode && event->button() == Qt::LeftButton) {
        m_selecting = true;
        m_selectStart = mapToScene(event->pos());
        m_selectEnd = m_selectStart;
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void PdfView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_selecting) {
        m_selectEnd = mapToScene(event->pos());
        m_scene->update();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void PdfView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_selecting && event->button() == Qt::LeftButton) {
        m_selecting = false;

        if (m_pageCount == 0)
            return;

        m_selectEnd = mapToScene(event->pos());

        QPointF topLeft(qMin(m_selectStart.x(), m_selectEnd.x()),
                        qMin(m_selectStart.y(), m_selectEnd.y()));
        QPointF bottomRight(qMax(m_selectStart.x(), m_selectEnd.x()),
                            qMax(m_selectStart.y(), m_selectEnd.y()));

        QPdfSelection sel = m_document->getSelection(
            m_page, topLeft, bottomRight);
        m_selectedText = sel.text();

        if (!m_selectedText.isEmpty())
            emit textSelected(m_selectedText);

        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void PdfView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        double delta = event->angleDelta().y();
        double step = delta > 0 ? 15.0 : -15.0;
        setZoom(m_zoom + step);
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void PdfView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_PageDown:
        goNextPage();
        break;
    case Qt::Key_PageUp:
        goPrevPage();
        break;
    case Qt::Key_Home:
        goToPage(0);
        break;
    case Qt::Key_End:
        goToPage(m_pageCount - 1);
        break;
    case Qt::Key_C:
        if (event->modifiers() & Qt::ControlModifier)
            copySelection();
        else
            QGraphicsView::keyPressEvent(event);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

void PdfView::dragEnterEvent(QDragEnterEvent *event)
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

void PdfView::dropEvent(QDropEvent *event)
{
    for (const auto &url : event->mimeData()->urls()) {
        QString path = url.toLocalFile();
        if (path.endsWith(".pdf", Qt::CaseInsensitive)) {
            loadFile(path);
            event->accept();
            return;
        }
    }
}
