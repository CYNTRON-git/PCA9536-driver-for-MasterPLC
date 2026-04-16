#pragma once

#include <map>
#include <string>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>
#include <list>

// #include <stdint.h>
#include <core/main.h>
#include <mplc/libs/containers.hpp>
#include <mplc/libs/string_view.hpp>
#include <rapidjson/rapidjson.h>
#include <share/filetime.h>
#include <mplc/date_time.h>

#include "rapidjson/document.h"
// clang-format on
#define COMPARE_VALS(val1, val2) ((val1) > (val2) ? 1 : ((val1) < (val2) ? -1 : 0))

// null terminated string
inline bool need_decode_to_cp1251(const char* utf8) {
    while (*utf8 && !(*utf8++ & 0x80)) {
    }
    return *utf8 != 0;
}
// MPLCSHARE_API size_t cp1251_to_utf8_diff(const char* cp1251, int len);

MPLCSHARE_API OpcUa_StatusCode ConvertStringToCP1251(const OpcUa_String* srcStr, OpcUa_CharA** dstStrChars);

#define TYPEHASH_NUMBER 1
#define TYPEHASH_FILETIME 2
#define TYPEHASH_BOOLEAN 3
#define TYPEHASH_STRING 4
#define TYPEHASH_ENUM 5
#define TYPEHASH_INTEGER 6
#define TYPEHASH_ANY 7

#define SYSTEM_REAL_PARAM_NAME "SYSTEM_REAL_PARAM"
#define SYSTEM_LREAL_PARAM_NAME "SYSTEM_LREAL_PARAM"
#define SYSTEM_INT_PARAM_NAME "SYSTEM_INT_PARAM"
#define SYSTEM_DINT_PARAM_NAME "SYSTEM_DINT_PARAM"
#define SYSTEM_BOOL_PARAM_NAME "SYSTEM_BOOL_PARAM"
#define SYSTEM_BYTE_PARAM_NAME "SYSTEM_BYTE_PARAM"
#define SYSTEM_WORD_PARAM_NAME "SYSTEM_WORD_PARAM"
#define SYSTEM_DWORD_PARAM_NAME "SYSTEM_DWORD_PARAM"
#define SYSTEM_LWORD_PARAM_NAME "SYSTEM_LWORD_PARAM"
#define SYSTEM_DT_PARAM_NAME "SYSTEM_DT_PARAM"
#define SYSTEM_STRING_PARAM_NAME "SYSTEM_STRING_PARAM"
#define SYSTEM_LINT_PARAM_NAME "SYSTEM_LINT_PARAM"
#define SYSTEM_DOUBLE_POINT_PARAM_NAME "SYSTEM_DOUBLE_POINT_PARAM"
#define SYSTEM_SINT_PARAM_NAME "SYSTEM_SINT_PARAM"
#define SYSTEM_DATE_PARAM_NAME "SYSTEM_DATE_PARAM"
#define SYSTEM_TIME_PARAM_NAME "SYSTEM_TIME_PARAM"
#define SYSTEM_TOD_PARAM_NAME "SYSTEM_TOD_PARAM"

#define TRUE_STR "true"
#define FALSE_STR "false"

#define OpcUa_DateTime_From_FILETIME(xFT) *((OpcUa_DateTime*)&xFT)

/*static const uint64_t ConvTable[27]
{ 0,
1 << OpcUaType_Boolean || 1 << OpcUaType_SByte || 1 << OpcUaType_Byte || 1 << OpcUaType_Int16  ,
1 << OpcUaType_Boolean || 1 << OpcUaType_SByte || 1 << OpcUaType_Byte || 1 << OpcUaType_Int16  ,
1 << OpcUaType_Boolean || 1 << OpcUaType_SByte || 1 << OpcUaType_Byte || 1 << OpcUaType_Int16  ,
1 << OpcUaType_Boolean || 1 << OpcUaType_SByte || 1 << OpcUaType_Byte || 1 << OpcUaType_Int16
};*/

