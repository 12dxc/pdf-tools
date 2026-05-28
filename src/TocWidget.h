#ifndef TOCWIDGET_H
#define TOCWIDGET_H

#include <QWidget>

class QTreeView;
class QPdfBookmarkModel;
class QPdfDocument;

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
