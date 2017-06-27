/******************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtKnx module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#ifndef QKNXNETIPDISCOVERYAGENT_H
#define QKNXNETIPDISCOVERYAGENT_H

#include <QtCore/qobject.h>
#include <QtCore/qvector.h>
#include <QtKnx/qknxglobal.h>
#include <QtKnx/qknxnetipserverdiscoveryinfo.h>
#include <QtNetwork/qhostaddress.h>

QT_BEGIN_NAMESPACE

class QKnxNetIpServerDiscoveryAgentPrivate;

class Q_KNX_EXPORT QKnxNetIpServerDiscoveryAgent final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QKnxNetIpServerDiscoveryAgent)
    Q_DECLARE_PRIVATE(QKnxNetIpServerDiscoveryAgent)

public:
    enum class State : quint8
    {
        NotRunning,
        Starting,
        Running,
        Stopping
    };
    Q_ENUM(State)

    enum class Error : quint8
    {
        None,
        Network,
        NotIPv4,
        Unknown = 0x80
    };
    Q_ENUM(Error)

    enum class ResponseType : quint8
    {
        Unicast = 0x00,
        Multicast = 0x01
    };
    Q_ENUM(ResponseType)

    QKnxNetIpServerDiscoveryAgent(QObject *parent = nullptr);
    ~QKnxNetIpServerDiscoveryAgent();

    explicit QKnxNetIpServerDiscoveryAgent(const QHostAddress &localAddress,
        QObject *parent = nullptr);
    QKnxNetIpServerDiscoveryAgent(const QHostAddress &localAddress, quint16 localPort,
        QObject *parent = nullptr);

    QKnxNetIpServerDiscoveryAgent::State state() const;
    QKnxNetIpServerDiscoveryAgent::Error error() const;

    QString errorString() const;
    QVector<QKnxNetIpServerDiscoveryInfo> discoveredServers() const;

    quint16 localPort() const;
    void setLocalPort(quint16 port);

    QHostAddress localAddress() const;
    void setLocalAddress(const QHostAddress &address);

    int timeout() const;
    void setTimeout(int msec);

    int searchFrequency() const;
    void setSearchFrequency(int timesPerMinute);

    bool natAware() const;
    void setNatAware(bool isAware);

    quint8 multicastTtl() const;
    void setMulticastTtl(quint8 ttl);

    QKnxNetIpServerDiscoveryAgent::ResponseType responseType() const;
    void setResponseType(QKnxNetIpServerDiscoveryAgent::ResponseType type);

public Q_SLOTS:
    void start();
    void start(int timeout);
    void stop();

Q_SIGNALS:
    void started();
    void finished();

    void deviceDiscovered(QKnxNetIpServerDiscoveryInfo server);
    void stateChanged(QKnxNetIpServerDiscoveryAgent::State state);
    void errorOccurred(QKnxNetIpServerDiscoveryAgent::Error error, QString errorString);

private:
    QKnxNetIpServerDiscoveryAgent(QKnxNetIpServerDiscoveryAgentPrivate &dd, QObject *parent);
};

QT_END_NAMESPACE

#endif