namespace mplc {
    class OpcUa_Json;
    namespace vm {
        struct VmType;
        // struct VMType;
        class VMInfo;
    }  // namespace vm
}  // namespace mplc

class WeakMemBuf;

/// Описание типа узла модели OPC UA
class OpcUa_BuiltInTypeHlp {
public:
    // const mplc::vm::VMType* vmType;
    const mplc::vm::VmType* vmType;
    OpcUa_BuiltInType Type;
    OpcUa_BuiltInType SubType;  // For OpcUaType_DataValue
    OpcUa_Int32 ArrayFrom;
    OpcUa_Int32 ArraySize;
    OpcUa_Byte ArrayType;
    OpcUa_Byte IsDynamic;

    OpcUa_BuiltInTypeHlp(): ArrayType(OpcUa_VariantArrayType_Scalar) {
        Type = SubType = OpcUaType_Null;
        ArrayFrom = 0;
        ArraySize = 0;
        IsDynamic = 0;
        vmType = nullptr;
    }
    virtual ~OpcUa_BuiltInTypeHlp() {}
    OpcUa_BuiltInTypeHlp(const OpcUa_BuiltInTypeHlp& val) = default;
    OpcUa_BuiltInTypeHlp(OpcUa_BuiltInTypeHlp&& val) = default;
    OpcUa_BuiltInTypeHlp& operator=(const OpcUa_BuiltInTypeHlp& val) = default;
    OpcUa_BuiltInTypeHlp& operator=(OpcUa_BuiltInTypeHlp&& val) = default;
    MPLCSHARE_API bool operator==(OpcUa_BuiltInTypeHlp val) const;
    MPLCSHARE_API bool operator==(OpcUa_BuiltInType val) const;

    OpcUa_BuiltInTypeHlp(OpcUa_BuiltInType type) {
        Type = type;
        SubType = OpcUaType_Null;
        ArrayType = OpcUa_VariantArrayType_Scalar;
        ArrayFrom = 0;
        ArraySize = 0;
        IsDynamic = 0;
        vmType = nullptr;
    }
    bool isNumber() const {
        if (ArrayType != OpcUa_VariantArrayType_Scalar)
            return false;
        switch (Type) {
        case OpcUaType_SByte:
        case OpcUaType_Byte:
        case OpcUaType_Int16:
        case OpcUaType_UInt16:
        case OpcUaType_Int32:
        case OpcUaType_UInt32:
        case OpcUaType_Int64:
        case OpcUaType_UInt64:
        case OpcUaType_Float:
        case OpcUaType_Double:
            return true;
        default:
            return false;
        }
    }
    MPLCSHARE_API OpcUa_StatusCode GetSubType(OpcUa_BuiltInTypeHlp& type, mplc::lib::string_view path = "") const;

    MPLCSHARE_API mplc::lib::string_view GetTypeName() const;
    MPLCSHARE_API static OpcUa_Int32 GetTypeSize(OpcUa_BuiltInType type);

    // MPLCSHARE_API void GetFirstChild(OpcUa_BuiltInTypeHlp& type);
    // MPLCSHARE_API std::vector<OpcUa_BuiltInTypeHlp> GetChilds();
};

class OpcUa_VariantHlp : public OpcUa_Variant {
    // BOOST_COPYABLE_AND_MOVABLE_ALT(OpcUa_VariantHlp)
public:
    OpcUa_VariantHlp() {
        OpcUa_Variant_Initialize(this);
    }

