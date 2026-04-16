#pragma once
#include <opcua.h>
#include <share/config.h>
extern MPLCSHARE_API OpcUa_StatusCode GetLinksParamValue(int64_t id, OpcUa_Variant* value);
//extern MPLCSHARE_API OpcUa_StatusCode SetLinksParamValue(int64_t id,
//                                                          const OpcUa_Variant* value,
//                                                          bool block_one,
//                                                          bool check_block);
