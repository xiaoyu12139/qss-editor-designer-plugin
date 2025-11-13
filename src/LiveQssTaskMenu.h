#pragma once
#include <QtDesigner/QDesignerTaskMenuExtension>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QAction>
#include <QFileSystemWatcher>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QWidget>
#include <QFileDialog>

class LiveQssTaskMenu : public QObject, public QDesignerTaskMenuExtension {
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)
public:
    explicit LiveQssTaskMenu(QWidget* widget, QObject* parent = nullptr);
    QList<QAction*> taskActions() const override;
private slots:
    void bindQss();
    void reloadQss();
private:
    void ensureWatcher(QDesignerFormWindowInterface* fw, const QString& path);
    void ensureReadd(QDesignerFormWindowInterface* fw, const QString& path);
    static void applyFile(const QString& path, QWidget* root);
    QWidget* m_widget;
    QAction* m_bind;
    QAction* m_reload;
    static QHash<QDesignerFormWindowInterface*, QFileSystemWatcher*> s_watchers;
};