    OpcUa_VariantHlp(const OpcUa_VariantHlp& source) {
        //        if(source.Datatype == OpcUaType_DataValue )
        //            printf("---- src %d %d\r\n",source.ArrayType, source.Value.DataValue->Value.Datatype);
        CopyVariants(*this, source, false);
        //        if (source.Datatype == OpcUaType_DataValue && source.Value.DataValue->Value.Datatype ==
        //        OpcUaType_String && source.Value.DataValue->Value.ArrayType == 0 &&
        //        (*this).Value.DataValue->Value.Value.String.strContent != NULL)
        //            printf("---- res src %d %d %p\r\n", source.ArrayType, source.Value.DataValue->Value.Datatype,
        //            *this->Value.DataValue->Value.Value.String.strContent);
    }

    OpcUa_VariantHlp(const OpcUa_Variant& source) {
        CopyVariants(*this, source, false);
    }
    OpcUa_VariantHlp& operator=(const OpcUa_VariantHlp& source) noexcept {
        CopyVariants(*this, source, true);
        return *this;
    }

    /*OpcUa_VariantHlp(BOOST_RV_REF(OpcUa_VariantHlp) source) noexcept {
        Swap(source);
    }

    OpcUa_VariantHlp& operator=(BOOST_RV_REF(OpcUa_VariantHlp) source) noexcept {
        Swap(source);
        return *this;
    }*/
    ~OpcUa_VariantHlp() {
        Clear();
    }
    void CopyFrom(const OpcUa_VariantHlp& source) {
        CopyVariants(*this, source, true);
    }

    void CopyFrom(const OpcUa_Variant& source) {
        CopyVariants(*this, source, true);
    }

    void Swap(OpcUa_VariantHlp& other) {
        std::swap(ArrayType, other.ArrayType);
        std::swap(Datatype, other.Datatype);
        std::swap(Value, other.Value);
    }

    void Swap(OpcUa_Variant& other) {
        std::swap(ArrayType, other.ArrayType);
        std::swap(Datatype, other.Datatype);
        std::swap(Value, other.Value);
    }

    OpcUa_BuiltInType GetType() const {
        return (OpcUa_BuiltInType)Datatype;
    }

    static int GetTypeHash(OpcUa_BuiltInType datatype) {
        switch (datatype) {
        case OpcUaType_Boolean:
            return TYPEHASH_BOOLEAN;
        case OpcUaType_Byte:
        case OpcUaType_SByte:
        case OpcUaType_Int16:
        case OpcUaType_UInt16:
        case OpcUaType_Int32:
        case OpcUaType_UInt32:
        case OpcUaType_Int64:
        case OpcUaType_UInt64:
            return TYPEHASH_INTEGER;
        case OpcUaType_Double:
        case OpcUaType_Float:
            return TYPEHASH_NUMBER;
        case OpcUaType_String:
            return TYPEHASH_STRING;
        case OpcUaType_DateTime:
            return TYPEHASH_FILETIME;
        case OpcUaType_Variant:
            return TYPEHASH_ANY;
        default:
            return 0;
        }
    }

    static OpcUa_BuiltInType GetUaTypeFromHash(int typeHash) {
        switch (typeHash) {
        case TYPEHASH_NUMBER:
            return OpcUaType_Double;
        case TYPEHASH_INTEGER:
            return OpcUaType_Int64;
        case TYPEHASH_BOOLEAN:
            return OpcUaType_Boolean;
        case TYPEHASH_FILETIME:
            return OpcUaType_DateTime;
        case TYPEHASH_ENUM:
            return OpcUaType_Int32;
        case TYPEHASH_STRING:
            return OpcUaType_String;
        case TYPEHASH_ANY:
            return OpcUaType_Variant;
        default:
            return OpcUaType_Null;
        }
    }
    int Compare(const OpcUa_VariantHlp& value) const {
        return CompareVariants(*this, value);
    }

