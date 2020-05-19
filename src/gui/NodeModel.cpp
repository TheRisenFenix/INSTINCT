#include "NodeModel.hpp"

#include <QtWidgets/QLabel>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>

#include <tuple>
#include <iostream>
#include <sstream>

#include "main.hpp"

void NodeModel::addListListIntRow(std::vector<std::string> config, int row, QGridLayout* layout, QGroupBox* gridGroupBox, QFormLayout* formLayout)
{
    for (size_t j = 0; j < config.size(); j++)
    {
        const std::string& line = config.at(j);
        if (j <= 1)
        {
            QComboBox* comboBox = new QComboBox(gridGroupBox);
            std::stringstream lineStream(line);
            std::string cell;
            while (std::getline(lineStream, cell, '|'))
            {
                if (cell.at(0) == '[')
                {
                    comboBox->addItem(QString::fromStdString(cell.substr(1, cell.size() - 2)));
                    comboBox->setCurrentIndex(comboBox->count() - 1);
                }
                else
                    comboBox->addItem(QString::fromStdString(cell));
            }
            comboBox->setStyleSheet("QComboBox { background-color: rgb(220,220,220); selection-background-color: rgb(169,169,169); color: black }");
            comboBox->setProperty("Row", row);

            layout->addWidget(comboBox, row, static_cast<int>(j));
        }
        else if (j == 2)
        {
            QSpinBox* spinBox = new QSpinBox(gridGroupBox);
            std::stringstream lineStream(line);
            std::string cell;
            for (size_t k = 0; k < 3; k++)
            {
                std::getline(lineStream, cell, '|');
                if (k == 0)
                    spinBox->setMinimum(std::stoi(cell));
                else if (k == 1)
                    spinBox->setValue(std::stoi(cell));
                else if (k == 2)
                    spinBox->setMaximum(std::stoi(cell));
            }
            spinBox->setStyleSheet("QSpinBox { background-color: rgb(220,220,220); selection-background-color: rgb(169,169,169); color: black }");
            spinBox->setProperty("Row", row);

            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                    [this, config, spinBox, layout, gridGroupBox, formLayout](int i) {
                        if (i != -1 && spinBox->property("Row") == layout->rowCount() - 1)
                            this->addListListIntRow(config, layout->rowCount(), layout, gridGroupBox, formLayout);
                    });

            layout->addWidget(spinBox, row, static_cast<int>(j));
        }
    }
}

