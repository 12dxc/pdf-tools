#ifndef TOCWIDGET_H
#define TOCWIDGET_H

#include <QWidget>

class QTreeView;
class QPdfBookmarkModel;
class QPdfDocument;

/**
 * @brief 目录导航面板
 *
 * 显示 PDF 文档的书签目录树，点击条目可跳转到对应页面。
 */
class TocWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TocWidget(QWidget *parent = nullptr);

    void setDocument(QPdfDocument *document);

signals:
    void pageRequested(int page);

private:
    QTreeView *m_treeView = nullptr;
    QPdfBookmarkModel *m_model = nullptr;
};

#endif // TOCWIDGET_H