    void Clear() {
        OpcUa_Variant_Clear(this);
    }
    OpcUa_StatusCode SetCP1251String(const std::string& s) {
        return SetCP1251String(s.c_str(), s.size());
    }
    static OpcUa_StatusCode ConvertToUTF8String(std::string& target, const std::string& source) {
        return ConvertToUTF8String(target, source.c_str(), source.size());
    }
    static OpcUa_StatusCode ConvertFromUTF8String(std::string& target, const std::string& source) {
        return ConvertFromUTF8String(target, source.c_str(), source.size());
    }
    OpcUa_StatusCode SetString(const std::string& str) {
        return SetString(str.c_str(), str.size());
    }
    OpcUa_StatusCode SetString(const char* s) {
        return SetString(s, s ? strlen(s) : 0);
    }

    MPLCSHARE_API static OpcUa_BuiltInType GetUaTypeFromName(const std::string& name);
    MPLCSHARE_API static OpcUa_StatusCode GetUaTypeHlpFromName(const std::string& name, OpcUa_BuiltInTypeHlp& type);
    MPLCSHARE_API static OpcUa_StatusCode GetUaTypeHlpFromNameExt(const std::string& name, OpcUa_BuiltInTypeHlp& type);
    MPLCSHARE_API static OpcUa_BuiltInType GetUaTypeFromInternalName(const std::string& name);
    MPLCSHARE_API static OpcUa_StatusCode ConvAndCopy(const OpcUa_Variant& source,
                                                      OpcUa_Variant& dest,
                                                      OpcUa_BuiltInType type);
    MPLCSHARE_API static OpcUa_StatusCode ConvertToUTF8String(std::string& target, const char* source, int size = -1);
    MPLCSHARE_API static OpcUa_StatusCode ConvertFromUTF8String(std::string& target, const char* source, int size = -1);
    MPLCSHARE_API static OpcUa_StatusCode GetBool(const OpcUa_Variant& source, OpcUa_Boolean& v);
    MPLCSHARE_API static OpcUa_StatusCode GetDouble(const OpcUa_Variant& source, double& v);
    MPLCSHARE_API static OpcUa_StatusCode GetInt(const OpcUa_Variant& source, OpcUa_Int32& v);
    MPLCSHARE_API static OpcUa_StatusCode GetInt16(const OpcUa_Variant& source, OpcUa_Int16& v);
    MPLCSHARE_API static OpcUa_StatusCode GetFloat(const OpcUa_Variant& source, OpcUa_Float& v);
    MPLCSHARE_API static int CompareVariants(const OpcUa_Variant& v1, const OpcUa_Variant& v2);
    MPLCSHARE_API static void CopyVariants(OpcUa_Variant& target, const OpcUa_Variant& source, bool clear);
    MPLCSHARE_API static const char* GetUaTypeName(OpcUa_BuiltInType type);

    MPLCSHARE_API bool IsContains(const OpcUa_VariantHlp& sub_str) const;
    MPLCSHARE_API OpcUa_StatusCode mul(float val);
    MPLCSHARE_API OpcUa_StatusCode Normalize(OpcUa_StatusCode* ret_sc = nullptr, OpcUa_DateTime* ret_dt = nullptr);
    MPLCSHARE_API OpcUa_StatusCode ChangeType(OpcUa_BuiltInTypeHlp const& type);
    MPLCSHARE_API OpcUa_StatusCode ChangeType(OpcUa_BuiltInType type);