NodeModel::NodeModel(QString const& name)
    : _name(name), _mainWidget(new QWidget())
{
    std::cout << "Creating " << name.toStdString() << std::endl;
    _mainWidget->setStyleSheet("QWidget { background-color: transparent; color: white }");
    QFormLayout* _layout = new QFormLayout(_mainWidget);
    _layout->setContentsMargins(0, 5, 0, 0);
    _layout->setLabelAlignment(Qt::AlignRight);

    const auto& nodeInfo = nodeManager.registeredNodeTypes().find(name.toStdString())->second;

    for (const auto& config : nodeInfo.constructorEmpty()->guiConfig())
    {
        QString description = QString::fromStdString(std::get<1>(config));

        if (std::get<0>(config) == NAV::Node::ConfigOptions::CONFIG_BOOL)
        {
            widgets.push_back(new QCheckBox());
            QCheckBox* checkBox = static_cast<QCheckBox*>(widgets.at(widgets.size() - 1));

            checkBox->setChecked(std::stoi(std::get<3>(config).front()));
            checkBox->setStyleSheet("QCheckBox::indicator:unchecked { border: 1px solid rgb(220,220,220); }");
            _layout->addRow(description, checkBox);
        }
        else if (std::get<0>(config) == NAV::Node::ConfigOptions::CONFIG_INT)
        {
            widgets.push_back(new QSpinBox());
            QSpinBox* spinBox = static_cast<QSpinBox*>(widgets.at(widgets.size() - 1));

            int defaultValue = 0;
            for (size_t j = 0; j < 3; j++)
            {
                std::string cell = std::get<3>(config).at(j);
                if (j == 0)
                    spinBox->setMinimum(std::stoi(cell));
                else if (j == 1)
                    defaultValue = std::stoi(cell);
                else if (j == 2)
                {
                    spinBox->setMaximum(std::stoi(cell));
                    spinBox->setValue(defaultValue);
                }
            }

            spinBox->setSingleStep(1);
            spinBox->setStyleSheet("QSpinBox { background: rgb(220,220,220); selection-background-color: rgb(169,169,169); color: black }");
            _layout->addRow(description, spinBox);
        }
        else if (std::get<0>(config) == NAV::Node::ConfigOptions::CONFIG_FLOAT)
        {
            widgets.push_back(new QDoubleSpinBox());
            QDoubleSpinBox* doubleSpinBox = static_cast<QDoubleSpinBox*>(widgets.at(widgets.size() - 1));

            double defaultValue = 0.0;
            for (size_t j = 0; j < 3; j++)
            {
                std::string cell = std::get<3>(config).at(j);
                if (j == 0)
                    doubleSpinBox->setMinimum(std::stod(cell));
                else if (j == 1)
                    defaultValue = std::stod(cell);
                else if (j == 2)
                {
                    doubleSpinBox->setMaximum(std::stod(cell));
                    doubleSpinBox->setValue(defaultValue);
                }
            }

            doubleSpinBox->setSingleStep(1.0);
            doubleSpinBox->setStyleSheet("QDoubleSpinBox { background: rgb(220,220,220); selection-background-color: rgb(169,169,169); color: black }");
            _layout->addRow(description, doubleSpinBox);
        }
        else if (std::get<0>(config) == NAV::Node::ConfigOptions::CONFIG_STRING)
        {
            widgets.push_back(new QLineEdit());
            QLineEdit* lineEdit = static_cast<QLineEdit*>(widgets.at(widgets.size() - 1));

            lineEdit->setStyleSheet("QLineEdit { background: rgb(220,220,220); selection-background-color: rgb(169,169,169); color: black }");
            lineEdit->setText(QString::fromStdString(std::get<3>(config).front()));
            _layout->addRow(description, lineEdit);
        }
        else if (std::get<0>(config) == NAV::Node::ConfigOptions::CONFIG_LIST)
        {
            widgets.push_back(new QComboBox());
            QComboBox* comboBox = static_cast<QComboBox*>(widgets.at(widgets.size() - 1));

            for (auto& cell : std::get<3>(config))
            {
                if (cell.at(0) == '[')
                {
                    comboBox->addItem(QString::fromStdString(cell.substr(1, cell.size() - 2)));
                    comboBox->setCurrentIndex(comboBox->count() - 1);
                }
                else
                    comboBox->addItem(QString::fromStdString(cell));
            }

            comboBox->setStyleSheet("QComboBox { background: rgb(220,220,220); selection-background-color: rgb(169,169,169); color: black }");
            _layout->addRow(description, comboBox);
        }
        else if (std::get<0>(config) == NAV::Node::ConfigOptions::CONFIG_LIST_LIST_INT)
        {
            widgets.push_back(new QGroupBox(description));
            QGroupBox* gridGroupBox = static_cast<QGroupBox*>(widgets.at(widgets.size() - 1));
            QGridLayout* layout = new QGridLayout;

            layout->addWidget(new QLabel("X Data Source", gridGroupBox), 0, 0);
            layout->addWidget(new QLabel("Y Data Source", gridGroupBox), 0, 1);
            layout->addWidget(new QLabel("Window", gridGroupBox), 0, 2);

            addListListIntRow(std::get<3>(config), 1, layout, gridGroupBox, _layout);

            gridGroupBox->setLayout(layout);
            // gridGroupBox->setStyleSheet("QGroupBox { color: green; }");
            _layout->addRow(gridGroupBox);
        }
        else if (std::get<0>(config) == NAV::Node::ConfigOptions::CONFIG_MAP_INT)
        {
            widgets.push_back(new QSpinBox());
            QSpinBox* spinBox = static_cast<QSpinBox*>(widgets.at(widgets.size() - 1));

            int defaultValue = 0;
            for (size_t j = 0; j < 4; j++)
            {
                std::string cell = std::get<3>(config).at(j);
                if (j == 0)
                    spinBox->setProperty("key", QString::fromStdString(cell));
                if (j == 1)
                    spinBox->setMinimum(std::stoi(cell));
                else if (j == 2)
                    defaultValue = std::stoi(cell);
                else if (j == 3)
                {
                    spinBox->setMaximum(std::stoi(cell));
                    spinBox->setValue(defaultValue);
                }
            }

            spinBox->setSingleStep(1);
            spinBox->setStyleSheet("QSpinBox { background: rgb(220,220,220); selection-background-color: rgb(169,169,169); color: black }");
            _layout->addRow(description, spinBox);
        }

        QWidget* widget = widgets.at(widgets.size() - 1);
        widget->setObjectName(description);
        widget->setProperty("type", std::get<0>(config));
        widget->setToolTip(QString::fromStdString(std::get<2>(config)));
    }
}

QString NodeModel::caption() const
{
    return _name;
}

QString NodeModel::name() const
{
    return _name;
}

unsigned int NodeModel::nPorts(PortType portType) const
{
    const auto& nodeInfo = nodeManager.registeredNodeTypes().find(_name.toStdString())->second;

    if (portType == PortType::In)
    {
        return static_cast<unsigned int>(nodeInfo.constructorEmpty()->nPorts(NAV::Node::PortType::In));
    }
    else if (portType == PortType::Out)
    {
        return static_cast<unsigned int>(nodeInfo.constructorEmpty()->nPorts(NAV::Node::PortType::Out));
    }

    return 0;
}

