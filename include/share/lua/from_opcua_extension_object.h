#pragma once


MPLCSHARE_API int CopyOpcUaBinaryObjectToLua(const OpcUa_Byte* pOpcUaExtObjectValue,
                            lua_State* L,
                            const OpcUa_BuiltInTypeHlp& varOpcType);

int CopyBinaryObjectToJson(const OpcUa_VariantHlp& srcValue,
                           OpcUa_VariantHlp& destValue,
                           const OpcUa_BuiltInTypeHlp& varOpcType);

int CopyBinaryObjectSingleValueToJson(Value& json,
                                      Document::AllocatorType& allocator,
                                      mplc::lib::string_view name,
                                      const mplc::vm::VmType* type,
                                      OpcUa_Byte* pOpcUaExtObjectValue);

// int CopyJsonToBinaryObject(const OpcUa_VariantHlp& val, const OpcUa_BuiltInTypeHlp& varOpcType, OpcUa_Byte*
// pOpcUaExtObjectValue, OpcUa_Int32 Length);


int CopyBinaryObjectToLua(OpcUa_UInt32 itemId,
                          const std::string& path,
                          const OpcUa_VariantHlp& srcValue,
                          lua_State* L,
                          const OpcUa_BuiltInTypeHlp& varOpcType);
int CopyRawBinaryObjectToLua(OpcUa_UInt32 itemId,
                             const std::string& path,
                             const OpcUa_Byte* pOpcUaExtObjectValue,
                             lua_State* L,
                             const OpcUa_BuiltInTypeHlp& varOpcType);
int CopyRawBinaryObjectArrayToLua(OpcUa_UInt32 itemId,
                                  const std::string& path,
                                  const OpcUa_VariantArrayValue& array,
                                  lua_State* L,
                                  const OpcUa_BuiltInTypeHlp& varOpcType);


int CopyOpcUaEncodeableToLua(const OpcUa_Byte* pOpcUaExtObjectValue,
                             lua_State* L,
                             const OpcUa_BuiltInTypeHlp& varOpcType);