    MPLCSHARE_API OpcUa_StatusCode SetCP1251String(const char* s, size_t size);
    MPLCSHARE_API OpcUa_StatusCode SetJson(const rapidjson::Value& data);
    MPLCSHARE_API OpcUa_StatusCode SetJson(mplc::OpcUa_Json& rv_data, bool move = true);
    MPLCSHARE_API OpcUa_StatusCode SetString(const char* s, int size);
    MPLCSHARE_API OpcUa_StatusCode SetTime(int64_t ft);
    MPLCSHARE_API OpcUa_StatusCode SetTime(FILETIME ft);
    MPLCSHARE_API OpcUa_StatusCode SetTime(OpcUa_DateTime dt);
    MPLCSHARE_API OpcUa_StatusCode SetBool(bool v);
    MPLCSHARE_API OpcUa_StatusCode SetByte(int v);
    MPLCSHARE_API OpcUa_StatusCode SetSByte(int v);
    MPLCSHARE_API OpcUa_StatusCode SetInt(int v);
    MPLCSHARE_API OpcUa_StatusCode SetUInt(OpcUa_UInt32 v);
    MPLCSHARE_API OpcUa_StatusCode SetInt16(int v);
    MPLCSHARE_API OpcUa_StatusCode SetUInt16(OpcUa_UInt16 v);
    MPLCSHARE_API OpcUa_StatusCode SetInt64(OpcUa_Int64 v);
    MPLCSHARE_API OpcUa_StatusCode SetUInt64(OpcUa_UInt64 v);
    MPLCSHARE_API OpcUa_StatusCode SetFloat(float v);
    MPLCSHARE_API OpcUa_StatusCode SetDouble(double v);
    MPLCSHARE_API OpcUa_StatusCode SetDataValue(OpcUa_VariantHlp& value,
                                                OpcUa_DateTime ft,
                                                OpcUa_StatusCode statusCode);
    MPLCSHARE_API OpcUa_StatusCode SetDataValue(OpcUa_VariantHlp& value,
                                                const FILETIME& ft,
                                                OpcUa_StatusCode statusCode) {
        return SetDataValue(value, OpcUa_DateTime_From_FILETIME(ft), statusCode);
    }
    MPLCSHARE_API OpcUa_StatusCode SetDataValue(OpcUa_VariantHlp& value,
                                                int64_t sourceTime,
                                                OpcUa_StatusCode statusCode) {
        return SetDataValue(value, OpcUa_DateTime_FromInt64(sourceTime), statusCode);
    }
    MPLCSHARE_API OpcUa_StatusCode SetDataValue(const OpcUa_DataValue& value);
    MPLCSHARE_API OpcUa_StatusCode SetDataValue(OpcUa_DataValue&& value);
    MPLCSHARE_API OpcUa_StatusCode SetStatusCode(OpcUa_StatusCode status_code);
    MPLCSHARE_API OpcUa_StatusCode SetSourceTime(OpcUa_DateTime source_time);
    MPLCSHARE_API OpcUa_StatusCode SetByteString(const char* s, int size);
    MPLCSHARE_API OpcUa_StatusCode SetByteString(const OpcUa_ByteString& value);

    MPLCSHARE_API OpcUa_StatusCode GetBool(bool& v) const;
    MPLCSHARE_API OpcUa_StatusCode GetInt(int& v) const;
    MPLCSHARE_API OpcUa_StatusCode GetInt16(OpcUa_Int16& v) const;
    MPLCSHARE_API OpcUa_StatusCode GetInt64(OpcUa_Int64& v) const;
    MPLCSHARE_API OpcUa_StatusCode GetDouble(double& v) const;
    MPLCSHARE_API OpcUa_StatusCode GetFloat(float& v) const;
    MPLCSHARE_API OpcUa_StatusCode GetTime(OpcUa_DateTime& dt) const;
    MPLCSHARE_API OpcUa_StatusCode GetTime(int64_t& time, int32_t magnitude = 1) const;
    MPLCSHARE_API OpcUa_StatusCode GetDateTime(OpcUa_DateTime& dt) const;
    MPLCSHARE_API OpcUa_StatusCode GetString(std::string& v) const;
    MPLCSHARE_API OpcUa_StatusCode GetStringValCP1251(OpcUa_CharA** dstStrChars) const;
    MPLCSHARE_API OpcUa_StatusCode GetStringValCP1251(std::string& v) const;
    MPLCSHARE_API OpcUa_StatusCode GetString(const OpcUa_Variant& source, OpcUa_String& v);
    MPLCSHARE_API OpcUa_StatusCode GetByteString(OpcUa_ByteString& v) const;

