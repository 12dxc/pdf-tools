#ifndef BOOKMARKWIDGET_H
#define BOOKMARKWIDGET_H

#include <QWidget>
#include <QVector>

class QListWidget;
class QPushButton;

/** @brief 书签条目 */
struct BookmarkEntry
{
    QString title;  ///< 书签标题
    int page;       ///< 页码（0-based）
};

/**
 * @brief 书签管理面板
 *
 * 支持添加、删除书签，书签按文件路径持久化保存到 QSettings。
 */
class BookmarkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BookmarkWidget(QWidget *parent = nullptr);

    void addBookmark(const QString &title, int page);
    void loadBookmarks(const QString &fileKey);
    void saveBookmarks(const QString &fileKey);
    void clear();

signals:
    void pageRequested(int page);

private:
    void removeSelected();

    QListWidget *m_list = nullptr;
    QVector<BookmarkEntry> m_entries;
};

#endif // BOOKMARKWIDGET_H
