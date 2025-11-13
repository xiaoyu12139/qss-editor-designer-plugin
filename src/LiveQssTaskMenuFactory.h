#pragma once
#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QDesignerTaskMenuExtension>
#include <QWidget>

class LiveQssTaskMenuFactory : public QExtensionFactory {
    Q_OBJECT
public:
    explicit LiveQssTaskMenuFactory(QExtensionManager* parent = nullptr);
protected:
    QObject* createExtension(QObject* object, const QString& iid, QObject* parent) const override;
};
