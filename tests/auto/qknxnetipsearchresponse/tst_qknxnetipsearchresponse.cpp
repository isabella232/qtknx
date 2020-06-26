/******************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtKnx module.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
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
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#include <QtCore/qdebug.h>
#include <QtTest/qtest.h>
#include <QtKnx/qknxnetipsearchresponse.h>
#include <QtKnx/qknxnetipconfigdib.h>

static QString s_msg;
static void myMessageHandler(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    s_msg = msg;
}

class tst_QKnxNetIpSearchResponse : public QObject
{
    Q_OBJECT

private slots:
    void testDefaultConstructor();
    void testConstructor();
    void testSearchResponseEndpoint();
    void testDebugStream();
    void testSupportedFamiliesVersions();
    void testExtendedResponse();
    void testExtendedFramesSupportedFamilies();
    void testExtendedFramesDeviceHardware();
    void testDibsShallBePresentOnce();
};

void tst_QKnxNetIpSearchResponse::testDefaultConstructor()
{
    QKnxNetIpFrame frame;
    QKnxNetIpSearchResponseProxy search(frame);

    QCOMPARE(search.isValid(), false);
    QCOMPARE(search.controlEndpoint().isValid(), false);
    QCOMPARE(search.deviceHardware().isValid(), false);
    QCOMPARE(search.supportedFamilies().isValid(), false);
}

void tst_QKnxNetIpSearchResponse::testConstructor()
{
    auto endpoint = QKnxNetIpHpaiProxy::builder().setHostAddress(QHostAddress::LocalHost)
        .setPort(3671).create();
    auto hardware = QKnxNetIpDeviceDibProxy::builder()
        .setMediumType(QKnx::MediumType::NetIP)
        .setDeviceStatus(QKnxNetIp::ProgrammingMode::Active)
        .setIndividualAddress(QKnxAddress::Individual::Unregistered)
        .setProjectInstallationId(0x1111)
        .setSerialNumber(QKnxByteArray::fromHex("123456123456"))
        .setMulticastAddress(QHostAddress::AnyIPv4)
        .setMacAddress(QKnxByteArray::fromHex("bcaec56690f9"))
        .setDeviceName(QByteArray("qt.io KNX device"))
        .create();
    auto families = QKnxNetIpServiceFamiliesDibProxy::builder()
        .setServiceInfos({ { QKnxNetIp::ServiceFamily::Core, 10 } }).create();

    auto frame = QKnxNetIpSearchResponseProxy::builder()
        .setControlEndpoint(endpoint)
        .setDeviceHardware(hardware)
        .setSupportedFamilies(families)
        .create();
    QKnxNetIpSearchResponseProxy search(frame);

    QCOMPARE(search.isValid(), true);
    QCOMPARE(frame.size(), quint16(72));
    QCOMPARE(frame.bytes(), QKnxByteArray::fromHex("06100202004808017f0000010e57")
        + QKnxByteArray::fromHex("36012001ffff111112345612345600000000bcaec56690f9")
        + QKnxByteArray::fromHex("71742e696f204b4e5820646576696365") // qt.io KNX device
        + QKnxByteArray::fromHex("0000000000000000000000000000")
        + QKnxByteArray::fromHex("0402020a"));
    QCOMPARE(frame.data().size(), quint16(66));
    QCOMPARE(frame.data(), QKnxByteArray::fromHex("08017f0000010e57")
        + QKnxByteArray::fromHex("36012001ffff111112345612345600000000bcaec56690f9")
        + QKnxByteArray::fromHex("71742e696f204b4e5820646576696365") // qt.io KNX device
        + QKnxByteArray::fromHex("0000000000000000000000000000")
        + QKnxByteArray::fromHex("0402020a"));

    QCOMPARE(search.controlEndpoint().isValid(), true);
    QCOMPARE(search.controlEndpoint().bytes(), QKnxByteArray::fromHex("08017f0000010e57"));
    QCOMPARE(search.deviceHardware().isValid(), true);
    QCOMPARE(search.deviceHardware().bytes(),
        QKnxByteArray::fromHex("36012001FFFF111112345612345600000000BCAEC56690F9")
        + QKnxByteArray::fromHex("71742e696f204b4e5820646576696365") // qt.io KNX device
        + QKnxByteArray::fromHex("0000000000000000000000000000"));
    QCOMPARE(search.supportedFamilies().isValid(), true);
    QCOMPARE(search.supportedFamilies().bytes(), QKnxByteArray::fromHex("0402020a"));

    QCOMPARE(QKnxNetIpFrame::fromBytes(frame.bytes(), 0).isValid(), true);
    QCOMPARE(QKnxNetIpFrame::fromBytes(frame.bytes(), 56).isValid(), false);
    QCOMPARE(QKnxNetIpFrame::fromBytes(frame.bytes(), 2).isValid(), false);
}

void tst_QKnxNetIpSearchResponse::testSearchResponseEndpoint()
{
    auto hardware = QKnxNetIpDeviceDibProxy::builder()
        .setMediumType(QKnx::MediumType::NetIP)
        .setDeviceStatus(QKnxNetIp::ProgrammingMode::Active)
        .setIndividualAddress(QKnxAddress::Individual::Unregistered)
        .setProjectInstallationId(0x1111)
        .setSerialNumber(QKnxByteArray::fromHex("123456123456"))
        .setMulticastAddress(QHostAddress::AnyIPv4)
        .setMacAddress(QKnxByteArray::fromHex("bcaec56690f9"))
        .setDeviceName(QByteArray("qt.io KNX device"))
        .create();
    auto families = QKnxNetIpServiceFamiliesDibProxy::builder()
        .setServiceInfos({ { QKnxNetIp::ServiceFamily::Core, 10 } }).create();

    auto frame = QKnxNetIpSearchResponseProxy::builder()
        .setControlEndpoint(QKnxNetIpHpaiProxy::builder()
            .setHostProtocol(QKnxNetIp::HostProtocol::TCP_IPv4)
            .setHostAddress(QHostAddress::LocalHost)
            .setPort(3671).create())
        .setDeviceHardware(hardware)
        .setSupportedFamilies(families)
        .create();

    QKnxNetIpSearchResponseProxy search(frame);
    QCOMPARE(search.isValid(), false);
}

void tst_QKnxNetIpSearchResponse::testDebugStream()
{
    struct DebugHandler
    {
        explicit DebugHandler(QtMessageHandler newMessageHandler)
            : oldMessageHandler(qInstallMessageHandler(newMessageHandler)) {}
        ~DebugHandler() {
            qInstallMessageHandler(oldMessageHandler);
        }
        QtMessageHandler oldMessageHandler;
    } _(myMessageHandler);

    qDebug() << QKnxNetIpSearchResponseProxy::builder().create();
    QCOMPARE(s_msg, QString::fromLatin1("0x061002020006"));

    qDebug() << QKnxNetIpSearchResponseProxy::builder()
        .setControlEndpoint(QKnxNetIpHpaiProxy::builder()
            .setHostAddress(QHostAddress::LocalHost)
            .setPort(3671).create())
        .setSupportedFamilies(QKnxNetIpServiceFamiliesDibProxy::builder()
            .setServiceInfos({ { QKnxNetIp::ServiceFamily::Core, 10 } }).create())
        .setDeviceHardware(QKnxNetIpDeviceDibProxy::builder()
            .setMediumType(QKnx::MediumType::NetIP)
            .setDeviceStatus(QKnxNetIp::ProgrammingMode::Active)
            .setIndividualAddress(QKnxAddress::Individual::Unregistered)
            .setProjectInstallationId(0x1111)
            .setSerialNumber(QKnxByteArray::fromHex("123456123456"))
            .setMulticastAddress(QHostAddress::AnyIPv4)
            .setMacAddress(QKnxByteArray::fromHex("bcaec56690f9"))
            .setDeviceName(QByteArray("qt.io KNX device"))
            .create()
        ).create();
    QCOMPARE(s_msg, QString::fromLatin1("0x06100202004808017f0000010e5736012001ffff1111123456123456"
        "00000000bcaec56690f971742e696f204b4e582064657669636500000000000000000000000000000"
        "402020a"));
}

void tst_QKnxNetIpSearchResponse::testSupportedFamiliesVersions()
{
    auto endpoint = QKnxNetIpHpaiProxy::builder()
        .setHostAddress(QHostAddress::LocalHost)
        .setPort(3671)
        .create();
    auto hardware = QKnxNetIpDeviceDibProxy::builder()
        .setMediumType(QKnx::MediumType::NetIP)
        .setDeviceStatus(QKnxNetIp::ProgrammingMode::Active)
        .setIndividualAddress(QKnxAddress::Individual::Unregistered)
        .setProjectInstallationId(0x1111)
        .setSerialNumber(QKnxByteArray::fromHex("123456123456"))
        .setMulticastAddress(QHostAddress::AnyIPv4)
        .setMacAddress(QKnxByteArray::fromHex("bcaec56690f9"))
        .setDeviceName(QByteArray("qt.io KNX device"))
        .create();
    QList<QKnxServiceInfo> fam = {
        { QKnxNetIp::ServiceFamily::IpTunneling, 0x04 },
        { QKnxNetIp::ServiceFamily::Core, 9 },
        { QKnxNetIp::ServiceFamily::DeviceManagement, 2 },
        { QKnxNetIp::ServiceFamily::DeviceManagement, 1 },
        { QKnxNetIp::ServiceFamily::IpTunneling, 11 },
        { QKnxNetIp::ServiceFamily::IpRouting, 12 },
        { QKnxNetIp::ServiceFamily::RemoteLogging, 13 },
        { QKnxNetIp::ServiceFamily::Security, 1 }
    };

    {   // test that a supported service families dib with security service
        // family is not valid in a search response
        auto families = QKnxNetIpServiceFamiliesDibProxy::builder()
            .setServiceInfos(fam).create();
        auto frame = QKnxNetIpSearchResponseProxy::builder()
            .setControlEndpoint(endpoint)
            .setDeviceHardware(hardware)
            .setSupportedFamilies(families)
            .create();
        QKnxNetIpSearchResponseProxy searchResponse(frame);

        auto extractedFamilies = searchResponse.supportedFamilies();
        QVERIFY(!extractedFamilies.isValid());
        QCOMPARE(extractedFamilies.size(), 0);
    }

    {   // test that a supported service families dib without service family is
        // valid in a search response
        QCOMPARE(8, fam.size());
        fam.erase(std::remove_if(fam.begin(), fam.end(), [](const QKnxServiceInfo &info) {
            return info.ServiceFamily == QKnxNetIp::ServiceFamily::Security;
        }), fam.end());
        auto families = QKnxNetIpServiceFamiliesDibProxy::builder()
            .setServiceInfos(fam).create();
        auto frame = QKnxNetIpSearchResponseProxy::builder()
            .setControlEndpoint(endpoint)
            .setDeviceHardware(hardware)
            .setSupportedFamilies(families)
            .create();
        QKnxNetIpSearchResponseProxy searchResponse(frame);
        auto dibFamilies = searchResponse.supportedFamilies();
        QVERIFY(dibFamilies.isValid());

        const QKnxNetIpServiceFamiliesDibProxy view(dibFamilies);
        QVERIFY(view.isValid());

        auto extractedFamilies = view.serviceInfos();
        QCOMPARE(extractedFamilies.size(), fam.size());
    }
}

void tst_QKnxNetIpSearchResponse::testExtendedResponse()
{
    auto endpoint = QKnxNetIpHpaiProxy::builder()
        .setHostAddress(QHostAddress::LocalHost)
        .setPort(3671)
        .create();
    auto hardware = QKnxNetIpDeviceDibProxy::builder()
        .setMediumType(QKnx::MediumType::NetIP)
        .setDeviceStatus(QKnxNetIp::ProgrammingMode::Active)
        .setIndividualAddress(QKnxAddress::Individual::Unregistered)
        .setProjectInstallationId(0x1111)
        .setSerialNumber(QKnxByteArray::fromHex("123456123456"))
        .setMulticastAddress(QHostAddress::AnyIPv4)
        .setMacAddress(QKnxByteArray::fromHex("bcaec56690f9"))
        .setDeviceName(QByteArray("qt.io KNX device"))
        .create();
    auto families = QKnxNetIpServiceFamiliesDibProxy::builder()
        .setServiceInfos({ { QKnxNetIp::ServiceFamily::Core, 10 } }).create();
    auto configDib = QKnxNetIpConfigDibProxy::builder()
        .setIpAddress(QHostAddress("192.168.2.12"))
        .setSubnetMask(QHostAddress("255.255.255.0"))
        .setDefaultGateway(QHostAddress("192.168.2.1"))
        .setCapabilities(QKnxNetIp::Capability::AutoIp
            | QKnxNetIp::Capability::Dhcp)
        .setAssignmentMethods(QKnxNetIp::AssignmentMethod::Manual
            | QKnxNetIp::AssignmentMethod::AutoIp
            | QKnxNetIp::AssignmentMethod::Dhcp)
        .create();
    QCOMPARE(configDib.isValid(), true);
    QCOMPARE(configDib.size(), quint16(16));
    QCOMPARE(configDib.bytes(),
        QKnxByteArray::fromHex("1003c0a8020cffffff00c0a80201060d"));

    QSet<QKnxNetIpDib> dibs = { configDib, configDib };
    QCOMPARE(dibs.size(), 1);

    auto frame = QKnxNetIpSearchResponseProxy::extendedBuilder()
        .setControlEndpoint(endpoint)
        .setDeviceHardware(hardware)
        .setSupportedFamilies(families)
        .setOptionalDibs(dibs)
        .create();

    QKnxNetIpSearchResponseProxy response(frame);
    QCOMPARE(response.isValid(), true);
    QCOMPARE(response.optionalDibs().size(), dibs.size());
    QCOMPARE(frame.size(), quint16(72 + 16));
    QCOMPARE(frame.data().size(), quint16(66 + 16));
    QCOMPARE(frame.data(), endpoint.bytes() + hardware.bytes() + families.bytes()
        + configDib.bytes());

    QCOMPARE(response.controlEndpoint().isValid(), true);
    QCOMPARE(response.controlEndpoint().bytes(), endpoint.bytes());

    auto hardwareDib = response.deviceHardware();
    QCOMPARE(hardwareDib.isValid(), true);
    QCOMPARE(hardwareDib.bytes(), hardware.bytes());

    auto familiesDib = response.supportedFamilies();
    QCOMPARE(familiesDib.isValid(), true);
    QCOMPARE(familiesDib.bytes(), families.bytes());

    auto responseDibs = response.optionalDibs();
    auto responseConfigDib = responseDibs.at(0);
    QCOMPARE(responseConfigDib.isValid(), true);
    QCOMPARE(responseConfigDib.size(), quint16(16));
    QCOMPARE(responseConfigDib.bytes(), configDib.bytes());
}

void tst_QKnxNetIpSearchResponse::testExtendedFramesSupportedFamilies()
{
    auto endpoint = QKnxNetIpHpaiProxy::builder()
        .setHostAddress(QHostAddress::LocalHost)
        .setPort(3671)
        .create();
    auto hardware = QKnxNetIpDeviceDibProxy::builder()
        .setMediumType(QKnx::MediumType::NetIP)
        .setDeviceStatus(QKnxNetIp::ProgrammingMode::Active)
        .setIndividualAddress(QKnxAddress::Individual::Unregistered)
        .setProjectInstallationId(0x1111)
        .setSerialNumber(QKnxByteArray::fromHex("123456123456"))
        .setMulticastAddress(QHostAddress::AnyIPv4)
        .setMacAddress(QKnxByteArray::fromHex("bcaec56690f9"))
        .setDeviceName(QByteArray("qt.io KNX device"))
        .create();
    auto families = QKnxNetIpServiceFamiliesDibProxy::builder()
        .setServiceInfos({ { QKnxNetIp::ServiceFamily::Core, 10 } }).create();
    auto configDib = QKnxNetIpConfigDibProxy::builder()
        .setIpAddress(QHostAddress("192.168.2.12"))
        .setSubnetMask(QHostAddress("255.255.255.0"))
        .setDefaultGateway(QHostAddress("192.168.2.1"))
        .setCapabilities(QKnxNetIp::Capability::AutoIp
            | QKnxNetIp::Capability::Dhcp)
        .setAssignmentMethods(QKnxNetIp::AssignmentMethod::Manual
            | QKnxNetIp::AssignmentMethod::AutoIp
            | QKnxNetIp::AssignmentMethod::Dhcp)
        .create();

    {   // test a frame with a supported families field
        QSet<QKnxNetIpDib> dibs = { configDib };
        auto frame = QKnxNetIpSearchResponseProxy::extendedBuilder()
            .setControlEndpoint(endpoint)
            .setDeviceHardware(hardware)
            .setSupportedFamilies(families)
            .setOptionalDibs(dibs)
            .create();

        QKnxNetIpSearchResponseProxy response(frame);
        QVERIFY(response.isExtended());
        QVERIFY(response.isValid());

        auto supportedFamilies = response.supportedFamilies();
        QVERIFY(supportedFamilies.isValid());

        QKnxNetIpServiceFamiliesDibProxy viewFam(supportedFamilies);
        QCOMPARE(viewFam.serviceInfos().at(0).ServiceFamily,
            QKnxNetIp::ServiceFamily::Core);
    }

    {   // test a frame without a supported families field
        QSet<QKnxNetIpDib> dibs = { hardware, configDib };
        auto frame = QKnxNetIpSearchResponseProxy::extendedBuilder()
            .setControlEndpoint(endpoint)
            .setOptionalDibs(dibs)
            .setDeviceHardware(hardware)
            .create();

        QKnxNetIpSearchResponseProxy invalidResponse(frame);
        QVERIFY(invalidResponse.isExtended());
        QVERIFY(!invalidResponse.isValid());

        auto supportedFamilies = invalidResponse.supportedFamilies();
        QVERIFY(!supportedFamilies.isValid());

        QKnxNetIpServiceFamiliesDibProxy viewFam(supportedFamilies);
        QVERIFY(viewFam.serviceInfos().isEmpty());
    }
}

void tst_QKnxNetIpSearchResponse::testExtendedFramesDeviceHardware()
{
    auto endpoint = QKnxNetIpHpaiProxy::builder()
        .setHostAddress(QHostAddress::LocalHost)
        .setPort(3671)
        .create();
    auto hardware = QKnxNetIpDeviceDibProxy::builder()
        .setMediumType(QKnx::MediumType::NetIP)
        .setDeviceStatus(QKnxNetIp::ProgrammingMode::Active)
        .setIndividualAddress(QKnxAddress::Individual::Unregistered)
        .setProjectInstallationId(0x1111)
        .setSerialNumber(QKnxByteArray::fromHex("123456123456"))
        .setMulticastAddress(QHostAddress::AnyIPv4)
        .setMacAddress(QKnxByteArray::fromHex("bcaec56690f9"))
        .setDeviceName(QByteArray("qt.io KNX device"))
        .create();
    auto families = QKnxNetIpServiceFamiliesDibProxy::builder()
        .setServiceInfos({ { QKnxNetIp::ServiceFamily::Core, 10 } }).create();
    auto configDib = QKnxNetIpConfigDibProxy::builder()
        .setIpAddress(QHostAddress("192.168.2.12"))
        .setSubnetMask(QHostAddress("255.255.255.0"))
        .setDefaultGateway(QHostAddress("192.168.2.1"))
        .setCapabilities(QKnxNetIp::Capability::AutoIp
            | QKnxNetIp::Capability::Dhcp)
        .setAssignmentMethods(QKnxNetIp::AssignmentMethod::Manual
            | QKnxNetIp::AssignmentMethod::AutoIp
            | QKnxNetIp::AssignmentMethod::Dhcp)
        .create();

    QSet<QKnxNetIpDib> dibs = { configDib };
    auto frame = QKnxNetIpSearchResponseProxy::extendedBuilder()
        .setControlEndpoint(endpoint)
        .setOptionalDibs(dibs)
        .setDeviceHardware(hardware)
        .setSupportedFamilies(families)
        .create();
    QKnxNetIpSearchResponseProxy response(frame);
    QVERIFY(response.isExtended());

    auto hardwareDib = response.deviceHardware();
    QKnxNetIpDeviceDibProxy hardwareView(hardwareDib);
    QVERIFY(hardwareView.isValid());
    QVERIFY(hardwareView.macAddress() == QKnxByteArray::fromHex("bcaec56690f9"));
}

void tst_QKnxNetIpSearchResponse::testDibsShallBePresentOnce()
{
    auto endpoint = QKnxNetIpHpaiProxy::builder()
        .setHostAddress(QHostAddress::LocalHost)
        .setPort(3671)
        .create();
    auto hardware = QKnxNetIpDeviceDibProxy::builder()
        .setMediumType(QKnx::MediumType::NetIP)
        .setDeviceStatus(QKnxNetIp::ProgrammingMode::Active)
        .setIndividualAddress(QKnxAddress::Individual::Unregistered)
        .setProjectInstallationId(0x1111)
        .setSerialNumber(QKnxByteArray::fromHex("123456123456"))
        .setMulticastAddress(QHostAddress::AnyIPv4)
        .setMacAddress(QKnxByteArray::fromHex("bcaec56690f9"))
        .setDeviceName(QByteArray("qt.io KNX device"))
        .create();
    auto families = QKnxNetIpServiceFamiliesDibProxy::builder()
        .setServiceInfos({ { QKnxNetIp::ServiceFamily::Core, 10 } }).create();
    auto configDib = QKnxNetIpConfigDibProxy::builder()
        .setIpAddress(QHostAddress("192.168.2.12"))
        .setSubnetMask(QHostAddress("255.255.255.0"))
        .setDefaultGateway(QHostAddress("192.168.2.1"))
        .setCapabilities(QKnxNetIp::Capability::AutoIp
            | QKnxNetIp::Capability::Dhcp)
        .setAssignmentMethods(QKnxNetIp::AssignmentMethod::Manual
            | QKnxNetIp::AssignmentMethod::AutoIp
            | QKnxNetIp::AssignmentMethod::Dhcp)
        .create();

    QSet<QKnxNetIpDib> dibs = { hardware, families, configDib, hardware,
        configDib, families, families };
    QCOMPARE(dibs.size(), 3);
    auto frame = QKnxNetIpSearchResponseProxy::extendedBuilder()
        .setControlEndpoint(endpoint)
        .setOptionalDibs(dibs)
        .setDeviceHardware(hardware)
        .setSupportedFamilies(families)
        .create();

    QKnxNetIpSearchResponseProxy response(frame);
    QVERIFY(response.isExtended());
    QVERIFY(response.isValid());
    QCOMPARE(response.variableDibs().size(), dibs.size());
}

QTEST_APPLESS_MAIN(tst_QKnxNetIpSearchResponse)

#include "tst_qknxnetipsearchresponse.moc"
