#include "TocWidget.h"

#include <QTreeView>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPdfBookmarkModel>
#include <QPdfDocument>

TocWidget::TocWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_treeView = new QTreeView(this);
    m_treeView->setHeaderHidden(true);
    m_treeView->setExpandsOnDoubleClick(true);
    m_treeView->setAnimated(true);
    layout->addWidget(m_treeView);

    m_model = new QPdfBookmarkModel(this);
    m_treeView->setModel(m_model);

    connect(m_treeView, &QTreeView::clicked, this, [this](const QModelIndex &index) {
        bool ok = false;
        int page = m_model->data(index, static_cast<int>(QPdfBookmarkModel::Role::Page)).toInt(&ok);
        if (ok && page >= 0)
            emit pageRequested(page);
    });
}

void TocWidget::setDocument(QPdfDocument *document)
{
    m_model->setDocument(document);
    m_treeView->expandAll();
}
