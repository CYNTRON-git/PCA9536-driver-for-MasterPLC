#pragma once

#include <string>
#include "mplc/driver/config.h"
namespace mplc { namespace drv_common { namespace messages {

    enum ErrorMessages {
        NoMessage = -1,
        // общие ошибки
        MessageErrorNotSupported = 0,
        MessageErrorIncorrectIp,
        MessageErrorIncorrectPort,
        MessageErrorIncorrectComPort,
        MessageErrorIncorrectReserved,
        MessageErrorIncorrectRequestDelay,
        MessageErrorManyPortsInProtocol,
        // ошибки адресации тегов
        MessageErrorIncorrectTagType,
        MessageErrorIncorrectDeviceAddress,
        MessageErrorIncorrectCrossAddress,
        MessageErrorIncorrectTagAddress,
        MessageErrorIncorrectTagBitAddress,
        MessageErrorIncorrectTagLengthString,
        MessageErrorIncorrectCountRegisters,
        MessageErrorIncorrectCountBits,
        MessageErrorHasDublicate,
        MessageErrorHasUnsupportedWrite,
        MessageErrorGroupOfChannelNotSupported,
        MessageErrorUnsupportedType,
        MessageErrorUnsupportedRegion,
        MessageErrorNotOctalFormat,
        MessageIncorrectRequestDelay,
        // ошибки работы с архивом
        MessageErrorNotArchiveTag,
        MessageErrorArchiveTag,
        MessageErrorCreateTag,
        MessageErrorNotArchivePointer,
        MessageErrorIncorrectSettingsArchive,
        // для Kvantor
        MessageErrorIncorrectSlot,
        MessageErrorIncorrectChannel,
        MessageErrorRangeNotSet,
        // для Helvar
        MessageErrorIncorrectQueryCode,
        MessageErrorIncorrectSubnet,
        MessageErrorIncorrectDevice,
        MessageErrorIncorrectSubDevice,
        MessageErrorIncorrectRouter,
        MessageErrorIncorrectCluster,
        MessageErrorIncorrectGroup,
        MessageErrorIncorrectBlock,
        MessageErrorUnsupportedTag,

        // для CANopen
        MessageDublicateModule,
        MessageNoEdsFile,
        MessageErrorAddPdoMapping,

        // для ABAK диагностики
        MessageDublicateDiagnosticModule,
        MessageNotFoundedTag,

        // для WBIO
        MessageIncorrectNumberModule,
        MessageIncorrectTypeModule,
        MessageVariableNotFound,
        MessageBusyAnother,
        MessageDublicateModuleWbio,
        MessageNoAnalogChannels,
        MessageAnalogChannelNotFind,
        MessageDiscreteChannelNotFind,

        // BACNET
        MessageErrorIncorrectMac,
        MessageUnknownObjectType,
        MessageUnknownPropertyType,
        MessageIncorrectObjectNumber,
        MessageDublicateTag,
        MessageIncorrectPriority,
        MessageIncorrectProcessIdentifier,
        MessageIncorrectPeriodResubscribe,
        // для EK-270
        MessageErrorIncorrectLock,

        // для EthernetIP
        MessageErrorInSomeTags,
        MessageErrorIncorrectBackplane,

        // SNMP Trap Reciever
        MessageSnmpDublicateModule,
        MessageSnmpIncorrectGeneric,
        MessageSnmpIncorrectEnterpriseOID,
        MessageSnmpIncorrectSnmpTrapOID,
        MessageSnmpIncorrectOidValue,
        MessageSnmpEmptyFilter,
        MessageSnmpDublicateChannel,
        MessageSnmpEmptyMessage,
        MessageSnmpIncorrectSeverity,
        MessageSnppErrorConstruct,

        // OMRON
        MessageErrorIncorrectReservedPort,
        MessageErrorIncorrectSourceNetworkList,
        MessageErrorIncorrectSourceNodeList,
        MessageErrorIncorrectDestinationNetworkList,
        MessageErrorIncorrectDestinationNodeList,
        MessageErrorIncorrectDestinationUnitList,
        MessageErrorIncorrectListIpAddress,

        // Mitsubishi
        MessageErrorIncorrectDestinationStationList,
        MessageErrorIncorrectDestinationModuleList,

        // SMPP
        MessageSmppValidatyPeriod,
        
        // специализированные ошибки
        MessageErrorGetCheckout,
        MessageErrorUnknownError,
    };

    // для выдачи ошибок на выход ФБ
    enum ErrorMessagesFB {
        MessageErrorGetProtocol = 0,
        MessageProtocolNotWorked,
        // HELVAR
        MessageHelvarNoModuleFound,
        MessageHelvarModuleNotWorked,
        MessageHelvarGetErrorCode,
        MessageHelvarIncorectValues,
        MessageHelvarErrorExecuteRequest,
        // SMPP
        MessageSmppIncorectSubscriber,
        //SNMP Trap sender
        MessageSnmpSenderIncorrectStruct,
        MessageSnmpSenderIncorrectOidValues,
        MessageSnmpSenderIncorrectValues,
        MessageSnmpSenderIncorrectDataTypes,
        MessageSnmpSenderIncorrectAddress,
        MessageSnmpSenderIncorrectGenericOid,
        MessageSnmpSenderIncorrectEnterpriseOid,
        MessageSnmpSenderIncorrectTrapOid,
        MessageSnmpSenderIncorrectSecurityLevel,
        MessageSnmpSenderIncorrectAuthAlg,
        MessageSnmpSenderIncorrectPrivProt,

    };

    // общее сообщение с преамбулой
    MPLCCOMMON_API void ShowMessages(lua_State* L, ErrorMessages TypeMessage, std::string Preambula = "");

    // сообщение Logica SpbBUS с номерами параметров
    MPLCCOMMON_API void ShowMessagesSpBus(lua_State* L,
                                         ErrorMessages TypeMessage,
                                         int NumChannelValue,
                                         int NumParameterValue,
                                         int TypeArchiveValue);

    // сообщение номером OBIS для DLMS
    MPLCCOMMON_API void ShowMessagesForDLMSChannel(lua_State* L, ErrorMessages TypeMessage, std::string OBIS);

    MPLCCOMMON_API void ShowMessagesAddressDevice(lua_State* L, ErrorMessages TypeMessage, int AddressDevice);

    MPLCCOMMON_API std::string GetTextMessage(ErrorMessagesFB ErrorCode);

}}}  // namespace mplc::drv_common::messages
