#ifndef PDFVIEW_H
#define PDFVIEW_H

#include <QGraphicsView>

class QPdfDocument;
class QPdfPageRenderer;
class QGraphicsScene;
class QGraphicsPixmapItem;

/**
 * @brief PDF 页面渲染视图
 *
 * 基于 QGraphicsView 的 PDF 页面显示组件，支持：
 * - 页面渲染与异步更新
 * - 缩放（25%-500%）与滚动
 * - 文本选择与复制
 * - 拖放打开文件
 */
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
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

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
