#pragma once
#include <QtUiPlugin/QDesignerCustomWidgetInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>
#include <QObject>

class LiveQssRegistrar : public QObject, public QDesignerCustomWidgetInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface")
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    explicit LiveQssRegistrar(QObject* parent = nullptr);
    QString name() const override;
    QString group() const override;
    QString toolTip() const override;
    QString whatsThis() const override;
    QString includeFile() const override;
    QIcon icon() const override;
    bool isContainer() const override;
    QWidget* createWidget(QWidget* parent) override;
    QString domXml() const override;
    void initialize(QDesignerFormEditorInterface* core) override;
    bool isInitialized() const override;
private:
    bool m_initialized;
};
