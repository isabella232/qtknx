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

#ifndef QKNXNETIPSECUREDSERVICEFAMILIESDIB_H
#define QKNXNETIPSECUREDSERVICEFAMILIESDIB_H

#include <QtKnx/qtknxglobal.h>
#include <QtKnx/qknxnetipstruct.h>

QT_BEGIN_NAMESPACE

struct Q_KNX_EXPORT QKnxSecuredServiceInfo
{
    QKnxNetIp::ServiceFamily ServiceFamily;
    quint8 RequiredSecurityVersion;
};

class QKnxNetIpSecuredServiceFamiliesDibBuilderPrivate;
class Q_KNX_EXPORT QKnxNetIpSecuredServiceFamiliesDibProxy final
{
public:
    QKnxNetIpSecuredServiceFamiliesDibProxy() = delete;
    ~QKnxNetIpSecuredServiceFamiliesDibProxy() = default;

    QKnxNetIpSecuredServiceFamiliesDibProxy(const QKnxNetIpDib &&) = delete;
    explicit QKnxNetIpSecuredServiceFamiliesDibProxy(const QKnxNetIpDib &dib);

    bool isValid() const;

    QKnxNetIp::DescriptionType descriptionType() const;
    QList<QKnxSecuredServiceInfo> serviceInfos() const;

    class Q_KNX_EXPORT Builder final
    {
    public:
        Builder();
        ~Builder();

        Builder &setServiceInfos(const QList<QKnxSecuredServiceInfo> &infos);

        QKnxNetIpDib create() const;

        Builder(const Builder &other);
        Builder &operator=(const Builder &other);

    private:
        QSharedDataPointer<QKnxNetIpSecuredServiceFamiliesDibBuilderPrivate> d_ptr;
    };
    static QKnxNetIpSecuredServiceFamiliesDibProxy::Builder builder();

private:
    const QKnxNetIpDib &m_dib;
};

QT_END_NAMESPACE

#endif
