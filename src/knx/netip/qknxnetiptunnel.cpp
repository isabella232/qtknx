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

#include "qknxnetipconnectresponse.h"
#include "qknxnetipendpointconnection_p.h"
#include "qknxnetiptunnel.h"
#include "qknxnetiptunnelingrequest.h"

QT_BEGIN_NAMESPACE

/*!
    \class QKnxNetIpTunnel

    \inmodule QtKnx
    \ingroup qtknx-tunneling
    \ingroup qtknx-netip

    \brief The QKnxNetIpTunnel class enables the opening and handling
    of a KNXnet/IP client connection to a KNXnet/IP server.

    A \e tunnel is a data connection that is established from a KNXnet/IP client
    to a KNXnet/IP server endpoint to access functionalities of devices on a KNX
    bus. The IP address of the client must be set. The client uses the
    connection to send link layer frames to the server.

    The following code sample illustrates how to connect to the server, request
    a data connection, and use the connection:

    \code
        QKnxNetIpTunnel tunnel;
        QHostAddress clientLocalAddress = ...
        tunnel.setLocalAddress(clientLocalAddress);

        QHostAddress knxNetIpServerAddress = ...
        quint16 knxNetIpServerDataEndPointPort = ...
        tunnel.connectToHost(knxNetIpServerAddress, knxNetIpServerDataEndPointPort);

        QKnxLinkLayerFrame frame = ...
        tunnel.sendFrame(frame);
    \endcode

    \sa QKnxLinkLayerFrame, {Qt KNX Tunneling Classes},
        {Qt KNXnet/IP Connection Classes}
*/

/*!
    \fn void QKnxNetIpTunnel::frameReceived(QKnxLinkLayerFrame frame)

    This signal is emitted when the KNXnet/IP client receives data (with the
    link layer frame \a frame as payload) from the KNXnet/IP server.
*/

class QKnxNetIpTunnelPrivate : public QKnxNetIpEndpointConnectionPrivate
{
    Q_DECLARE_PUBLIC(QKnxNetIpTunnel)

public:
    QKnxNetIpTunnelPrivate(const QHostAddress &a, quint16 p, QKnxNetIp::TunnelLayer l)
        : QKnxNetIpEndpointConnectionPrivate(a, p, QKnxNetIpCriProxy::builder().setTunnelLayer(l)
            .create(), 1, QKnxNetIp::TunnelingRequestTimeout)
        , m_layer(l)
    {}

    void process(const QKnxLinkLayerFrame &frame) override
    {
        Q_Q(QKnxNetIpTunnel);
        emit q->frameReceived(frame);
    }

    void processConnectResponse(const QKnxNetIpFrame &frame, const QNetworkDatagram &dg) override
    {
        QKnxNetIpConnectResponseProxy response(frame);
        if (response.status() == QKnxNetIp::Error::NoMoreUniqueConnections) {
            Q_ASSERT_X(false, "QKnxNetIpTunnelPrivate::process", "NoMoreUniqueConnections "
                "error handling not implemented yet.");
            // TODO: Maybe implement 03_08_04 Tunneling v01.05.03 AS.pdf, paragraph 3.3
        }

        Q_Q(QKnxNetIpTunnel);
        if (q->state() != QKnxNetIpTunnel::Connected) {
            const auto &crd = response.responseData();
            m_address = QKnxNetIpCrdProxy(crd).individualAddress();
        }
        QKnxNetIpEndpointConnectionPrivate::processConnectResponse(frame, dg);
    }

private:
    QKnxAddress m_address;
    QKnxNetIp::TunnelLayer m_layer { QKnxNetIp::TunnelLayer::Unknown };
};

/*!
    Creates a tunnel connection with the parent \a parent.
*/
QKnxNetIpTunnel::QKnxNetIpTunnel(QObject *parent)
    : QKnxNetIpTunnel({ QHostAddress::LocalHost }, 0, QKnxNetIp::TunnelLayer::Link,
        parent)
{}

/*!
    Creates a tunnel connection with the KNXnet/IP client address
    \a localAddress and parent \a parent.
*/
QKnxNetIpTunnel::QKnxNetIpTunnel(const QHostAddress &localAddress,
        QObject *parent)
    : QKnxNetIpTunnel(localAddress, 0, QKnxNetIp::TunnelLayer::Link, parent)
{}

/*!
    Creates a tunnel connection with the KNXnet/IP client address
    \a localAddress, port number \a localPort, and parent \a parent.
*/
QKnxNetIpTunnel::QKnxNetIpTunnel(const QHostAddress &localAddress,
        quint16 localPort, QObject *parent)
    : QKnxNetIpTunnel(localAddress, localPort, QKnxNetIp::TunnelLayer::Link, parent)
{}

/*!
    Creates a tunnel connection with the KNXnet/IP client address
    \a localAddress, port number \a localPort, layer \a layer, and
    parent \a parent.
*/
QKnxNetIpTunnel::QKnxNetIpTunnel(const QHostAddress &localAddress,
        quint16 localPort, QKnxNetIp::TunnelLayer layer, QObject *parent)
    : QKnxNetIpEndpointConnection(*new QKnxNetIpTunnelPrivate(localAddress, localPort,
        layer), parent)
{}

/*!
    Returns the individual address of the KNXnet/IP client.
*/
QKnxAddress QKnxNetIpTunnel::individualAddress() const
{
    return d_func()->m_address;
}

/*!
    \internal

    Sets the individual address of the KNXnet/IP client to \a address.
*/
void QKnxNetIpTunnel::setIndividualAddress(const QKnxAddress &address)
{
    Q_ASSERT_X(false, "QKnxNetIpTunnelClient::setIndividualAddress", "Setting the individual "
        "address used for tunnel connections not implemented yet.");

    Q_UNUSED(address) // TODO: Maybe implement 03_08_04 Tunneling v01.05.03 AS.pdf, paragraph 3.2
}

/*!
    Returns the layer used for the tunnel connection.
*/
QKnxNetIp::TunnelLayer QKnxNetIpTunnel::layer() const
{
    return d_func()->m_layer;
}

/*!
    Sets the layer used for the tunnel connection to \a layer.

    The layer is changed only if no connection is currently established. The new
    layer is set the next time a connection is created.

    \sa QKnxNetIpEndpointConnection::State
*/
void QKnxNetIpTunnel::setTunnelLayer(QKnxNetIp::TunnelLayer layer)
{
    if (state() != State::Disconnected)
        return;
    d_func()->m_layer = layer;
}

/*!
    Inserts the link layer frame \a frame into a tunneling request that is sent
    to a KNXnet/IP server.

    If the tunnel runs in bus monitor mode, no frames can be sent to the bus.

    If no connection is currently established, returns \c false and does not
    send the frame.

    \sa QKnxNetIpEndpointConnection::State
*/
bool QKnxNetIpTunnel::sendFrame(const QKnxLinkLayerFrame &frame)
{
    if (state() != State::Connected)
        return false;

    Q_D(QKnxNetIpTunnel);
    if (d->m_layer == QKnxNetIp::TunnelLayer::Busmonitor)
        return false; // 03_08_04 Tunneling v01.05.03, paragraph 2.4

    return d->sendTunnelingRequest(frame);
}

QT_END_NAMESPACE
