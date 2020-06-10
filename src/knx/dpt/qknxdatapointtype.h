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

#ifndef QKNXDATAPOINTTYPE_H
#define QKNXDATAPOINTTYPE_H

#include <QtCore/qcoreapplication.h>
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>
#include <QtCore/qshareddata.h>
#include <QtKnx/qknxbytearray.h>
#include <QtKnx/qtknxglobal.h>

QT_BEGIN_NAMESPACE

struct QKnxDatapointTypePrivate;

class Q_KNX_EXPORT QKnxDatapointType
{
    Q_GADGET
    Q_DECLARE_TR_FUNCTIONS(QKnxDatapointType)

public:
    enum class Type : quint32
    {
        Unknown = 0,
        Dpt1_1Bit = 100000,
            DptSwitch, DptBool, DptEnable, DptRamp, DptAlarm, DptBinaryValue, DptStep, DptUpDown,
            DptOpenClose, DptStart, DptState, DptInvert, DptDimSendStyle, DptInputSource, DptReset,
            DptAck, DptTrigger, DptOccupancy, DptWindowDoor, DptLogicalFunction = 100021, DptSceneAB,
            DptShutterBlindsMode, DptHeatCool = 100100,
        Dpt2_1BitControlled = 200000,
            DptSwitchControl, DptBoolControl, DptEnableControl, DptRampControl, DptAlarmControl,
            DptBinaryValueControl, DptStepControl, DptDirection1Control, DptDirection2Control,
            DptStartControl, DptStateControl, DptInvertControl,
        Dpt3_3BitControlled = 300000,
            DptControlDimming = 300007, DptControlBlinds = 300008,
        Dpt4_Character = 400000,
            DptCharAscii, DptChar88591,
        Dpt5_8bitUnsigned = 500000,
            DptScaling, DptAngle = 500003, DptPercentU8, DptDecimalFactor, DptTariff,
            DptValue1Ucount = 500010,
        Dpt6_8bitSigned = 600000,
            DptPercentV8, DptValue1Count = 600010, DptStatusMode3 = 600020,
        Dpt7_2ByteUnsigned = 700000, DptValue2UCount, DptTimePeriodMsec, DptTimePeriod10Msec,
            DptTimePeriod100Msec, DptTimePeriodSec, DptTimePeriodMin, DptTimePeriodHrs,
            DptPropertyDataType = 700010, DptLengthMilliMeter, DptUEICurrentMilliA, DptBrightness,
        Dpt8_2ByteSigned = 800000,
            DptValue2Count, DptDeltaTimeMsec, DptDeltaTime10Msec, DptDeltaTime100Msec,
            DptDeltaTimeSec, DptDeltaTimeMin, DptDeltaTimeHrs, DptPercentV16 = 800010,
            DptRotationAngle,
        Dpt9_2ByteFloat = 900000,
        DptTemperatureCelsius, DptTemperatureKelvin, DptTemperatureChange, DptValueLux,
            DptWindSpeed, DptPressure, DptHumidity, DptAirQuality, DptAirFlow, DptTimeSecond,
            DptTimeMilliSecond, DptVoltage = 900020, DptCurrent, DptPowerDensity,
            DptKelvinPerPercent, DptPower, DptVolumeFlow, DptAmountRain, DptTemperatureFahrenheit,
            DptWindSpeedKmPerHour, DptValueAbsoluteHumidity, DptConcentration,
        Dpt10_TimeOfDay = 1000000,
            DptTimeOfDay = 1000001,
        Dpt11_Date = 1100000,
            DptDate = 1100001,
        Dpt12_4ByteUnsigned = 1200000,
            DptValue4Ucount,
        Dpt13_4ByteSigned = 1300000,
            DptValue4Count, DptFlowRateCubicMeterPerHour, DptActiveEnergy = 1300010,
            DptApparentEnergy, DptReactiveEnergy, DptActiveEnergykWh, Dpt_ApparentEnergykVAh,
            DptReactiveEnergykVARh, DptLongDeltaTimeSec = 1300100,
        Dpt14_4ByteFloat = 1400000,
            DptValueAcceleration = 1400000, DptValueAccelerationAngular, DptValueActivationEnergy,
            DptValueActivity, DptValueMol,
            DptValueAmplitude, DptValueAngleRad, DptValueAngleDeg, DptValueAngularMomentum,
            DptValueAngularVelocity, DptValueArea, DptValueCapacitance,
            DptValueChargeDensitySurface, DptValueChargeDensityVolume, DptValueCompressibility,
            DptValueConductance, DptValueElectricalConductivity, DptValueDensity,
            DptValueElectricCharge, DptValueElectricCurrent, DptValueElectricCurrentDensity,
            DptValueElectricDipoleMoment, DptValueElectricDisplacement,
            DptValueElectricFieldStrength, DptValueElectricFlux, DptValueElectricFluxDensity,
            DptValueElectricPolarization, DptValueElectricPotential,
            DptValueElectricPotentialDifference, DptValueElectromagneticMoment,
            DptValueElectromotiveForce, DptValueEnergy, DptValueForce, DptValueFrequency,
            DptValueAngularFrequency, DptValueHeatCapacity, DptValueHeatFlowRate,
            DptValueHeatQuantity, DptValueImpedance, DptValueLength, DptValueLightQuantity,
            DptValueLuminance, DptValueLuminousFlux, DptValueLuminousIntensity,
            DptValueMagneticFieldStrength, DptValueMagneticFlux, DptValueMagneticFluxDensity,
            DptValueMagneticMoment, DptValueMagneticPolarization, DptValueMagnetization,
            DptValueMagnetomotiveForce, DptValueMass, DptValueMassFlux, DptValueMomentum,
            DptValuePhaseAngleRad, DptValuePhaseAngleDeg, DptValuePower, DptValuePowerFactor,
            DptValuePressure, DptValueReactance, DptValueResistance, DptValueResistivity,
            DptValueSelfInductance, DptValueSolidAngle, DptValueSoundIntensity, DptValueSpeed,
            DptValueStress, DptValueSurfaceTension, DptValueCommonTemperature,
            DptValueAbsoluteTemperature, DptValueTemperatureDifference, DptValueThermalCapacity,
            DptValueThermalConductivity, DptValueThermoelectricPower, DptValueTime, DptValueTorque,
            DptValueVolume, DptValueVolumeFlux, DptValueWeight, DptValueWork,
        Dpt15_EntranceAccess = 1500000,
        Dpt16_CharacterString = 1600000,
            DptStringASCII = 1600000, DptString88591,
        Dpt17_SceneNumber = 1700000,
            DptSceneNumber,
        Dpt18_SceneControl = 1800000,
            DptSceneControl,
        Dpt19_DataTime = 1900000,
            DptDateTime = 1900001,
        Dpt20_1Byte = 2000000,
            DptScloMode, DptBuildingMode, DptOccMode, DptPriority, DptLightApplicationMode,
            DptApplicationArea, DptAlarmClassType, DptPsuMode, DptErrorClassSystem = 2000011,
            DptErrorClassHvac, DptTimeDelay, DptBeaufortWindForceScale, DptSensorSelect = 2000017,
            DptActuatorConnectType = 2000020, DptCloudCover, DptCommandMode = 2001000,
            DptAdditionalInfoTypes = 2001001,
        Dpt21_8BitSet = 2100000,
            DptGeneralStatus, DptDeviceControl,
        Dpt22_16BitSet = 2200000,
            DptMedia = 2201000,
        Dpt23_2BitSet = 2300000,
            DptOnOffAction, DptAlarmReaction, DptUpDownAction,
        Dpt24_VariableString = 2400000,
            DptVariableString88591 = 2400001,
        Dpt25_2NibbleSet = 2500000,
        Dpt26_8BitSet = 2600000,
            DptSceneInfo,
        Dpt27_32BitSet = 2700000,
            DptCombinedInfoOnOff,
        Dpt28_StringUtf8 = 2800000,
            DptUtf8 = 2800001,
        Dpt29_ElectricalEnergy = 2900000,
            DptActiveEnergyV64 = 2900010, DptApparentEnergyV64, DptReactiveEnergyV64,
        Dpt30_24TimesChannelActivation = 3000000,
        DPT217_DatapointTypeVersion = 21700000,
            DptVersion = 21700001,
        Dpt221 = 22100000,
            DptSerialNumber = 22100001,
        Dpt225_ScalingSpeed = 22500000,
        Dpt232_3ByteColourRGB = 23200000,
            DptColourRGB = 23200600
    };
    Q_ENUM(Type)
    Type type() const;