NodeDataType NodeModel::dataType(PortType portType, PortIndex portIndex) const
{
    const auto& nodeInfo = nodeManager.registeredNodeTypes().find(_name.toStdString())->second;

    uint8_t port = static_cast<uint8_t>(portIndex);

    if (portType == PortType::In)
    {
        return { QString::fromStdString(std::string(nodeInfo.constructorEmpty()->dataType(NAV::Node::PortType::In, port))),
                 QString::fromStdString(std::string(nodeInfo.constructorEmpty()->dataType(NAV::Node::PortType::In, port))) };
    }
    else if (portType == PortType::Out)
    {
        return { QString::fromStdString(std::string(nodeInfo.constructorEmpty()->dataType(NAV::Node::PortType::Out, port))),
                 QString::fromStdString(std::string(nodeInfo.constructorEmpty()->dataType(NAV::Node::PortType::Out, port))) };
    }

    return NodeDataType();
}

QJsonObject NodeModel::save() const
{
    QJsonObject modelJson = NodeDataModel::save();

    for (size_t i = 0; i < widgets.size(); i++)
    {
        if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_BOOL)
            modelJson[widgets.at(i)->objectName()] = static_cast<QCheckBox*>(widgets.at(i))->isChecked();
        else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_INT)
            modelJson[widgets.at(i)->objectName()] = static_cast<QSpinBox*>(widgets.at(i))->value();
        else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_FLOAT)
            modelJson[widgets.at(i)->objectName()] = static_cast<QDoubleSpinBox*>(widgets.at(i))->value();
        else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_STRING)
            modelJson[widgets.at(i)->objectName()] = static_cast<QLineEdit*>(widgets.at(i))->text();
        else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_LIST)
            modelJson[widgets.at(i)->objectName()] = static_cast<QComboBox*>(widgets.at(i))->currentText();
        else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_LIST_LIST_INT)
        {
            auto gridGroupBox = static_cast<QGroupBox*>(widgets.at(i));
            auto layout = static_cast<QGridLayout*>(gridGroupBox->layout());
            std::string json;
            for (int j = 1; j < layout->rowCount(); j++)
            {
                QComboBox* xlist = static_cast<QComboBox*>(layout->itemAtPosition(j, 0)->widget());
                QComboBox* ylist = static_cast<QComboBox*>(layout->itemAtPosition(j, 1)->widget());
                QSpinBox* spinBox = static_cast<QSpinBox*>(layout->itemAtPosition(j, 2)->widget());

                if (spinBox->value() != -1)
                {
                    if (!json.empty())
                        json += ";";

                    json += xlist->currentText().toStdString() + "|" + ylist->currentText().toStdString() + "|" + std::to_string(spinBox->value());
                }
            }
            modelJson[widgets.at(i)->objectName()] = QString::fromStdString(json);
        }
        else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_MAP_INT)
            modelJson[widgets.at(i)->objectName()] = static_cast<QSpinBox*>(widgets.at(i))->value();
    }

    return modelJson;
}

void NodeModel::restore(QJsonObject const& p)
{
    for (size_t i = 0; i < widgets.size(); i++)
    {
        QJsonValue v = p[widgets.at(i)->objectName()];
        if (!v.isUndefined())
        {
            if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_BOOL)
                static_cast<QCheckBox*>(widgets.at(i))->setChecked(v.toBool());
            else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_INT)
                static_cast<QSpinBox*>(widgets.at(i))->setValue(v.toInt());
            else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_FLOAT)
                static_cast<QDoubleSpinBox*>(widgets.at(i))->setValue(v.toDouble());
            else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_STRING)
                static_cast<QLineEdit*>(widgets.at(i))->setText(v.toString());
            else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_LIST)
                static_cast<QComboBox*>(widgets.at(i))->setCurrentText(v.toString());
            else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_LIST_LIST_INT)
            {
                auto gridGroupBox = static_cast<QGroupBox*>(widgets.at(i));
                auto layout = static_cast<QGridLayout*>(gridGroupBox->layout());

                std::string json = v.toString().toStdString();

                std::stringstream lineStream(json);
                std::string line;
                for (int j = 1; std::getline(lineStream, line, ';'); j++)
                {
                    std::stringstream cellStream(line);
                    std::string cell;
                    for (int k = 0; std::getline(cellStream, cell, '|'); k++)
                    {
                        if (k <= 1)
                            static_cast<QComboBox*>(layout->itemAtPosition(j, k)->widget())->setCurrentText(QString::fromStdString(cell));
                        else if (k == 2)
                            static_cast<QSpinBox*>(layout->itemAtPosition(j, k)->widget())->setValue(std::stoi(cell));
                    }
                }
            }
            else if (widgets.at(i)->property("type").toUInt() == NAV::Node::ConfigOptions::CONFIG_MAP_INT)
                static_cast<QSpinBox*>(widgets.at(i))->setValue(v.toInt());
        }
    }
}

QWidget* NodeModel::embeddedWidget()
{
    return _mainWidget;
}

std::shared_ptr<NodeData> NodeModel::outData(PortIndex)
{
    return nullptr;
}

void NodeModel::setInData(std::shared_ptr<NodeData>, int) {}