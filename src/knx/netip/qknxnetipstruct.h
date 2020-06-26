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

#ifndef QKNXNETIPSTRUCT_H
#define QKNXNETIPSTRUCT_H

#include <QtKnx/qknxbytearray.h>
#include <QtKnx/qknxnetip.h>
#include <QtKnx/qknxtraits.h>
#include <QtKnx/qknxnetipstructheader.h>

QT_BEGIN_NAMESPACE

template <typename CodeType> class QKnxNetIpStruct final
{
    static_assert(is_type<CodeType, QKnxNetIp::HostProtocol, QKnxNetIp::ConnectionType,
        QKnxNetIp::DescriptionType, QKnxNetIp::SearchParameterType>::value, "Type not supported.");

public:
    QKnxNetIpStruct() = default;
    ~QKnxNetIpStruct() = default;

    QKnxNetIpStruct(CodeType codeType, const QKnxByteArray &dataField = {})
        : m_header(codeType)
    {
         setData(dataField);
    }

    QKnxNetIpStruct(const QKnxNetIpStructHeader<CodeType> &headerField,
            const QKnxByteArray &dataField = {})
        : m_header(headerField)
        , m_data(dataField)
    {}

    bool isNull() const
    {
        return m_header.isNull() && m_data.isNull();
    }

    bool isValid() const
    {
        return m_header.isValid() && size() == (m_header.size() + m_data.size());
    }

    quint16 size() const
    {
        return m_header.totalSize();
    }

    quint16 dataSize() const
    {
        return m_header.dataSize();
    }

    QKnxNetIpStructHeader<CodeType> header() const
    {
        return m_header;
    }

    void setHeader(const QKnxNetIpStructHeader<CodeType> &headerField)
    {
        m_header = headerField;
    }

    CodeType code() const
    {
        return m_header.code();
    }

    QKnxByteArray data() const
    {
        return m_data;
    }

    const QKnxByteArray &constData() const
    {
        return m_data;
    }

    void setData(const QKnxByteArray &dataField)
    {
        m_data = dataField;
        m_header.setDataSize(dataField.size());
    }

    QKnxByteArray bytes() const
    {
        return m_header.bytes() + m_data;
    }

    static QKnxNetIpStruct fromBytes(const QKnxByteArray &bytes, quint16 index = 0)
    {
        auto header = QKnxNetIpStructHeader<CodeType>::fromBytes(bytes, index);
        if (!header.isValid())
            return {};
        return { header, bytes.mid(index + header.size(), header.dataSize()) };
    }

    bool operator==(const QKnxNetIpStruct &other) const
    {
        return (m_header == other.m_header) && (m_data == other.m_data);
    }

    bool operator!=(const QKnxNetIpStruct &other) const
    {
        return !operator==(other);
    }

private:
    QKnxNetIpStructHeader<CodeType> m_header;
    QKnxByteArray m_data;
};

#ifndef Q_CLANG_QDOC
    using QKnxNetIpHpai = QKnxNetIpStruct<QKnxNetIp::HostProtocol>;
    using QKnxNetIpCri = QKnxNetIpStruct<QKnxNetIp::ConnectionType>;
    using QKnxNetIpCrd = QKnxNetIpStruct<QKnxNetIp::ConnectionType>;
    using QKnxNetIpDib = QKnxNetIpStruct<QKnxNetIp::DescriptionType>;
    using QKnxNetIpSrp = QKnxNetIpStruct<QKnxNetIp::SearchParameterType>;
#else
    class QKnxNetIpHpai final {public:};
    class QKnxNetIpCri final {public:};
    class QKnxNetIpCrd final {public:};
    class QKnxNetIpDib final {public:};
    class QKnxNetIpSrp final {public:};
#endif

Q_KNX_EXPORT QDebug operator<<(QDebug debug, const QKnxNetIpStruct<QKnxNetIp::HostProtocol> &hpai);
Q_KNX_EXPORT QDebug operator<<(QDebug debug, const QKnxNetIpStruct<QKnxNetIp::ConnectionType> &cr);
Q_KNX_EXPORT QDebug operator<<(QDebug debug, const QKnxNetIpStruct<QKnxNetIp::DescriptionType> &dib);
Q_KNX_EXPORT QDebug operator<<(QDebug debug, const QKnxNetIpStruct<QKnxNetIp::SearchParameterType> &srp);

template <typename CodeType>
Q_DECL_PURE_FUNCTION size_t qHash(const QKnxNetIpStruct<CodeType> &key, uint seed = 0) Q_DECL_NOTHROW
{
    return qHash(key.bytes(), seed);
}

QT_END_NAMESPACE

#endif
