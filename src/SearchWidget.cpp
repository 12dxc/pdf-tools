#include "SearchWidget.h"

#include <QLineEdit>
#include <QListView>
#include <QLabel>
#include <QVBoxLayout>
#include <QPdfSearchModel>
#include <QPdfDocument>
#include <QTimer>

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);

    m_input = new QLineEdit(this);
    m_input->setPlaceholderText(tr("搜索..."));
    m_input->setClearButtonEnabled(true);
    layout->addWidget(m_input);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: #888; padding: 2px 4px;");
    layout->addWidget(m_statusLabel);

    m_listView = new QListView(this);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_listView, 1);

    m_model = new QPdfSearchModel(this);
    m_listView->setModel(m_model);

    m_statusLabel->setVisible(false);

    QTimer *debounce = new QTimer(this);
    debounce->setSingleShot(true);
    debounce->setInterval(300);

    connect(m_input, &QLineEdit::textChanged, this, [debounce](const QString &) {
        debounce->start();
    });

    connect(debounce, &QTimer::timeout, this, [this] {
        QString text = m_input->text().trimmed();
        if (text.isEmpty()) {
            m_model->setSearchString({});
            m_statusLabel->setVisible(false);
            return;
        }
        m_model->setSearchString(text);
    });

    connect(m_model, &QPdfSearchModel::countChanged, this, [this]() {
        int n = m_model->count();
        if (n > 0) {
            m_statusLabel->setText(tr("找到 %1 个结果").arg(n));
            m_statusLabel->setVisible(true);
        } else if (!m_input->text().trimmed().isEmpty()) {
            m_statusLabel->setText(tr("未找到结果"));
            m_statusLabel->setVisible(true);
        } else {
            m_statusLabel->setVisible(false);
        }
    });

    connect(m_listView, &QListView::clicked, this, [this](const QModelIndex &index) {
        bool ok = false;
        int page = m_model->data(index, static_cast<int>(QPdfSearchModel::Role::Page)).toInt(&ok);
        if (ok && page >= 0)
            emit pageRequested(page);
    });
}

void SearchWidget::setDocument(QPdfDocument *document)
{
    m_model->setDocument(document);
    m_input->clear();
    m_statusLabel->setVisible(false);
}

void SearchWidget::focusSearch()
{
    m_input->setFocus();
    m_input->selectAll();
}
