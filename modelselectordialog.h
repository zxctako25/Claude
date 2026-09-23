#ifndef MODELSELECTORDIALOG_H
#define MODELSELECTORDIALOG_H

#include <QDialog>
#include <QVector>
#include <QString>
#include <QHash>
#include <QStringList>

struct ModelEntry;

class QListWidget;

class ModelSelectorDialog : public QDialog
{
    Q_OBJECT
public:
    QString selectedModel;
    QString selectedName;

    ModelSelectorDialog(const QVector<ModelEntry> &models,
                        const QString &currentId,
                        QWidget *parent = nullptr);

private:
    QHash<QString, QVector<ModelEntry>> m_byFamily;
    QStringList m_families;
    QString m_currentId;
    QListWidget *m_catList = nullptr;
    QListWidget *m_modelList = nullptr;

    void populate(const QString &family);
    void refreshStyle();
};

#endif
