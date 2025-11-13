#include "LiveQssTaskMenuFactory.h"
#include "LiveQssTaskMenu.h"

LiveQssTaskMenuFactory::LiveQssTaskMenuFactory(QExtensionManager* parent) : QExtensionFactory(parent) {}

QObject* LiveQssTaskMenuFactory::createExtension(QObject* object, const QString& iid, QObject* parent) const {
    if (iid != QStringLiteral("org.qt-project.Qt.Designer.TaskMenu")) return nullptr;
    QWidget* w = qobject_cast<QWidget*>(object);
    if (!w) return nullptr;
    return new LiveQssTaskMenu(w, parent);
}