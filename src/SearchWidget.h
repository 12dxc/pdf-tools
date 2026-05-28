#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>

class QLineEdit;
class QListView;
class QLabel;
class QPdfSearchModel;
class QPdfDocument;

/**
 * @brief 全文搜索面板
 *
 * 提供实时搜索功能，显示匹配结果列表，点击可跳转到对应页面。
 * 搜索使用 300ms 防抖，避免频繁查询。
 */
class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(QWidget *parent = nullptr);

    void setDocument(QPdfDocument *document);
    void focusSearch();

signals:
    void pageRequested(int page);

private:
    QLineEdit *m_input = nullptr;
    QListView *m_listView = nullptr;
    QLabel *m_statusLabel = nullptr;
    QPdfSearchModel *m_model = nullptr;
};

#endif // SEARCHWIDGET_H
