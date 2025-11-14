#include "LiveQssRegistrar.h"
#include <QWidget>
#include <QApplication>
#include <QAbstractNativeEventFilter>
#include <QMdiSubWindow>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QFileSystemWatcher>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHash>
#include <QSet>
#ifdef Q_OS_WIN
#  include <windows.h>
#endif

#include <QMenu>
#include <QAction>
#include <QStyle>

static QHash<QDesignerFormWindowInterface*, QFileSystemWatcher*> g_watchers;

static void liveqss_applyFile(const QString& path, QWidget* target) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QString qss = QString::fromUtf8(f.readAll());
    target->setStyleSheet(qss);
}

static QMdiSubWindow* liveqss_findSubWindow(QDesignerFormWindowInterface* fw) {
    QWidget* p = fw;
    QMdiSubWindow* sub = nullptr;
    while (p && !sub) {
        sub = qobject_cast<QMdiSubWindow*>(p);
        p = p->parentWidget();
    }
    return sub;
}

static void liveqss_ensureWatcher(QDesignerFormWindowInterface* fw, const QString& path) {
    auto w = g_watchers.value(fw, nullptr);
    if (!w) {
        w = new QFileSystemWatcher(qApp);
        g_watchers.insert(fw, w);
        QObject::connect(fw, &QObject::destroyed, qApp, [fw]() {
            auto ww = g_watchers.take(fw);
            if (ww) ww->deleteLater();
        });
        QObject::connect(w, &QFileSystemWatcher::fileChanged, qApp, [fw, w](const QString& p) {
            QWidget* root = fw->mainContainer();
            if (!root) return;
            if (!root->property("liveqss_hot_enabled").toBool()) return;
            QMdiSubWindow* sub = liveqss_findSubWindow(fw);
            if (sub) {
                liveqss_applyFile(p, sub);
            }
            QFileInfo info(p);
            if (info.exists() && !w->files().contains(p)) w->addPath(p);
        });
    }
    if (!w->files().contains(path)) w->addPath(path);
    QFileInfo info(path);
    if (info.exists()) {
        QString dir = info.absolutePath();
        if (!w->directories().contains(dir)) w->addPath(dir);
    }
}

extern "C" void LiveQss_EnsureTitleMenu(QDesignerFormWindowInterface* fw) {
    if (!fw) return;
    QWidget* p = fw;
    QMdiSubWindow* sub = nullptr;
    while (p && !sub) {
        sub = qobject_cast<QMdiSubWindow*>(p);
        p = p->parentWidget();
    }
    if (!sub) return;
    if (sub->property("liveqss_menu_injected").toBool()) return;

    QMenu* m = sub->systemMenu();
    if (!m) return;

    QAction* bind = m->addAction(QString("bind QSS file..."));
    bool enabled = fw->mainContainer()->property("liveqss_hot_enabled").toBool();
    QIcon iconOn = qApp->style()->standardIcon(QStyle::SP_DialogYesButton);
    QIcon iconOff = qApp->style()->standardIcon(QStyle::SP_DialogCancelButton);
    QAction* hot = m->addAction(enabled ? QString("Hot Reload: On") : QString("Hot Reload: Off"));
    hot->setCheckable(true);
    hot->setChecked(enabled);
    hot->setIcon(enabled ? iconOn : iconOff);

    QObject::connect(bind, &QAction::triggered, sub, [fw]() {
        QWidget* root = fw->mainContainer();
        if (!root) return;
        QMdiSubWindow* sub = liveqss_findSubWindow(fw);
        if (!sub) return;
        QString path = QFileDialog::getOpenFileName(
            root,
            QString("select QSS file"),
            QString(),
            QString("QSS (*.qss);;all file(*.*)")
        );
        if (path.isEmpty()) return;
        root->setProperty("designerQssPath", path);
        liveqss_ensureWatcher(fw, path);
        liveqss_applyFile(path, sub);
    });

    QObject::connect(hot, &QAction::triggered, sub, [fw, hot, iconOn, iconOff]() {
        QWidget* root = fw->mainContainer();
        if (!root) return;
        bool cur = root->property("liveqss_hot_enabled").toBool();
        bool next = !cur;
        root->setProperty("liveqss_hot_enabled", next);
        hot->setChecked(next);
        hot->setIcon(next ? iconOn : iconOff);
        hot->setText(next ? QString("Hot Reload: On") : QString("Hot Reload: Off"));
        QString path = root->property("designerQssPath").toString();
        if (next && !path.isEmpty()) {
            liveqss_ensureWatcher(fw, path);
        }
    });

    sub->setProperty("liveqss_menu_injected", true);
}

LiveQssRegistrar::LiveQssRegistrar(QObject* parent) : QObject(parent), m_initialized(false) {}

QString LiveQssRegistrar::name() const { return QStringLiteral("LiveQssRegistrar"); }
QString LiveQssRegistrar::group() const { return QStringLiteral("LiveQss"); }
QString LiveQssRegistrar::toolTip() const { return QString(); }
QString LiveQssRegistrar::whatsThis() const { return QString(); }
QString LiveQssRegistrar::includeFile() const { return QStringLiteral("LiveQssRegistrar.h"); }
QIcon LiveQssRegistrar::icon() const { return QIcon(); }
bool LiveQssRegistrar::isContainer() const { return false; }
QWidget* LiveQssRegistrar::createWidget(QWidget* parent) { return new QWidget(parent); }
QString LiveQssRegistrar::domXml() const { return QStringLiteral("<widget class=\"QWidget\" name=\"LiveQssRegistrar\"/>\n"); }

void LiveQssRegistrar::initialize(QDesignerFormEditorInterface* core) {
    if (m_initialized) return;
    auto manager = core->extensionManager();
    auto fwm = core->formWindowManager();
    if (fwm) {
        int count = fwm->formWindowCount();
        for (int idx = 0; idx < count; ++idx) {
            QDesignerFormWindowInterface* fw = fwm->formWindow(idx);
            if (fw) LiveQss_EnsureTitleMenu(fw);
        }
        QObject::connect(fwm, &QDesignerFormWindowManagerInterface::formWindowAdded, qApp, [](QDesignerFormWindowInterface* fw){ LiveQss_EnsureTitleMenu(fw); });
        QObject::connect(fwm, &QDesignerFormWindowManagerInterface::activeFormWindowChanged, qApp, [](QDesignerFormWindowInterface* fw){ LiveQss_EnsureTitleMenu(fw); });
    }
    m_initialized = true;
}

bool LiveQssRegistrar::isInitialized() const { return m_initialized; }
