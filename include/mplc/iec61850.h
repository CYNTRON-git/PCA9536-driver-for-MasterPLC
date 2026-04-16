#pragma once
#include "tls_config.h"
#include "iec61850_client.h"

#ifdef WIN32
#    ifdef MPLC_61850_EXPORTS
#        define MPLC_61850_API __declspec(dllexport)
#    else
#        define MPLC_61850_API __declspec(dllimport)
#    endif
#else
#    define MPLC_61850_API
#endif

MPLC_61850_API unsigned int GetDAQualityBy61850(uint16_t input);

MPLC_61850_API void LoadTLSCerts(TLSConfiguration tlsConfig, const char* path, const char* suffix);

MPLC_61850_API int Mms_To_opcUa_Value(MmsValue* src, OpcUa_VariantHlp* dst);

MPLC_61850_API int Mms_To_opcUa_DataValue(MmsValue* src, OpcUa_VariantHlp* dst);

FILETIME Mms_UTC_Time_To_FileTime(MmsValue* src);

MPLC_61850_API MmsValue* CreateMms_From_opcUa_VariantType(const OpcUa_BuiltInType type);

MPLC_61850_API MmsValue* CreateMms_From_opcUa_Variant(const OpcUa_VariantHlp* src);

MPLC_61850_API void Mms_From_opcUa_Variant(const OpcUa_VariantHlp* src, MmsValue* dst);

MPLC_61850_API IedClientError iedConnection_mapMmsErrorToIedError(MmsError mmsError);

MPLC_61850_API IedClientError iedConnection_mapDataAccessErrorToIedError(MmsDataAccessError mmsError);