    virtual ~QKnxDatapointType();

    QKnxDatapointType(Type type, int size);
    QKnxDatapointType(const QString &dptId, int size);
    QKnxDatapointType(quint16 mainType, quint16 subType, int size);

    int subType() const;
    int mainType() const;

    int size() const;

    QVariant minimum() const;
    void setMinimum(const QVariant &minimum);

    QVariant maximum() const;
    void setMaximum(const QVariant &maximum);

    double coefficient() const;
    void setCoefficient(double coef);

    QString minimumText() const;
    void setMinimumText(const QString &minimumText);

    QString maximumText() const;
    void setMaximumText(const QString &maximumText);

    void setRange(const QVariant &minimum, const QVariant &maximum);
    void setRangeText(const QString &minimumText, const QString &maximumText);

    QString unit() const;
    void setUnit(const QString &unit);

    QString description() const;
    void setDescription(const QString &description);

    virtual bool isValid() const;
    const quint8 *constData() const;

    quint8 byte(quint16 index) const;
    bool setByte(quint16 index, quint8 bytes);

    QKnxByteArray bytes() const;
    virtual bool setBytes(const QKnxByteArray &bytesToSet, quint16 index, quint16 count);

    QKnxDatapointType(const QKnxDatapointType &other);
    QKnxDatapointType &operator=(const QKnxDatapointType &other);

