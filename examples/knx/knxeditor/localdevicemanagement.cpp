/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtKnx module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "localdevicemanagement.h"
#include "ui_localdevicemanagement.h"

#include <QKnxByteArray>
#include <QKnxDeviceManagementFrameBuilder>
#include <QKnxInterfaceObjectPropertyDataType>
#include <QKnxNetIpSecureConfiguration>
#include <QMetaEnum>
#include <QMetaType>
#include <QTreeWidget>
#include <QRegularExpressionValidator>

LocalDeviceManagement::LocalDeviceManagement(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LocalDeviceManagement)
{
    ui->setupUi(this);

    setupMessageCodeComboBox();

    const auto setTreeWidget = [](QComboBox *comboBox) {
        auto treeWidget = new QTreeWidget;
        treeWidget->setHeaderHidden(true);
        treeWidget->setUniformRowHeights(true);

        comboBox->setModel(treeWidget->model());
        comboBox->setView(treeWidget);
    };
    setTreeWidget(ui->property);
    setTreeWidget(ui->objectType);

    setupComboBox(ui->property, QKnxInterfaceObjectProperty::staticMetaObject);
    setupComboBox(ui->objectType, QKnxInterfaceObjectType::staticMetaObject);

    connect(ui->connectRequestDeviceManagement, &QPushButton::clicked, this, [&]() {
        m_management.setLocalPort(0);
        if (ui->secureSessionCheckBox->isChecked()) {
            auto config = m_configs.value(ui->secureSessionCb->currentIndex());
            config.setKeepSecureSessionAlive(true);
            m_management.setSecureConfiguration(config);
            m_management.connectToHostEncrypted(m_server.controlEndpointAddress(),
                m_server.controlEndpointPort());
        } else {
            m_management.connectToHost(m_server.controlEndpointAddress(),
                m_server.controlEndpointPort(), m_proto);
        }
    });

    connect(&m_management, &QKnxNetIpDeviceManagement::connected, this, [&] {
        ui->deviceManagementSendRequest->setEnabled(true);
        ui->connectRequestDeviceManagement->setEnabled(false);
        ui->disconnectRequestDeviceManagement->setEnabled(true);
        ui->textOuputDeviceManagement->append(tr("Successfully connected to: %1 on port: %2")
            .arg(m_server.controlEndpointAddress().toString()).arg(m_server.controlEndpointPort()));

        m_management.sendFrame(QKnxDeviceManagementFrame::propertyReadBuilder()
            .setObjectType(QKnxInterfaceObjectType::System::Device)
            .setObjectInstance(1)
            .setProperty(QKnxInterfaceObjectProperty::Device::IoList)
            .setNumberOfElements(1)
            .setStartIndex(0).createRequest());
    });

    connect(ui->disconnectRequestDeviceManagement, &QPushButton::clicked, this, [&]() {
        m_management.disconnectFromHost();
    });

    connect(&m_management, &QKnxNetIpDeviceManagement::disconnected, this, [&] {
        if (!ui)
            return;
        m_awaitIoListResponse = true;
        ui->deviceManagementSendRequest->setEnabled(false);
        ui->connectRequestDeviceManagement->setEnabled(true);
        ui->disconnectRequestDeviceManagement->setEnabled(false);
        setupComboBox(ui->objectType, QKnxInterfaceObjectType::staticMetaObject);
        ui->textOuputDeviceManagement->append(tr("Successfully disconnected from: %1 on port: %2\n")
            .arg(m_server.controlEndpointAddress().toString()).arg(m_server.controlEndpointPort()));
    });

    connect(ui->deviceManagementSendRequest, &QPushButton::clicked, this, [&]() {
        ui->textOuputDeviceManagement->append(tr("Send device management frame with cEMI payload: ")
            + ui->cemiFrame->text());
        auto data = QKnxByteArray::fromHex(ui->cemiFrame->text().toUtf8());
        if (ui->cemiData->isEnabled())
            data.append(QKnxByteArray::fromHex(ui->cemiData->text().toUtf8()));
        m_management.sendFrame(QKnxDeviceManagementFrame::fromBytes(data, 0,
            data.size()));
    });

    connect(&m_management, &QKnxNetIpDeviceManagement::frameReceived,
        this, [&](QKnxDeviceManagementFrame frame) {
        ui->textOuputDeviceManagement->append(tr("Received device management frame with cEMI "
            "payload: " + frame.bytes().toHex().toByteArray()));

        if (m_awaitIoListResponse)
            handleIoListResponse(frame);
    });

    connect(&m_management, &QKnxNetIpDeviceManagement::errorOccurred, this,
        [&] (QKnxNetIpEndpointConnection::Error, QString errorString) {
            ui->textOuputDeviceManagement->append(errorString);
    });

    ui->cemiData->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9a-fA-F]+")));
    ui->cemiFrame->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9a-fA-F]+")));
}

