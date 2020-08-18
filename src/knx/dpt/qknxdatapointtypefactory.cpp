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

#include "qknxdatapointtype_p.h"
#include "qknxdatapointtypefactory.h"

#include "qknx1bit.h"
#include "qknx1bitcontrolled.h"
#include "qknx1byte.h"
#include "qknx2bitset.h"
#include "qknx2bytefloat.h"
#include "qknx2bytesignedvalue.h"
#include "qknx2byteunsignedvalue.h"
#include "qknx32bitset.h"
#include "qknx3bitcontrolled.h"
#include "qknx4bytefloat.h"
#include "qknx4bytesignedvalue.h"
#include "qknx4byteunsignedvalue.h"
#include "qknx8bitset.h"
#include "qknx8bitsignedvalue.h"
#include "qknx8bitunsignedvalue.h"
#include "qknxchar.h"
#include "qknxcharstring.h"
#include "qknxdatetime.h"
#include "qknxelectricalenergy.h"
#include "qknxentranceaccess.h"
#include "qknxscene.h"
#include "qknxstatusmode3.h"
#include "qknxutf8string.h"
#include "qknxvarstring.h"

QT_BEGIN_NAMESPACE

/*!
    \class QKnxDatapointTypeFactory

    \inmodule QtKnx
    \ingroup qtknx-datapoint-types

    \brief The QKnxDatapointTypeFactory class is used to create
    \l QKnxDatapointType objects based on their main number and sub number.

    You can register one or more datapoints with this factory and query them
    based on the main number and sub number.

    This class follows the singleton design pattern. Only one instance of this
    class can be created and its reference can be fetched from the instance()
    method.

    The KNX datapoint types are identified by a 16-bit main number and a 16-bit
    sub number.

    \sa {Qt KNX Datapoint Type Classes}
*/

/*!
    \fn QKnxDatapointTypeFactory::instance()

    Returns a reference to the datapoint type factory.
*/

/*!
    \typedef QKnxDatapointTypeFactory::FactoryFunction
    \internal
*/

/*!
    \fn void QKnxDatapointTypeFactory::registerType(int mainType, int subType, int size)

    Registers a datapoint type with the main type \a mainType, sub type
    \a subType, and size \a size.
*/

/*!
    Returns a new instance of a \l QKnxDatapointType subclass. The instantiation
    of the subclass depends on the \a mainType and \a subType given as arguments
    to this function.

    \note The ownership of the created object remains with the programmer.
*/
QKnxDatapointType *QKnxDatapointTypeFactory::createType(int mainType, int subType) const
{
    const auto main = factoryTable().constFind(mainType);
    if (main == factoryTable().constEnd())
        return nullptr;

    auto sub = (*main).constFind(subType);
    if (sub == (*main).constEnd())
        sub = (*main).constFind(0); // try base, e.g. 1.00[0]

    if (sub == (*main).constEnd())
        return nullptr;

    return (*sub)();
}

/*!
    Returns a new instance of a \l QKnxDatapointType subclass. The instantiation
    of the subclass depends on the \a type given as an argument to this
    function.

    \note The ownership of the created object remains with the programmer.
*/
QKnxDatapointType *QKnxDatapointTypeFactory::createType(QKnxDatapointType::Type type) const
{
    const auto num = QString::number(int(type));
    if (num.size() < 6)
        return nullptr;

    bool okMain = false, okSub = false;
    const auto number = QStringView{ num };

    // Datapoint Type shall be identified by a 16 bit main number separated by a dot from a 16 bit
    // sub number. The assumption being made is that QKnxDatapointType::Type is encoded in that way
    // while omitting the dot.
    int mainType = number.left(number.size() - 5).toInt(&okMain);
    int subType = number.right(5).toInt(&okSub);

    if (okMain && okSub)
        return createType(mainType, subType);
    return nullptr;
}

/*!
    Returns the size in bytes for the given \a mainType.
*/
int QKnxDatapointTypeFactory::typeSize(int mainType)
{
    return sizeTable().value(mainType);
}

/*!
    Returns a list of registered main datapoint types.
*/
QList<int> QKnxDatapointTypeFactory::mainTypes() const
{
    return factoryTable().keys();
}

/*!
    Queries the factory for a the given \a mainType and if it is registered,
    returns \c true; \c false otherwise.
*/
bool QKnxDatapointTypeFactory::containsMainType(int mainType) const
{
    return factoryTable().contains(mainType);
}