    QKnxDatapointType(QKnxDatapointType &&other) Q_DECL_NOTHROW;
    QKnxDatapointType &operator=(QKnxDatapointType &&other) Q_DECL_NOTHROW;

    void swap(QKnxDatapointType &other) Q_DECL_NOTHROW;

    bool operator==(const QKnxDatapointType &other) const;
    bool operator!=(const QKnxDatapointType &other) const;

    template<typename T> static constexpr bool testBit(T byteToTest, quint8 bit) noexcept
    {
        return (byteToTest & (T(1) << bit)) != 0;
    }

    template<typename T> static constexpr T setBit(T byteToSet, bool value, quint8 bit) noexcept
    {
        return (value ? byteToSet | (T(1) << bit) : byteToSet & ~(T(1) << bit));
    }

    static Type toType(const QString &dpt);
    static const constexpr int SubType = 0x00;

protected:
    void resize(int newSize);

private:
    QKnxDatapointType() = delete;
    explicit QKnxDatapointType(QKnxDatapointTypePrivate &dd);

private:
    QSharedDataPointer<QKnxDatapointTypePrivate> d_ptr;
};

#ifndef Q_CLANG_QDOC
    using QKnxFixedSizeDatapointType = QKnxDatapointType;
#else
    class QKnxFixedSizeDatapointType : public QKnxDatapointType {public:};
#endif

class Q_KNX_EXPORT QKnxVariableSizeDatapointType : public QKnxDatapointType
{
public:
    QKnxVariableSizeDatapointType(Type type, int size);
    QKnxVariableSizeDatapointType(const QString &dptId, int size);
    QKnxVariableSizeDatapointType(quint16 mainType, quint16 subType, int size);

    bool setBytes(const QKnxByteArray &bytesToSet, quint16 index, quint16 count) override;
};
Q_KNX_EXPORT QDebug operator<<(QDebug debug, const QKnxDatapointType &dpt);

inline size_t qHash(const QKnxDatapointType::Type &key, uint seed)
{
    return qHash(quint32(key), seed);
}

QT_END_NAMESPACE

#endif