    MPLCSHARE_API const OpcUa_VariantUnion& GetRawValue() const;
    MPLCSHARE_API mplc::OpcUa_Json& GetJson();
    MPLCSHARE_API const mplc::OpcUa_Json& GetJson() const;
    MPLCSHARE_API int CopyJsonToBinaryObject(const OpcUa_BuiltInTypeHlp& varOpcType, WeakMemBuf& buf);
    MPLCSHARE_API int ConvertJsonToBinaryObject(const OpcUa_BuiltInTypeHlp& varOpcType);

    template<class T>
    OpcUa_StatusCode Set(T val) {
        static_assert(sizeof(T) != 0, "OpcUa_VariantHlp::Set for type");
        return OpcUa_Bad;
    }
    template<class T>
    OpcUa_StatusCode Set(T val, OpcUa_DateTime source_time, OpcUa_StatusCode status_code) {
        OpcUa_VariantHlp tmp;
        OpcUa_ReturnErrorIfBad(tmp.Set(val));
        OpcUa_ReturnErrorIfBad(SetDataValue(tmp, source_time, status_code));
        return OpcUa_Good;
    }
    template<class T>
    T Get(const T& def_val = T{}) const {
        static_assert(sizeof(T) != 0, "OpcUa_VariantHlp::Get for type");
        return def_val;
    }

protected:
    template<typename T>
    static void CreateAndCopyArray(const OpcUa_Variant& source, OpcUa_Variant& target) {
        target.Value.Array.Length = source.Value.Array.Length;
        target.Value.Array.Value.Array = (T*)OpcUa_Alloc(source.Value.Array.Length * sizeof(T));
        memcpy(target.Value.Array.Value.Array, source.Value.Array.Value.Array, sizeof(T) * source.Value.Array.Length);
    }

    void CreateDataValue() {
        if (Datatype != OpcUaType_DataValue) {
            Clear();
            Datatype = OpcUaType_DataValue;
            Value.DataValue = OpcUa_DataValue_AllocAndInit();
        } else {
            OpcUa_DataValue_Clear(Value.DataValue);
        }
    }
};