/*!
    Returns a list of registered sub datapoint types for the given \a mainType.
*/
QList<int> QKnxDatapointTypeFactory::subTypes(int mainType) const
{
    const auto main = factoryTable().constFind(mainType);
    if (main != factoryTable().constEnd())
        return (*main).keys();
    return {};
}

/*!
    Queries the factory for a the given \a mainType and \a subType and if the
    type is registered, returns \c true; \c false otherwise.
*/
bool QKnxDatapointTypeFactory::containsSubType(int mainType, int subType) const
{
    const auto main = factoryTable().constFind(mainType);
    if (main != factoryTable().constEnd())
        return (*main).constFind(subType) != (*main).constEnd();
    return false;
}

/*!
    \internal
*/
QKnxDatapointTypeFactory::QKnxDatapointTypeFactory()
{
    // DPT-1
    registerType<QKnx1Bit>();
    registerType<QKnxSwitch>();
    registerType<QKnxBool>();
    registerType<QKnxEnable>();
    registerType<QKnxRamp>();
    registerType<QKnxAlarm>();
    registerType<QKnxBinaryValue>();
    registerType<QKnxStep>();
    registerType<QKnxUpDown>();
    registerType<QKnxOpenClose>();
    registerType<QKnxStart>();
    registerType<QKnxState>();
    registerType<QKnxInvert>();
    registerType<QKnxDimSendStyle>();
    registerType<QKnxInputSource>();
    registerType<QKnxReset>();
    registerType<QKnxAck>();
    registerType<QKnxTrigger>();
    registerType<QKnxOccupancy>();
    registerType<QKnxWindowDoor>();
    registerType<QKnxLogicalFunction>();
    registerType<QKnxSceneAB>();
    registerType<QKnxShutterBlindsMode>();
    registerType<QKnxHeatCool>();

    // DPT-2
    registerType<QKnx1BitControlled>();
    registerType<QKnxSwitchControl>();
    registerType<QKnxBoolControl>();
    registerType<QKnxEnableControl>();
    registerType<QKnxRampControl>();
    registerType<QKnxAlarmControl>();
    registerType<QKnxBinaryValueControl>();
    registerType<QKnxStepControl>();
    registerType<QKnxDirection1Control>();
    registerType<QKnxDirection2Control>();
    registerType<QKnxStartControl>();
    registerType<QKnxStateControl>();
    registerType<QKnxInvertControl>();

    // DPT-3
    registerType<QKnx3BitControlled>();
    registerType<QKnxControlDimming>();
    registerType<QKnxControlBlinds>();

    // DPT-4
    registerType<QKnxChar>();
    registerType<QKnxCharASCII>();
    registerType<QKnxChar88591>();

    // DPT-5
    registerType<QKnx8BitUnsignedValue>();
    registerType<QKnxScaling>();
    registerType<QKnxAngle>();
    registerType<QKnxPercentU8>();
    registerType<QKnxDecimalFactor>();
    registerType<QKnxValue1Ucount>();
    registerType<QKnxTariff>();

    // DPT-6
    registerType<QKnx8BitSignedValue>();
    registerType<QKnxPercentV8>();
    registerType<QKnxValue1Count>();
    registerType<QKnxStatusMode3>();

    // DPT-7
    registerType<QKnx2ByteUnsignedValue>();
    registerType<QKnxValue2Ucount>();
    registerType<QKnxPropDataType>();
    registerType<QKnxTimePeriodMsec>();
    registerType<QKnxTimePeriod10Msec>();
    registerType<QKnxTimePeriod100Msec>();
    registerType<QKnxTimePeriodSec>();
    registerType<QKnxTimePeriodMin>();
    registerType<QKnxTimePeriodHrs>();
    registerType<QKnxLengthMilliMeter>();
    registerType<QKnxUEICurrentMilliA>();
    registerType<QKnxBrightness>();

    // DPT-8
    registerType<QKnx2ByteSignedValue>();
    registerType<QKnxValue2Count>();
    registerType<QKnxPercentV16>();
    registerType<QKnxDeltaTimeMsec>();
    registerType<QKnxDeltaTime10Msec>();
    registerType<QKnxDeltaTime100Msec>();
    registerType<QKnxDeltaTimeSec>();
    registerType<QKnxDeltaTimeMin>();
    registerType<QKnxDeltaTimeHrs>();
    registerType<QKnxRotationAngle>();

    // DPT-9
    registerType<QKnx2ByteFloat>();
    registerType<QKnxTemperatureCelsius>();
    registerType<QKnxTemperatureKelvin>();
    registerType<QKnxTemperatureChange>();
    registerType<QKnxValueLux>();
    registerType<QKnxWindSpeed>();
    registerType<QKnxPressure>();
    registerType<QKnxHumidity>();
    registerType<QKnxAirQuality>();
    registerType<QKnxAirFlow>();
    registerType<QKnxTimeSecond>();
    registerType<QKnxTimeMilliSecond>();
    registerType<QKnxVoltage>();
    registerType<QKnxCurrent>();
    registerType<QKnxPowerDensity>();
    registerType<QKnxKelvinPerPercent>();
    registerType<QKnxPower>();
    registerType<QKnxVolumeFlow>();
    registerType<QKnxAmountRain>();
    registerType<QKnxTemperatureFahrenheit>();
    registerType<QKnxWindSpeedKmPerHour>();
    registerType<QKnxValueAbsoluteHumidity>();
    registerType<QKnxConcentration>();

    // DPT-10
    registerType<QKnxTimeOfDay>();
    registerType<QKnxDate>();

    // DPT-12
    registerType<QKnx4ByteUnsignedValue>();
    registerType<QKnxValue4UCount>();

    // DPT-13
    registerType<QKnx4ByteSignedValue>();
    registerType<QKnxValue4Count>();
    registerType<QKnxFlowRateCubicMeterPerHour>();
    registerType<QKnxActiveEnergy>();
    registerType<QKnxApparentEnergy>();
    registerType<QKnxReactiveEnergy>();
    registerType<QKnxActiveEnergykWh>();
    registerType<QKnxApparentEnergykVAh>();
    registerType<QKnxReactiveEnergykVARh>();
    registerType<QKnxLongDeltaTimeSec>();

    // DPT-14
    registerType<QKnx4ByteFloat>();
    registerType<QKnxValueAcceleration>(); // TODO: this replaces the QKnx4ByteFloat (subtype == 0)
    registerType<QKnxValueAccelerationAngular>();
    registerType<QKnxValueActivationEnergy>();
    registerType<QKnxValueActivity>();
    registerType<QKnxValueMol>();
    registerType<QKnxValueAmplitude>();
    registerType<QKnxValueAngleRad>();
    registerType<QKnxValueAngleDeg>();
    registerType<QKnxValueAngularMomentum>();
    registerType<QKnxValueAngularVelocity>();
    registerType<QKnxValueArea>();
    registerType<QKnxValueCapacitance>();
    registerType<QKnxValueChargeDensitySurface>();
    registerType<QKnxValueChargeDensityVolume>();
    registerType<QKnxValueCompressibility>();
    registerType<QKnxValueConductance>();
    registerType<QKnxValueElectricalConductivity>();
    registerType<QKnxValueDensity>();
    registerType<QKnxValueElectricCharge>();
    registerType<QKnxValueElectricCurrent>();
    registerType<QKnxValueElectricCurrentDensity>();
    registerType<QKnxValueElectricDipoleMoment>();
    registerType<QKnxValueElectricDisplacement>();
    registerType<QKnxValueElectricFieldStrength>();
    registerType<QKnxValueElectricFlux>();
    registerType<QKnxValueElectricFluxDensity>();
    registerType<QKnxValueElectricPolarization>();
    registerType<QKnxValueElectricPotential>();
    registerType<QKnxValueElectricPotentialDifference>();
    registerType<QKnxValueElectromagneticMoment>();
    registerType<QKnxValueElectromotiveForce>();
    registerType<QKnxValueEnergy>();
    registerType<QKnxValueForce>();
    registerType<QKnxValueFrequency>();
    registerType<QKnxValueAngularFrequency>();
    registerType<QKnxValueHeatCapacity>();
    registerType<QKnxValueHeatFlowRate>();
    registerType<QKnxValueHeatQuantity>();
    registerType<QKnxValueImpedance>();
    registerType<QKnxValueLength>();
    registerType<QKnxValueLightQuantity>();
    registerType<QKnxValueLuminance>();
    registerType<QKnxValueLuminousFlux>();
    registerType<QKnxValueLuminousIntensity>();
    registerType<QKnxValueMagneticFieldStrength>();
    registerType<QKnxValueMagneticFlux>();
    registerType<QKnxValueMagneticFluxDensity>();
    registerType<QKnxValueMagneticMoment>();
    registerType<QKnxValueMagneticPolarization>();
    registerType<QKnxValueMagnetization>();
    registerType<QKnxValueMagnetomotiveForce>();
    registerType<QKnxValueMass>();
    registerType<QKnxValueMassFlux>();
    registerType<QKnxValueMomentum>();
    registerType<QKnxValuePhaseAngleRad>();
    registerType<QKnxValuePhaseAngleDeg>();
    registerType<QKnxValuePower>();
    registerType<QKnxValuePowerFactor>();
    registerType<QKnxValuePressure>();
    registerType<QKnxValueReactance>();
    registerType<QKnxValueResistance>();
    registerType<QKnxValueResistivity>();
    registerType<QKnxValueSelfInductance>();
    registerType<QKnxValueSolidAngle>();
    registerType<QKnxValueSoundIntensity>();
    registerType<QKnxValueSpeed>();
    registerType<QKnxValueStress>();
    registerType<QKnxValueSurfaceTension>();
    registerType<QKnxValueCommonTemperature>();
    registerType<QKnxValueAbsoluteTemperature>();
    registerType<QKnxValueTemperatureDifference>();
    registerType<QKnxValueThermalCapacity>();
    registerType<QKnxValueThermalConductivity>();
    registerType<QKnxValueThermoelectricPower>();
    registerType<QKnxValueTime>();
    registerType<QKnxValueTorque>();
    registerType<QKnxValueVolume>();
    registerType<QKnxValueVolumeFlux>();
    registerType<QKnxValueWeight>();
    registerType<QKnxValueWork>();

    // DPT-15
    registerType<QKnxEntranceAccess>();

    // DPT-16
    registerType<QKnxCharString>();
    registerType<QKnxCharStringASCII>(); // TODO: this replaces the QKnxCharString (subtype == 0)
    registerType<QKnxCharString88591>();

    // DPT-17
    registerType<QKnxSceneNumber>();

    // DPT-18
    registerType<QKnxSceneControl>();

    // DPT-19
    registerType<QKnxDateTime>();

    // DPT-20
    registerType<QKnx1Byte>();
    registerType<QKnxScloMode>();
    registerType<QKnxBuildingMode>();
    registerType<QKnxOccupyMode>();
    registerType<QKnxPriority>();
    registerType<QKnxLightApplicationMode>();
    registerType<QKnxApplicationArea>();
    registerType<QKnxAlarmClassType>();
    registerType<QKnxPsuMode>();
    registerType<QKnxErrorClassSystem>();
    registerType<QKnxErrorClassHvac>();
    registerType<QKnxTimeDelay>();
    registerType<QKnxBeaufortWindForceScale>();
    registerType<QKnxSensorSelect>();
    registerType<QKnxActuatorConnectType>();
    registerType<QKnxCloudCover>();

    // DPT-21
    registerType<QKnx8BitSet>();
    registerType<QKnxGeneralStatus>();
    registerType<QKnxDeviceControl>();

    // DPT-23
    registerType<QKnx2BitSet>();
    registerType<QKnxOnOffAction>();
    registerType<QKnxAlarmReaction>();
    registerType<QKnxUpDownAction>();

    // DPT-24
    registerType<QKnxVarString>();
    registerType<QKnxVarString88591>();

    // DPT-26
    registerType<QKnxSceneInfo>();

    // DPT-27
    registerType<QKnx32BitSet>();
    registerType<QKnxCombinedInfoOnOff>();

    // DPT-29
    registerType<QKnxElectricalEnergy>();
    registerType<QKnxActiveEnergyV64>();
    registerType<QKnxApparentEnergyV64>();
    registerType<QKnxReactiveEnergyV64>();

    // DPT-28
    registerType<QKnxUtf8String>();
    registerType<QKnxUtf8>();
}

/*!
    \internal
*/
QHash<int, int> &QKnxDatapointTypeFactory::sizeTable()
{
    static QHash<int, int> _instance;
    return _instance;
}

/*!
    \internal
*/
void QKnxDatapointTypeFactory::setTypeSize(int mainType, int size)
{
    sizeTable().insert(mainType, size);
}

QT_END_NAMESPACE