LocalDeviceManagement::~LocalDeviceManagement()
{
    delete ui;
    ui = nullptr;
}

void LocalDeviceManagement::setNatAware(bool isNatAware)
{
    m_management.setNatAware(isNatAware);
}

void LocalDeviceManagement::setLocalAddress(const QHostAddress &address)
{
    m_management.disconnectFromHost();
    m_management.setLocalAddress(address);
}

void LocalDeviceManagement::setKnxNetIpServer(const QKnxNetIpServerInfo &server)
{
    m_management.disconnectFromHost();
    m_server = server;

    if (m_management.state() == QKnxNetIpEndpointConnection::State::Disconnected) {
        ui->connectRequestDeviceManagement->setEnabled(true);
        ui->disconnectRequestDeviceManagement->setEnabled(false);
    }

    updateSecureConfigCombo();
    ui->deviceManagementSendRequest->setEnabled(false);
}

void LocalDeviceManagement::setTcpEnable(bool value)
{
    m_proto = (value ? QKnxNetIp::HostProtocol::TCP_IPv4 : QKnxNetIp::HostProtocol::UDP_IPv4);
}

void LocalDeviceManagement::clearLogging()
{
    ui->textOuputDeviceManagement->clear();
}

void LocalDeviceManagement::on_mc_currentIndexChanged(int index)
{
    if (ui->cemiFrame->text().size() > 2)
        m_fullCemiFrame = ui->cemiFrame->text();

    int maxLength = 10;
    bool dataEnabled = true;
    auto cemiFrame = m_fullCemiFrame;

    quint8 data = ui->mc->itemData(index).toUInt();
    switch (QKnxDeviceManagementFrame::MessageCode(data)) {
    case QKnxDeviceManagementFrame::MessageCode::PropertyReadRequest:
        dataEnabled = false;
    case QKnxDeviceManagementFrame::MessageCode::PropertyWriteRequest:
        maxLength = 14;
    case QKnxDeviceManagementFrame::MessageCode::FunctionPropertyCommandRequest:
    case QKnxDeviceManagementFrame::MessageCode::FunctionPropertyStateReadRequest:
        if (cemiFrame.size() < maxLength) {
            cemiFrame.append(QStringLiteral("%1").arg(ui->noe->value(), 1, 16, QLatin1Char('0')));
            cemiFrame.append(QStringLiteral("%1").arg(ui->startIndex->value(), 3, 16, QLatin1Char('0')));
        }
        cemiFrame = QStringLiteral("%1").arg(data, 2, 16, QLatin1Char('0')) + cemiFrame.mid(2);
        break;
    case QKnxDeviceManagementFrame::MessageCode::ResetRequest:
        maxLength = 2;
        dataEnabled = false;
        cemiFrame = QStringLiteral("%1").arg(data, 2, 16, QLatin1Char('0'));
        break;
    default:
        break;
    }
    ui->cemiFrame->setMaxLength(maxLength);
    ui->cemiFrame->setText(cemiFrame);
    ui->cemiData->setEnabled(dataEnabled);
}

void LocalDeviceManagement::on_objectType_currentTextChanged(const QString &type)
{
    bool keyExists = false;
    quint16 value = keyToValue(QKnxInterfaceObjectType::staticMetaObject, type, &keyExists);
    if (keyExists) {
        auto text = ui->cemiFrame->text();
        ui->cemiFrame->setText(text.left(2) + QStringLiteral("%1").arg(value, 4, 16,
            QLatin1Char('0')) + text.mid(6));
        updatePropertyTypeCombobox(type);
    }
}

