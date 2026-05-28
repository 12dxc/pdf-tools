#include "BookmarkWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QSettings>

BookmarkWidget::BookmarkWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);

    m_list = new QListWidget(this);
    layout->addWidget(m_list, 1);

    auto *delBtn = new QPushButton(tr("Delete"), this);
    layout->addWidget(delBtn);

    connect(delBtn, &QPushButton::clicked, this, &BookmarkWidget::removeSelected);

    connect(m_list, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        int row = m_list->row(item);
        if (row >= 0 && row < m_entries.size())
            emit pageRequested(m_entries[row].page);
    });
}

void BookmarkWidget::addBookmark(const QString &title, int page)
{
    for (const auto &e : m_entries) {
        if (e.page == page)
            return;
    }
    BookmarkEntry entry;
    entry.title = title;
    entry.page = page;
    m_entries.append(entry);
    m_list->addItem(QString("%1  (p.%2)").arg(title).arg(page + 1));
}

void BookmarkWidget::loadBookmarks(const QString &fileKey)
{
    clear();
    if (fileKey.isEmpty())
        return;

    QSettings settings;
    int size = settings.beginReadArray(QString("bookmarks/%1").arg(fileKey));
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        BookmarkEntry entry;
        entry.title = settings.value("title").toString();
        entry.page = settings.value("page").toInt();
        m_entries.append(entry);
        m_list->addItem(QString("%1  (p.%2)").arg(entry.title).arg(entry.page + 1));
    }
    settings.endArray();
}

void BookmarkWidget::saveBookmarks(const QString &fileKey)
{
    if (fileKey.isEmpty())
        return;

    QSettings settings;
    settings.beginWriteArray(QString("bookmarks/%1").arg(fileKey));
    for (int i = 0; i < m_entries.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("title", m_entries[i].title);
        settings.setValue("page", m_entries[i].page);
    }
    settings.endArray();
}

void BookmarkWidget::clear()
{
    m_entries.clear();
    m_list->clear();
}

void BookmarkWidget::removeSelected()
{
    int row = m_list->currentRow();
    if (row < 0 || row >= m_entries.size())
        return;
    m_entries.removeAt(row);
    delete m_list->takeItem(row);
}
