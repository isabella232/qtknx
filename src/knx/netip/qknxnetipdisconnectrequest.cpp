/******************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#include "qknxnetipdisconnectrequest.h"

QT_BEGIN_NAMESPACE

QKnxNetIpDisconnectRequestProxy::QKnxNetIpDisconnectRequestProxy(const QKnxNetIpFrame &frame)
    : m_frame(frame)
{}

quint8 QKnxNetIpDisconnectRequestProxy::channelId() const
{
    return m_frame.constData().value(0);
}

QKnxNetIpHpai QKnxNetIpDisconnectRequestProxy::controlEndpoint() const
{
    return QKnxNetIpHpai::fromBytes(m_frame.constData(), 2);
}

bool QKnxNetIpDisconnectRequestProxy::isValid() const
{
    return m_frame.isValid() && m_frame.size() == 16
         && m_frame.serviceType() == QKnxNetIp::ServiceType::DisconnectRequest;
}

QKnxNetIpDisconnectRequestProxy::Builder QKnxNetIpDisconnectRequestProxy::builder()
{
    return QKnxNetIpDisconnectRequestProxy::Builder();
}


// -- QKnxNetIpDisconnectRequestProxy::Builder

QKnxNetIpDisconnectRequestProxy::Builder &
    QKnxNetIpDisconnectRequestProxy::Builder::setChannelId(quint8 channelId)
{
    m_channelId = channelId;
    return *this;
}

QKnxNetIpDisconnectRequestProxy::Builder &
    QKnxNetIpDisconnectRequestProxy::Builder::setControlEndpoint(const QKnxNetIpHpai &hpai)
{
    m_hpai = hpai;
    return *this;
}

QKnxNetIpFrame QKnxNetIpDisconnectRequestProxy::Builder::create() const
{
    return { QKnxNetIp::ServiceType::DisconnectRequest, QKnxByteArray { m_channelId, 0x00 }
        + m_hpai.bytes() };
}

QT_END_NAMESPACE