void LocalDeviceManagement::on_property_currentTextChanged(const QString &property)
{
    bool keyExists = false;
    quint8 value = keyToValue(QKnxInterfaceObjectProperty::staticMetaObject, property, &keyExists);
    if (keyExists) {
        auto text = ui->cemiFrame->text();
        ui->cemiFrame->setText(text.left(8) + QStringLiteral("%1").arg(value, 2, 16,
            QLatin1Char('0')) + text.mid(10));
    }
}

void LocalDeviceManagement::on_noe_valueChanged(int value)
{
    auto text = ui->cemiFrame->text();
    ui->cemiFrame->setText(text.left(10) + QStringLiteral("%1").arg(value, 1,
        16, QLatin1Char('0')) + text.mid(11));
}

void LocalDeviceManagement::on_startIndex_valueChanged(int value)
{
    auto text = ui->cemiFrame->text();
    ui->cemiFrame->setText(text.left(11) + QStringLiteral("%1").arg(value, 3, 16,
        QLatin1Char('0')));
}

void LocalDeviceManagement::on_objectInstance_valueChanged(int value)
{
    auto text = ui->cemiFrame->text();
    ui->cemiFrame->setText(text.left(6) + QStringLiteral("%1").arg(value, 2, 16,
        QLatin1Char('0')) + text.mid(8));
}

void LocalDeviceManagement::on_manualInput_clicked(bool checked)
{
    ui->cemiData->setEnabled(checked);
    ui->cemiFrame->setReadOnly(!checked);
    ui->cemiFrame->setMaxLength(SHRT_MAX);
    ui->cemiFrame->setFocus();

    if (checked) {
        if (ui->mc->currentIndex() != ui->mc->count() -1)
            m_fullCemiFrame = ui->cemiFrame->text();
    } else {
        ui->cemiFrame->setText(m_fullCemiFrame);
        on_mc_currentIndexChanged(ui->mc->currentIndex());
    }
}

void LocalDeviceManagement::onKeyringChanged(const QList<QKnxNetIpSecureConfiguration> &configs)
{
    m_configs = configs;
    updateSecureConfigCombo();
}

void LocalDeviceManagement::setupMessageCodeComboBox()
{
    ui->mc->addItem("M_PropRead.req",
        quint8(QKnxDeviceManagementFrame::MessageCode::PropertyReadRequest));
    ui->mc->addItem("M_PropWrite.req",
        quint8(QKnxDeviceManagementFrame::MessageCode::PropertyWriteRequest));
    ui->mc->addItem("M_FuncPropCommand.req",
        quint8(QKnxDeviceManagementFrame::MessageCode::FunctionPropertyCommandRequest));
    ui->mc->addItem("M_FuncPropStateRead.req",
        quint8(QKnxDeviceManagementFrame::MessageCode::FunctionPropertyStateReadRequest));
    ui->mc->addItem("M_Reset.req",
        quint8(QKnxDeviceManagementFrame::MessageCode::ResetRequest));
}

void LocalDeviceManagement::updatePropertyTypeCombobox(const QString &type)
{
    int index = QKnxInterfaceObjectProperty::staticMetaObject.indexOfEnumerator("General");
    if (index >= 0) {
        auto treeView = qobject_cast<QTreeWidget *>(ui->property->view());
        if (!treeView)
            return;

        auto topLevelItem = treeView->takeTopLevelItem(0);
        ui->property->clear();
        treeView->addTopLevelItem(topLevelItem);

        auto index = QKnxInterfaceObjectProperty::staticMetaObject.indexOfEnumerator(type.toLatin1());
        if (index >= 0) {
            auto typeEnum = QKnxInterfaceObjectProperty::staticMetaObject.enumerator(index);
            topLevelItem = new QTreeWidgetItem(treeView, { typeEnum.name() });
            for (auto a = 0; a < typeEnum.keyCount(); ++a)
                topLevelItem->addChild(new QTreeWidgetItem({ typeEnum.key(a) }));
            topLevelItem->setFlags(topLevelItem->flags() &~Qt::ItemIsSelectable);
        }
        treeView->expandItem(topLevelItem);
        selectFirstSubitem(treeView, topLevelItem, ui->property);
    }
}

