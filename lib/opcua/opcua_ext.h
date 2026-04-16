#pragma once


OPCUA_BEGIN_EXTERN_C

/**
* @brief Convert OpcUa_Int64 to OpcUa_DateTime. (DT = OpcUa_DateTime_FromInt64(x64))
*/
//  #define OpcUa_DateTime_FromInt64(x64)   *((OpcUa_DateTime*)&x64)
OPCUA_EXPORT OpcUa_DateTime OpcUa_DateTime_FromInt64(OpcUa_Int64 x64);
//OpcUa_DateTime OpcUa_DateTime_FromInt64(OpcUa_Int64 x64);


//InSAT На ARM int64 значение должно быть выровнено по 8-байтной границе
#define OpcUa_DateTime_ToInt64(xDT)     ((OpcUa_Int64)((((OpcUa_UInt64)(xDT).dwHighDateTime) << 32) + (OpcUa_UInt64)((xDT).dwLowDateTime)))
#define OpcUa_DateTime_ToMs(xDT)  (OpcUa_DateTime_ToInt64(xDT) / 10000LL)


OPCUA_END_EXTERN_C
