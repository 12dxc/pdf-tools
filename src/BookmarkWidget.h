#ifndef BOOKMARKWIDGET_H
#define BOOKMARKWIDGET_H

#include <QWidget>
#include <QVector>

class QListWidget;
class QPushButton;

struct BookmarkEntry
{
    QString title;
    int page;
};

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
