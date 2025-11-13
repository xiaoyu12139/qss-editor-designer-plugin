#include "LiveQssTaskMenu.h"
#include <QtDesigner/QDesignerFormWindowInterface>
extern "C" void LiveQss_EnsureTitleMenu(QDesignerFormWindowInterface* fw);

QHash<QDesignerFormWindowInterface*, QFileSystemWatcher*> LiveQssTaskMenu::s_watchers;

LiveQssTaskMenu::LiveQssTaskMenu(QWidget* widget, QObject* parent)
    : QObject(parent), m_widget(widget) {
    m_bind = new QAction(QString::fromUtf8(u8"绑定QSS文件..."), this);
    m_reload = new QAction(QString::fromUtf8(u8"重新加载样式"), this);
    connect(m_bind, &QAction::triggered, this, &LiveQssTaskMenu::bindQss);
    connect(m_reload, &QAction::triggered, this, &LiveQssTaskMenu::reloadQss);
}

QList<QAction*> LiveQssTaskMenu::taskActions() const {
    QList<QAction*> actions;
    auto fw = QDesignerFormWindowInterface::findFormWindow(m_widget);
    if (!fw) return actions;
    LiveQss_EnsureTitleMenu(fw);
    actions << m_bind << m_reload;
    return actions;
}

void LiveQssTaskMenu::bindQss() {
    auto fw = QDesignerFormWindowInterface::findFormWindow(m_widget);
    if (!fw) return;
    QString path = QFileDialog::getOpenFileName(
        m_widget,
        QString::fromUtf8(u8"选择QSS文件"),
        QString(),
        QString::fromUtf8(u8"QSS (*.qss);;所有文件 (*.*)")
    );
    if (path.isEmpty()) return;
    auto root = fw->mainContainer();
    if (!root) return;
    root->setProperty("designerQssPath", path);
    ensureWatcher(fw, path);
    applyFile(path, root);
}

void LiveQssTaskMenu::reloadQss() {
    auto fw = QDesignerFormWindowInterface::findFormWindow(m_widget);
    if (!fw) return;
    auto root = fw->mainContainer();
    if (!root) return;
    QString path = root->property("designerQssPath").toString();
    if (path.isEmpty()) return;
    applyFile(path, root);
}

void LiveQssTaskMenu::ensureWatcher(QDesignerFormWindowInterface* fw, const QString& path) {
    if (!s_watchers.contains(fw)) {
        auto w = new QFileSystemWatcher(this);
        s_watchers.insert(fw, w);
        connect(w, &QFileSystemWatcher::fileChanged, this, [this, fw](const QString& p) {
            applyFile(p, fw->mainContainer());
            ensureReadd(fw, p);
        });
        connect(fw, &QObject::destroyed, this, [fw]() {
            auto w = s_watchers.take(fw);
            if (w) w->deleteLater();
        });
    }
    auto w = s_watchers.value(fw);
    if (!w->files().contains(path)) w->addPath(path);
    QFileInfo info(path);
    if (info.exists()) {
        QString dir = info.absolutePath();
        if (!w->directories().contains(dir)) w->addPath(dir);
    }
}

void LiveQssTaskMenu::ensureReadd(QDesignerFormWindowInterface* fw, const QString& path) {
    auto w = s_watchers.value(fw);
    if (!w) return;
    QFileInfo info(path);
    if (info.exists() && !w->files().contains(path)) w->addPath(path);
}

void LiveQssTaskMenu::applyFile(const QString& path, QWidget* root) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QString qss = QString::fromUtf8(f.readAll());
    root->setStyleSheet(qss);
}
