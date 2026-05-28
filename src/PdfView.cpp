#include "PdfView.h"

#include <QPdfDocument>
#include <QPdfPageRenderer>
#include <QPdfDocumentRenderOptions>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QKeyEvent>

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
        emit documentClosed();
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
    m_zoom = qBound(25.0, percent, 500.0);
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
    m_zoom = transform().m11() * 100.0;
    emit zoomChanged(m_zoom);
}

void PdfView::fitToPage()
{
    if (!m_pageItem || m_pageItem->pixmap().isNull())
        return;
    fitInView(m_pageItem, Qt::KeepAspectRatio);
    m_zoom = transform().m11() * 100.0;
    emit zoomChanged(m_zoom);
}

void PdfView::renderCurrentPage()
{
    if (m_pageCount == 0)
        return;
    QSize pageSize = m_document->pagePointSize(m_page).toSize();
    m_renderer->requestPage(m_page, pageSize);
}

void PdfView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        double delta = event->angleDelta().y();
        double factor = 1.0 + (delta > 0 ? 0.15 : -0.15);
        QGraphicsView::scale(factor, factor);
        m_zoom = transform().m11() * 100.0;
        emit zoomChanged(m_zoom);
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void PdfView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_PageDown:
    case Qt::Key_Down:
    case Qt::Key_Right:
        goNextPage();
        break;
    case Qt::Key_PageUp:
    case Qt::Key_Up:
    case Qt::Key_Left:
        goPrevPage();
        break;
    case Qt::Key_Home:
        goToPage(0);
        break;
    case Qt::Key_End:
        goToPage(m_pageCount - 1);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}
