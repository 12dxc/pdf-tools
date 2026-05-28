#ifndef PDFVIEW_H
#define PDFVIEW_H

#include <QGraphicsView>

class QPdfDocument;
class QPdfPageRenderer;
class QGraphicsScene;
class QGraphicsPixmapItem;

class PdfView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit PdfView(QWidget *parent = nullptr);

    void loadFile(const QString &path);
    void closeDocument();

    QPdfDocument *document() const { return m_document; }
    int currentPage() const { return m_page; }
    int pageCount() const { return m_pageCount; }
    double zoomFactor() const { return m_zoom; }
    bool isSelectMode() const { return m_selectMode; }
    QString selectedText() const { return m_selectedText; }

public slots:
    void goNextPage();
    void goPrevPage();
    void goToPage(int page);
    void setZoom(double percent);
    void fitToWidth();
    void fitToPage();
    void setSelectMode(bool on);
    void copySelection();

signals:
    void pageChanged(int page);
    void zoomChanged(double percent);
    void documentLoaded(const QString &path);
    void documentClosed();
    void selectModeChanged(bool on);
    void textSelected(const QString &text);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void renderCurrentPage();

    QPdfDocument *m_document = nullptr;
    QPdfPageRenderer *m_renderer = nullptr;
    QGraphicsScene *m_scene = nullptr;
    QGraphicsPixmapItem *m_pageItem = nullptr;

    int m_page = 0;
    int m_pageCount = 0;
    double m_zoom = 100.0;

    bool m_selectMode = false;
    bool m_selecting = false;
    QPointF m_selectStart;
    QPointF m_selectEnd;
    QString m_selectedText;
};

#endif // PDFVIEW_H
