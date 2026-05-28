#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>

class QLineEdit;
class QListView;
class QLabel;
class QPdfSearchModel;
class QPdfDocument;

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
