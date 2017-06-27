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

#include <QtCore/qdebug.h>
#include <QtTest/qtest.h>
#include <QtKnx/qknxnetipdeviceconfigurationacknowledge.h>

static QString s_msg;
static void myMessageHandler(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    s_msg = msg;
}

class tst_QKnxNetIpDeviceConfigurationAcknowledge: public QObject
{
    Q_OBJECT

private slots:
    void testDefaultConstructor();
    void testConstructor(); // to do
    void testDebugStream(); // to do
    void testDataStream(); // to do
};

void tst_QKnxNetIpDeviceConfigurationAcknowledge::testDefaultConstructor()
{
    QKnxNetIpDeviceConfigurationAcknowledge configuration;
    QCOMPARE(configuration.isValid(), false);
}

void tst_QKnxNetIpDeviceConfigurationAcknowledge::testConstructor()
{
}

void tst_QKnxNetIpDeviceConfigurationAcknowledge::testDebugStream()
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

    //qDebug() << DEFAULTCONSTRUCTOR;
    //QCOMPARE(s_msg, QString::fromLatin1("0x1nv4l1d"));
}

void tst_QKnxNetIpDeviceConfigurationAcknowledge::testDataStream()
{
    {
        //QByteArray byteArray;
        //QDataStream out(&byteArray, QIODevice::WriteOnly);
        //out << CONSTRUCTORWITHARG;
        //QCOMPARE(byteArray, QByteArray::fromHex("1404C0A8020CFFFFFF00C0A80201B48A03020100"));
    }

    {
        //out << OTHERCONSTRUCTOR;
        //QCOMPARE(byteArray, QByteArray::fromHex("1404C0A8020CFFFFFF00C0A80201B48A03020100"));
    }
}

QTEST_APPLESS_MAIN(tst_QKnxNetIpDeviceConfigurationAcknowledge)

#include "tst_qknxnetipdeviceconfigurationacknowledge.moc"