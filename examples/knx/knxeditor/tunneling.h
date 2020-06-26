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

#ifndef TUNNELING_H
#define TUNNELING_H

#include <QKnxControlField>
#include <QKnxExtendedControlField>
#include <QKnxLinkLayerFrame>
#include <QKnxNetIpTunnel>
#include <QKnxNetIpServerInfo>
#include <QRegularExpression>
#include <QValidator>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
    class Tunneling;
}

class QComboBox;
QT_END_NAMESPACE

class KnxAddressValidator : public QValidator
{
    Q_OBJECT

public:
    KnxAddressValidator(QLatin1Char delimiter, QObject *parent = nullptr);
    QValidator::State validate(QString &input, int &) const override;

private:
    QChar m_delimiter;
    QRegularExpression m_expr;
};

class Tunneling : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(Tunneling)

public:
    explicit Tunneling(QWidget* parent = nullptr);
    ~Tunneling();

    void setNatAware(bool isNatAware);
    void setLocalAddress(const QHostAddress &address);
    void setKnxNetIpServer(const QKnxNetIpServerInfo &server);
    void setTcpEnable(bool value);
    void onKeyringChanged(const QList<QKnxNetIpSecureConfiguration> &configs);

public slots:
    void clearLogging();

private slots:
    void updateFrame();
    void updateControlField();
    void updateExtendedControlField();
    void on_manualInput_clicked(bool checked);

private:
    void setupApciTpciComboBox();
    void setupMessageCodeComboBox();
    void updateAdditionalInfoTypesComboBox();
    void updateSecureConfigCombo();

private:
    Ui::Tunneling *ui { nullptr };

    QKnxControlField m_ctrl;
    QKnxExtendedControlField m_extCtrl;

    QKnxLinkLayerFrame m_frame;
    QKnxNetIpTunnel m_tunnel;
    QKnxNetIpServerInfo m_server;
    QKnxNetIp::HostProtocol m_proto { QKnxNetIp::HostProtocol::UDP_IPv4 };
    QList<QKnxNetIpSecureConfiguration> m_configs;

};

#endif