template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<FILETIME>(FILETIME val) {
    return SetTime(val);
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<OpcUa_DateTime>(OpcUa_DateTime val) {
    return SetTime(val);
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<mplc::DateTime>(mplc::DateTime val) {
    return SetTime(val.ticks());
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<bool>(bool val) {
    return SetBool(val);
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<OpcUa_Int32>(OpcUa_Int32 val) {
    return SetInt(val);
}

template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<OpcUa_UInt32>(OpcUa_UInt32 val) {
    return SetUInt(val);
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<OpcUa_UInt16>(OpcUa_UInt16 val) {
    return SetUInt16(val);
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<OpcUa_Int16>(OpcUa_Int16 val) {
    return SetInt16(val);
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<OpcUa_Byte>(OpcUa_Byte val) {
    return SetByte(val);
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<OpcUa_Int64>(OpcUa_Int64 val) {
    return SetInt64(val);
}

template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<OpcUa_UInt64>(OpcUa_UInt64 val) {
    return SetUInt64(val);
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<float>(float val) {
    return SetFloat(val);
}

template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<double>(double val) {
    return SetDouble(val);
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<OpcUa_DataValue>(OpcUa_DataValue val) {
    return SetDataValue(val);
}

template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<FileTime>(FileTime val) {
    return SetTime(val.dt());
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<std::string>(std::string val) {
    return SetString(val);
}
template<>
inline OpcUa_StatusCode OpcUa_VariantHlp::Set<OpcUa_VariantHlp>(OpcUa_VariantHlp val) {
    CopyFrom(val);
    return OpcUa_Good;
}

// Get -------

template<>
inline FileTime OpcUa_VariantHlp::Get<FileTime>(const FileTime& def_val) const {
    int64_t ft;
    if (OpcUa_IsBad(GetTime(ft))) {
        return def_val;
    }
    return ft;
}

template<>
inline OpcUa_DateTime OpcUa_VariantHlp::Get<OpcUa_DateTime>(const OpcUa_DateTime& def_val) const {
    OpcUa_DateTime ft;
    if (OpcUa_IsBad(GetTime(ft))) {
        return def_val;
    }
    return ft;
}
template<>
inline mplc::DateTime OpcUa_VariantHlp::Get<mplc::DateTime>(const mplc::DateTime& def_val) const {
    OpcUa_DateTime ft;
    if (OpcUa_IsBad(GetTime(ft))) {
        return def_val;
    }
    return ft;
}
template<>
inline bool OpcUa_VariantHlp::Get<bool>(const bool& def_val) const {
    bool val;
    if (OpcUa_IsBad(GetBool(val))) {
        return def_val;
    }
    return val;
}
template<>
inline OpcUa_Int64 OpcUa_VariantHlp::Get<OpcUa_Int64>(const OpcUa_Int64& def_val) const {
    OpcUa_Int64 val;
    if (OpcUa_IsBad(GetInt64(val))) {
        return def_val;
    }
    return val;
}
template<>
inline OpcUa_UInt64 OpcUa_VariantHlp::Get<OpcUa_UInt64>(const OpcUa_UInt64& def_val) const {
    return Get<OpcUa_Int64>(def_val);
}
template<>
inline OpcUa_Int32 OpcUa_VariantHlp::Get<OpcUa_Int32>(const OpcUa_Int32& def_val) const {
    OpcUa_Int32 val;
    if (OpcUa_IsBad(GetInt(val))) {
        return def_val;
    }
    return val;
}
template<>
inline OpcUa_UInt32 OpcUa_VariantHlp::Get<OpcUa_UInt32>(const OpcUa_UInt32& def_val) const {
    return Get<OpcUa_Int64>(def_val);
}
template<>
inline OpcUa_Int16 OpcUa_VariantHlp::Get<OpcUa_Int16>(const OpcUa_Int16& def_val) const {
    OpcUa_Int16 val;
    if (OpcUa_IsBad(GetInt16(val))) {
        return def_val;
    }
    return val;
}
template<>
inline OpcUa_UInt16 OpcUa_VariantHlp::Get<OpcUa_UInt16>(const OpcUa_UInt16& def_val) const {
    return Get<OpcUa_Int16>(def_val);
}
template<>
inline OpcUa_Byte OpcUa_VariantHlp::Get<OpcUa_Byte>(const OpcUa_Byte& def_val) const {
    return Get<OpcUa_Int16>(def_val);
}
template<>
inline OpcUa_SByte OpcUa_VariantHlp::Get<OpcUa_SByte>(const OpcUa_SByte& def_val) const {
    return Get<OpcUa_Int16>(def_val);
}
template<>
inline OpcUa_CharA OpcUa_VariantHlp::Get<OpcUa_CharA>(const OpcUa_CharA& def_val) const {
    return Get<OpcUa_Int16>(def_val);
}
template<>
inline float OpcUa_VariantHlp::Get<float>(const float& def_val) const {
    float val;
    if (OpcUa_IsBad(GetFloat(val))) {
        return def_val;
    }
    return val;
}
template<>
inline double OpcUa_VariantHlp::Get<double>(const double& def_val) const {
    double val;
    if (OpcUa_IsBad(GetDouble(val))) {
        return def_val;
    }
    return val;
}

template<>
inline std::string OpcUa_VariantHlp::Get<std::string>(const std::string& def_val) const {
    std::string val;
    if (OpcUa_IsBad(GetString(val))) {
        return def_val;
    }
    return val;
}

typedef std::vector<OpcUa_VariantHlp> OpcUa_VariantArray;
typedef std::list<OpcUa_VariantHlp> OpcUa_VariantList;
typedef std::map<std::string, OpcUa_VariantHlp> OpcUa_StringToVariantMap;
