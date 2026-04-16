/* Copyright (c) 1996-2018, OPC Foundation. All rights reserved.

   The source code in this file is covered under a dual-license scenario:
     - RCL: for OPC Foundation members in good-standing
     - GPL V2: everybody else

   RCL license terms accompanied with this source code. See http://opcfoundation.org/License/RCL/1.00/

   GNU General Public License as published by the Free Software Foundation;
   version 2 of the License are accompanied with this source code. See http://opcfoundation.org/License/GPLv2

   This source code is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef _OpcUa_Utilities_H_
#define _OpcUa_Utilities_H_ 1

#include <opcua_platformdefs.h>

OPCUA_BEGIN_EXTERN_C

enum _OpcUa_ProtocolType
{
    OpcUa_ProtocolType_Invalid,
    OpcUa_ProtocolType_Http,
    OpcUa_ProtocolType_Tcp
};
typedef enum _OpcUa_ProtocolType OpcUa_ProtocolType;


/**
 * @brief Sorts an array.
 *
 * @param pElements     [in] The array of elements to sort.
 * @param nElementCount [in] The number of elements in the array.
 * @param nElementSize  [in] The size a single element in the array.
 * @param pfnCompare    [in] The function used to compare elements.
 * @param pContext      [in] A context that is passed to the compare function.
 */
OPCUA_EXPORT
OpcUa_StatusCode OpcUa_QSort(   OpcUa_Void*       pElements,
                                OpcUa_UInt32      nElementCount,
                                OpcUa_UInt32      nElementSize,
                                OpcUa_PfnCompare* pfnCompare,
                                OpcUa_Void*       pContext);

/**
 * @brief Searches a sorted array.
 *
 * @param pKey          [in] The element to find.
 * @param pElements     [in] The array of elements to sort.
 * @param nElementCount [in] The number of elements in the array.
 * @param nElementSize  [in] The size a single element in the array.
 * @param pfnCompare    [in] The function used to compare elements.
 * @param pContext      [in] A context that is passed to the compare function.
 */
OPCUA_EXPORT
OpcUa_Void* OpcUa_BSearch(  OpcUa_Void*       pKey,
                            OpcUa_Void*       pElements,
                            OpcUa_UInt32      nElementCount,
                            OpcUa_UInt32      nElementSize,
                            OpcUa_PfnCompare* pfnCompare,
                            OpcUa_Void*       pContext);

/**
 * @brief Returns the CRT errno constant.
 */
OPCUA_EXPORT
OpcUa_UInt32 OpcUa_GetLastError(void);

/**
 * @brief Returns the number of milliseconds since the system or process was started.
 */
OPCUA_EXPORT
OpcUa_UInt32 OpcUa_GetTickCount(void);

/**
 * @brief Convert string to integer.
 */
#define OpcUa_CharAToInt(xChar) OpcUa_ProxyStub_g_PlatformLayerCalltable->CharToInt(xChar)

/**
 * @brief Returns text description of error (code)
 */
OPCUA_EXPORT
char* OpcUa_GetErrorDescription(OpcUa_StatusCode code);


// InSAT код ниже взят из старой версии. В 1.04 отсутствовал

/**
 * @brief Subtract Value 2 from Value 1 and store the result as rounded number
 *        of seconds in Result.
 *
 * @param a_Value1   [ in] Operand 1
 * @param a_Value2   [ in] Operand 2
 * @param a_puResult [out] Pointer to designated result.
 * @return Statuscode; OpcUa_Good; OpcUa_BadInvalidArgument; OpcUa_BadOutOfRange;
 */
OPCUA_EXPORT OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_GetDateTimeDiffInSeconds32(OpcUa_DateTime  a_Value1, OpcUa_DateTime  a_Value2, OpcUa_UInt32*   a_puResult);

// InSAT 

/// <summary>
/// При запросе на чтение или подписку клиен отправляет типы временных меток, которые он хочет получать от сервера.
/// Функция проверяет типы запрашиваемых временных меток на допустимость.
/// </summary>
/// <param name="a"></param>
/// <returns></returns>
#if defined(WINCE)
__inline OpcUa_Boolean IsValidTimestampsToReturn(OpcUa_TimestampsToReturn a) {
#else
inline OpcUa_Boolean IsValidTimestampsToReturn(OpcUa_TimestampsToReturn a) {
#endif
        return ((a <= OpcUa_TimestampsToReturn_Neither) && (a >= OpcUa_TimestampsToReturn_Source));
}


// InSAT код ниже взят из старой версии. В 1.04 отсутствовал

//InSAT На ARM int64 значение должно быть выровнено по 8-байтной границе
#define OpcUa_DateTime_ToInt64(xDT)     ((OpcUa_Int64)((((OpcUa_UInt64)(xDT).dwHighDateTime) << 32) + (OpcUa_UInt64)((xDT).dwLowDateTime)))
#define OpcUa_DateTime_ToMs(xDT)  (OpcUa_DateTime_ToInt64(xDT) / 10000LL)

/**
 * @brief Get the differenc between two OpcUa_DateTime. (DT = xDT2 - xDT1)
 */
#define OpcUa_DateTime_Diff(xDT2, xDT1) OpcUa_DateTime_FromInt64(OpcUa_DateTime_ToInt64(xDT2)-OpcUa_DateTime_ToInt64(xDT1))

#define OpcUa_DateTime_Diff_AsInt64(xDT2, xDT1) OpcUa_DateTime_FromInt64(OpcUa_DateTime_ToInt64(xDT2)-OpcUa_DateTime_ToInt64(xDT1))

// End InSAT код ниже взят из старой версии. В 1.04 отсутствовал


OPCUA_END_EXTERN_C

#endif /* _OpcUa_Utilities_H_ */
