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

#ifndef QKNXNETIPSEARCHREQUEST_H
#define QKNXNETIPSEARCHREQUEST_H

#include <QtKnx/qknxnetipframe.h>
#include <QtKnx/qknxnetiphpai.h>
#include <QtKnx/qtknxglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class QKnxNetIpSearchRequestExtendedBuilderPrivate;
class Q_KNX_EXPORT QKnxNetIpSearchRequestProxy final
{
public:
    QKnxNetIpSearchRequestProxy() = delete;
    ~QKnxNetIpSearchRequestProxy() = default;

    QKnxNetIpSearchRequestProxy(const QKnxNetIpFrame &&) = delete;
    explicit QKnxNetIpSearchRequestProxy(const QKnxNetIpFrame &frame);

    bool isValid() const;
    bool isExtended() const;

    QKnxNetIpHpai discoveryEndpoint() const;
    QList<QKnxNetIpSrp> extendedSearchParameters() const;

    class Q_KNX_EXPORT Builder final
    {
    public:
        Builder &setDiscoveryEndpoint(const QKnxNetIpHpai &hpai);
        QKnxNetIpFrame create() const;

    private:
        QKnxNetIpHpai m_hpai;
    };
    static QKnxNetIpSearchRequestProxy::Builder builder();

    class Q_KNX_EXPORT ExtendedBuilder final
    {
    public:
        ExtendedBuilder();
        ~ExtendedBuilder();

        ExtendedBuilder &setDiscoveryEndpoint(const QKnxNetIpHpai &hpai);
        ExtendedBuilder &setExtendedParameters(const QList<QKnxNetIpSrp> &srps);

        QKnxNetIpFrame create() const;

        ExtendedBuilder(const ExtendedBuilder &other);
        ExtendedBuilder &operator=(const ExtendedBuilder &other);

    private:
        QSharedDataPointer<QKnxNetIpSearchRequestExtendedBuilderPrivate> d_ptr;
    };
    static QKnxNetIpSearchRequestProxy::ExtendedBuilder extendedBuilder();

private:
    const QKnxNetIpFrame &m_frame;
};

QT_END_NAMESPACE

#endif
