/******************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtKnx module.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#ifndef TUNNELINGFEATURES_H
#define TUNNELINGFEATURES_H

#include <QKnxNetIpTunnel>
#include <QKnxNetIpServerInfo>
#include <QKnxLinkLayerFrame>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class TunnelingFeatures;
}
QT_END_NAMESPACE

class TunnelingFeatures : public QWidget
{
    Q_OBJECT

public:
    explicit TunnelingFeatures(QWidget *parent = nullptr);
    ~TunnelingFeatures();

    void setNatAware(bool isNatAware);
    void setLocalAddress(const QHostAddress &address);
    void setKnxNetIpServer(const QKnxNetIpServerInfo &server);
    void setTcpEnable(bool value);
    void onKeyringChanged(const QList<QKnxNetIpSecureConfiguration> &configs);

private:
    void checkFeatureValue();
    void updateSecureConfigCombo();

private:
    Ui::TunnelingFeatures *ui { nullptr };

    QKnxNetIpServerInfo m_server;
    QKnxNetIpTunnel m_tunnel;

    QKnxNetIp::HostProtocol m_protocol = { QKnxNetIp::HostProtocol::UDP_IPv4 };
    QList<QKnxNetIpSecureConfiguration> m_configs;
};

#endif // TUNNELINGFEATURES_H
