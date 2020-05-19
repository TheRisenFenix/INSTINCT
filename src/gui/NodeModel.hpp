#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>

#include <nodes/NodeData>
#include <nodes/NodeDataModel>

#include <memory>
#include <vector>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::PortType;
using QtNodes::PortIndex;

/// The model dictates the number of inputs and outputs for the Node
class NodeModel : public NodeDataModel
{
    Q_OBJECT

  public:
    explicit NodeModel(QString const& name);

    QString caption() const override;

    QString name() const override;

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    QJsonObject save() const override;

    void restore(QJsonObject const& p) override;

    QWidget* embeddedWidget() override;

    std::shared_ptr<NodeData> outData(PortIndex) override;

    void setInData(std::shared_ptr<NodeData>, int) override;

    std::vector<QWidget*> widgets;

  private:
    QString const _name = "Template";
    QWidget* _mainWidget;

    void addListListIntRow(std::vector<std::string> config, int row, QGridLayout* layout, QGroupBox* gridGroupBox, QFormLayout* formLayout);
};