void LocalDeviceManagement::handleIoListResponse(const QKnxDeviceManagementFrame &frame)
{
    if (frame.objectType() != QKnxInterfaceObjectType::System::Device
        || frame.property() != QKnxInterfaceObjectProperty::Device::IoList) {
            return;
    }

    if (frame.isNegativeConfirmation()) {
        auto metaEnum = QMetaEnum::fromType<QKnxNetIp::CemiServer::Error>();
        ui->textOuputDeviceManagement->append(tr("Received negative confirmation. Error code: %1")
            .arg(QString::fromLatin1(metaEnum.valueToKey(int(frame.error())))));
        m_awaitIoListResponse = false;
        return;
    }

    auto dataTypes = QKnxInterfaceObjectPropertyDataType::fromProperty(QKnxInterfaceObjectProperty
        ::Device::IoList);
    if (!dataTypes.value(0).isValid())
        return;

    auto data = frame.data().toByteArray();
    quint8 expectedDataSize = dataTypes[0].size();
    if (frame.startIndex() == 0) {
        if (data.size() == expectedDataSize) {
            m_management.sendFrame(QKnxDeviceManagementFrame::propertyReadBuilder()
                .setObjectType(QKnxInterfaceObjectType::System::Device)
                .setObjectInstance(1)
                .setProperty(QKnxInterfaceObjectProperty::Device::IoList)
                .setNumberOfElements(data.toHex().toUShort(nullptr, 16))
                .setStartIndex(1)
                .createRequest()
            );
        }
    } else {
        if ((data.size() % expectedDataSize) == 0) {
            QSet<int> values;
            for (int i = 0; i < data.size(); i += expectedDataSize)
                values.insert(data.mid(i, expectedDataSize).toHex().toUShort(nullptr, 16));

            m_awaitIoListResponse = false;
            setupComboBox(ui->objectType, QKnxInterfaceObjectType::staticMetaObject, values);
        }
    }
}

int LocalDeviceManagement::keyToValue(const QMetaObject &object, const QString &key, bool *ok)
{
    auto enumCount = object.enumeratorCount();
    for (auto i = 0; i < enumCount; ++i) {
        int value = object.enumerator(i).keyToValue(key.toLatin1(), ok);
        if (value != -1 && *ok)
            return value;
    }
    return -1;
}

void LocalDeviceManagement::setupComboBox(QComboBox *comboBox, const QMetaObject &object,
    const QSet<int> &values)
{
    comboBox->clear();

    auto treeWidget = qobject_cast<QTreeWidget *> (comboBox->view());
    if (!treeWidget)
        return;

    QTreeWidgetItem *rootItem = nullptr;
    auto enumCount = object.enumeratorCount();
    for (auto i = 0; i < enumCount; ++i) {
        auto typeEnum = object.enumerator(i);
        auto topLevelItem = new QTreeWidgetItem(treeWidget, { typeEnum.name() });
        if (!rootItem)
            rootItem = topLevelItem;
        for (auto a = 0; a < typeEnum.keyCount(); ++a) {
            auto subItem = new QTreeWidgetItem(topLevelItem, { typeEnum.key(a) });
            subItem->setDisabled(values.isEmpty() ? false : !values.contains(typeEnum.value(a)));
        }
        topLevelItem->setFlags(topLevelItem->flags() &~ Qt::ItemIsSelectable);
    }
    treeWidget->expandItem(rootItem);
    selectFirstSubitem(treeWidget, rootItem, comboBox);
}

void LocalDeviceManagement::selectFirstSubitem(QTreeWidget *treeWidget, QTreeWidgetItem *rootItem,
    QComboBox *comboBox)
{
    treeWidget->setCurrentItem(rootItem, 0);
    comboBox->setRootModelIndex(treeWidget->currentIndex());
    comboBox->setCurrentIndex(0);
    treeWidget->setCurrentItem(treeWidget->invisibleRootItem(), 0);
    comboBox->setRootModelIndex(treeWidget->currentIndex());
}

void LocalDeviceManagement::updateSecureConfigCombo()
{
    ui->secureSessionCb->clear();

    ui->secureSessionCheckBox->setEnabled(!m_configs.isEmpty());
    ui->secureSessionCheckBox->setChecked(m_proto == QKnxNetIp::HostProtocol::TCP_IPv4);

    for (int i = 0; i < m_configs.size(); ++i) {
        const auto &config = m_configs[i];
        if (m_server.individualAddress() != config.host())
            continue;

        ui->secureSessionCb->addItem(tr("User ID: %1 (Individual Address: %2)").arg(config.userId())
            .arg(config.individualAddress().toString()), i);
    }
}
