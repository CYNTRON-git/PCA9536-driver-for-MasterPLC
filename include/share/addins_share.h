#pragma once

#include <opcua.h>
#ifndef PLC_GCC2
#    include <limits>
inline bool my_isinf(double value) {
    return std::numeric_limits<double>::has_infinity && value == std::numeric_limits<double>::infinity();
}

#else
#    include <cmath>
inline bool my_isinf(double value) {
    return std::isinf(value);
}
#endif

inline bool my_isnan(const double value) {
    return value != value;
}

#undef RAPIDJSON_ASSERT
#define RAPIDJSON_ASSERT(x)                                                                                            \
    if (!(x)) {                                                                                                        \
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "<--Assert " #x " failed\n");                                             \
    }
// #undef RAPIDJSON_HAS_STDSTRING
// #define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>  // rapidjson's DOM-style API
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer_wrapper.h>
#include <share/opcua_json.h>
#include <mplc/libs/string_view.hpp>

using rapidjson::Document;
using rapidjson::StringBuffer;
using rapidjson::UTF8;
using rapidjson::Value;

#if !defined(BOOST_NO_CXX11_TEMPLATE_ALIASES)
// template<class OS>
// using JsonTextWriter = rapidjson::Writer<OS>;

template<class Writer>
using JsonWriter = rapidjson::WriterWrapper<Writer>;

// template<class OS>
// using JsonTextStream = JsonWriter<JsonTextWriter<OS>>;

#else
// #    define JsonTextWriter rapidjson::Writer
#    define JsonWriter rapidjson::WriterWrapper
// template<class OS>
// struct JsonTextStream  {
//    typedef JsonWriter<JsonTextWriter<OS> > type;
//};
#endif

template<class OS>
struct JsonTextStream {
    typedef JsonWriter<rapidjson::Writer<OS> > type;
};

// case-insensitive string comparison
// This code based on code from
// "The C++ Programming Language, Third Edition" by Bjarne Stroustrup
MPLCSHARE_API bool stringsAreEqual(const std::string& s1, const std::string& s2);

// case-insensitive string comparison
MPLCSHARE_API bool stringsAreEqual(const std::string& s1, const std::string& s2, size_t n);

#define FT_MICROSECOND ((INT64)10)
#define FT_MILLISECOND (1000 * FT_MICROSECOND)
#define FT_SECOND (1000 * FT_MILLISECOND)
#define FT_MINUTE (60 * FT_SECOND)
#define FT_HOUR (60 * FT_MINUTE)
#define FT_DAY (24 * FT_HOUR)

static inline int64_t filetime_to_timetms(OpcUa_Int64 t) {
    return t == 0 ? 0 : (t / 10000ULL - 11644473600000ULL);
}

static inline OpcUa_Int64 timetms_to_filetime(int64_t t) {
    return t == 0 ? 0 : (t + 11644473600000ULL) * 10000ULL;
}

inline int64_t getTimePointMS() {
    return getTimePoint() / FT_MILLISECOND;
}

inline int64_t DeltaTimePointMS(int64_t begin, int64_t end) {
    return end - begin;
}

#include "opcua_variant_hlp.h"

struct SimpleOStream {
    virtual ~SimpleOStream() = default;
    typedef UTF8<>::Ch Ch;
    virtual void Put(char c) = 0;

    virtual void Flush() = 0;
    // Not implemented
    char Peek() const {
        RAPIDJSON_ASSERT(false);
        return 0;
    }
    char Take() {
        RAPIDJSON_ASSERT(false);
        return 0;
    }
    size_t Tell() const {
        RAPIDJSON_ASSERT(false);
        return 0;
    }
    char* PutBegin() {
        RAPIDJSON_ASSERT(false);
        return 0;
    }
    size_t PutEnd(char*) {
        RAPIDJSON_ASSERT(false);
        return 0;
    }
};

class IResponseBuffer : public SimpleOStream {
public:
    virtual size_t Size() const = 0;
};

struct FixedMemoryBuffer : IResponseBuffer {
    FixedMemoryBuffer(size_t capacity, char* buf): buf_(buf), cur_(buf), capacity_(capacity) {
        overflow = false;
    }

    void Put(Ch c) override {
        overflow = overflow || cur_ - buf_ >= capacity_;
        if (!overflow)
            *cur_++ = c;
    }
    size_t Size() const override {
        return cur_ - buf_;
    }
    bool isOverflow() const {
        return overflow;
    }
    void Flush() override {
        return;
    }

    void Reset() {
        cur_ = buf_;
        overflow = false;
    }

private:
    char* const buf_;
    char* cur_;
    size_t capacity_;
    bool overflow;
    FixedMemoryBuffer(const FixedMemoryBuffer&);
    FixedMemoryBuffer& operator=(const FixedMemoryBuffer&);
};

template<class T>
struct StdOStreamBuf : IResponseBuffer {
    size_t size{};

    StdOStreamBuf(std::basic_ostream<T>& stream): stream_(stream) {}

    void Put(Ch c) override {
        stream_.put(c);
        ++size;
    }

    void Flush() override {
        stream_.flush();
    }
    size_t Size() const override {
        return size;
    }

private:
    StdOStreamBuf(const StdOStreamBuf<T>&) = delete;
    StdOStreamBuf& operator=(const StdOStreamBuf<T>&) = delete;
    std::basic_ostream<T>& stream_;
};

struct StringStreamBuf : SimpleOStream, std::stringstream {
    StringStreamBuf() {}

    void Put(Ch c) override {
        put(c);
    }

    void Flush() override {
        flush();
    }
    size_t Size() {
        return tellp();
    }

private:
    StringStreamBuf(const StringStreamBuf&) = delete;
    StringStreamBuf& operator=(const StringStreamBuf&) = delete;
};
typedef JsonTextStream<IResponseBuffer>::type ResponseWriter;

const FILETIME NullFileTime = {0, 0};

inline bool GetSafeBoolValue(const Value& v, mplc::lib::string_view name, bool defValue = false) {
    if (!v.IsObject())
        return defValue;
    auto m = v.FindMember(Value(name.data(), name.size()));
    return m != v.MemberEnd() && m->value.IsBool() ? m->value.GetBool() : defValue;
}

inline int GetSafeIntValue(const Value& v, mplc::lib::string_view name, int defValue = 0) {
    if (!v.IsObject())
        return defValue;
    auto m = v.FindMember(Value(name.data(), name.size()));
    return m != v.MemberEnd() && m->value.IsInt() ? m->value.GetInt() : defValue;
}

inline unsigned int GetSafeUIntValue(const Value& v, mplc::lib::string_view name, int defValue = 0) {
    if (!v.IsObject())
        return defValue;
    auto m = v.FindMember(Value(name.data(), name.size()));
    return m != v.MemberEnd() && m->value.IsUint() ? m->value.GetUint() : defValue;
}

inline double GetSafeDoubleValue(const Value& v, mplc::lib::string_view name, double defValue = 0) {
    if (!v.IsObject())
        return defValue;
    auto m = v.FindMember(Value(name.data(), name.size()));
    return m == v.MemberEnd() || m->value.IsNull() ? defValue :
           m->value.IsNumber()                     ? m->value.GetDouble() :
           m->value.IsString()                     ? atof(m->value.GetString()) :
                                                     defValue;
}

inline int64_t GetSafeInt64Value(const Value& v, mplc::lib::string_view name, int64_t defValue = 0) {
    if (!v.IsObject())
        return defValue;
    auto m = v.FindMember(Value(name.data(), name.size()));
    return m == v.MemberEnd() || m->value.IsNull() ? defValue :
           m->value.IsInt64()                      ? m->value.GetInt64() :
           m->value.IsNumber()                     ? static_cast<int64_t>(m->value.GetDouble()) :
           m->value.IsString()                     ? _atoi64(m->value.GetString()) :
                                                     defValue;
}
inline const Value& GetSafeJsonValue(const Value& v, mplc::lib::string_view name) {
    static const Value null;
    if (!v.IsObject())
        return null;
    auto m = v.FindMember(Value(name.data(), name.size()));
    return m != v.MemberEnd() ? m->value : null;
}
inline std::string GetSafeStringValue(const Value& v, mplc::lib::string_view name, std::string defValue = {}) {
    if (!v.IsObject())
        return defValue;
    auto m = v.FindMember(Value(name.data(), name.size()));
    return m != v.MemberEnd() && m->value.IsString() ? std::string(m->value.GetString(), m->value.GetStringLength()) :
                                                       defValue;
}
inline mplc::lib::string_view GetStringViewValue(const Value& v,
                                                 mplc::lib::string_view name,
                                                 mplc::lib::string_view defValue = {}) {
    if (!v.IsObject())
        return defValue;
    auto m = v.FindMember(Value(name.data(), name.size()));
    return m != v.MemberEnd() && m->value.IsString() ?
               mplc::lib::string_view(m->value.GetString(), m->value.GetStringLength()) :
               defValue;
}

const OpcUa_DateTime OpcUa_DateTime_Null = {0, 0};

inline OpcUa_Int64 OpcUa_GetDateTimeDiff(const OpcUa_DateTime& a_Value1, const OpcUa_DateTime& a_Value2) {
    return OpcUa_DateTime_ToInt64(a_Value1) - OpcUa_DateTime_ToInt64(a_Value2);
}

inline bool operator<(const OpcUa_DateTime& v1, const OpcUa_DateTime& v2) {
    return OpcUa_DateTime_ToInt64(v1) < OpcUa_DateTime_ToInt64(v2);
}

inline bool operator<=(const OpcUa_DateTime& v1, const OpcUa_DateTime& v2) {
    return OpcUa_DateTime_ToInt64(v1) <= OpcUa_DateTime_ToInt64(v2);
}

inline bool operator>(const OpcUa_DateTime& v1, const OpcUa_DateTime& v2) {
    return OpcUa_DateTime_ToInt64(v1) > OpcUa_DateTime_ToInt64(v2);
}

inline bool operator>=(const OpcUa_DateTime& v1, const OpcUa_DateTime& v2) {
    return OpcUa_DateTime_ToInt64(v1) >= OpcUa_DateTime_ToInt64(v2);
}

inline bool operator==(const OpcUa_DateTime& v1, const OpcUa_DateTime& v2) {
    return OpcUa_DateTime_ToInt64(v1) == OpcUa_DateTime_ToInt64(v2);
}

inline bool operator!=(const OpcUa_DateTime& v1, const OpcUa_DateTime& v2) {
    return OpcUa_DateTime_ToInt64(v1) != OpcUa_DateTime_ToInt64(v2);
}

inline OpcUa_DateTime operator-(const OpcUa_DateTime& v1, OpcUa_Int64 v2) {
    OpcUa_Int64 v = OpcUa_DateTime_ToInt64(v1) - v2;
    return OpcUa_DateTime_FromInt64(v);
}

inline OpcUa_DateTime GetSafeDateTimeValue(const Value& v, mplc::lib::string_view name) {
    // const Value::Member* m = v.FindMember(name.c_str());
    int64_t rawtime = GetSafeInt64Value(v, name);
    OpcUa_Int64 ft = timetms_to_filetime(rawtime);
    return OpcUa_DateTime_FromInt64(ft);
}
inline int64_t GetSafeInt64FTValue(const Value& v, mplc::lib::string_view name) {
    return timetms_to_filetime(GetSafeInt64Value(v, name));
}
template<typename T>
OpcUa_StatusCode WriteDateValue(const int64_t& value, JsonWriter<T>& writer) {
    int64_t rawtime = filetime_to_timetms(value);
    //if (rawtime < 0)
    //    rawtime = value / FT_MILLISECOND;
    writer.Int64(rawtime);
    return OpcUa_Good;
}
template<typename T>
OpcUa_StatusCode WriteDateValueDbl(const int64_t& value, JsonWriter<T>& writer) {
    double rawtime = value == 0 ? 0 : (value - 116444736000000000LL) / 10000.0;
    //if (rawtime < 0)
    //    rawtime = value / 10000.0;
    writer.Double(rawtime);
    return OpcUa_Good;
}
template<typename T>
OpcUa_StatusCode WriteDateValue(const OpcUa_DateTime& value, JsonWriter<T>& writer) {
    OpcUa_Int64 t = OpcUa_DateTime_ToInt64(value);
    return WriteDateValue(t, writer);
}

MPLCSHARE_API OpcUa_StatusCode ReadVarValue(OpcUa_VariantHlp& value,
                                            const Value& v,
                                            OpcUa_BuiltInType uaType = OpcUaType_Null);
// template<typename SO, typename SE, typename TE, typename A>
// OpcUa_StatusCode WriteVarValue(const OpcUa_VariantHlp& value, Writer<SO, SE, TE, A>& writer) {
template<typename T>
OpcUa_StatusCode WriteVarValue(const OpcUa_VariantHlp& value, JsonWriter<T>& writer) {  //-V1071
    switch (value.GetType()) {
    case OpcUaType_Null:
        writer.Null();
        break;
    case OpcUaType_Variant:
        writer.Null();  // Сюда не должен заходить, так как из lua должен читаться реальный тип
        break;
    case OpcUaType_Boolean:
        writer.Bool(value.GetRawValue().Boolean != OpcUa_False);
        break;
    case OpcUaType_Byte:
        writer.Uint(value.GetRawValue().Byte);
        break;
    case OpcUaType_SByte:
        writer.Int(value.GetRawValue().SByte);
        break;
    case OpcUaType_Int16:
        writer.Int(value.GetRawValue().Int16);
        break;
    case OpcUaType_UInt16:
        writer.Uint(value.GetRawValue().UInt16);
        break;
    case OpcUaType_Int32:
        writer.Int(value.GetRawValue().Int32);
        break;
    case OpcUaType_StatusCode:
        writer.Uint(value.GetRawValue().StatusCode);
        break;
    case OpcUaType_UInt32:
        writer.Uint(value.GetRawValue().UInt32);
        break;
    case OpcUaType_Int64:
        writer.Int64(value.GetRawValue().Int64);
        break;
    case OpcUaType_UInt64:
        writer.Uint64(value.GetRawValue().UInt64);
        break;
    case OpcUaType_Float: {
        float f = value.GetRawValue().Float;
        if (my_isnan(f) || my_isinf(f))
            writer.Null();
        else
            writer.Double(value.GetRawValue().Float);
        break;
    }
    case OpcUaType_Double: {
        double f = value.GetRawValue().Double;
        if (my_isnan(f) || my_isinf(f))
            writer.Null();
        else
            writer.Double(value.GetRawValue().Double);
        break;
    }

    case OpcUaType_DateTime:
        WriteDateValue(value.GetRawValue().DateTime, writer);
        break;

    case OpcUaType_String: {
        OpcUa_CharA* str = OpcUa_String_GetRawString(&value.GetRawValue().String);
        if (str == nullptr)
            writer.String("");
        else
            writer.String(str, OpcUa_String_StrSize(&value.GetRawValue().String));
        break;
    }
    case OpcUaType_JsonElement: {
        value.GetJson().get().Accept(writer);
        break;
    }
    case OpcUaType_DataValue: {
        writer.StartObject();
        writer.Key("Value");
        OpcUa_ReturnErrorIfBad(WriteVarValue(value.GetRawValue().DataValue->Value, writer));
        writer.String("SourceTime");
        WriteDateValueDbl(OpcUa_DateTime_ToInt64(value.GetRawValue().DataValue->SourceTimestamp), writer);
        writer.Key("StatusCode").Uint(value.GetRawValue().DataValue->StatusCode);
        writer.EndObject();
        break;
    }
    default:
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "<-- Invalid var type %d\n", value.GetType());
        break;
    }
    return OpcUa_Good;
}

inline const Value& operator>>(const Value& json, OpcUa_VariantHlp& v) {
    switch (json.GetType()) {
    case rapidjson::kNumberType:
        if (json.IsInt64() || json.IsUint64())
            v.SetInt64(json.GetInt64());
        else if (json.IsInt() || json.IsUint())
            v.SetInt(json.GetInt());
        else if (json.IsDouble())
            v.SetDouble(json.GetDouble());
        break;
    case rapidjson::kFalseType:
    case rapidjson::kTrueType:
        v.SetBool(json.GetType() == rapidjson::kTrueType);
        break;
    case rapidjson::kStringType:
        v.SetString(json.GetString());
        break;
    case rapidjson::kNullType:
        break;
    default:
        v.SetJson(json);
    }
    return json;
}

template<class T>
JsonWriter<T>& operator<<(JsonWriter<T>& os, const OpcUa_VariantHlp& v) {
    WriteVarValue(v, os);
    return os;
}
inline std::string DateToFormattedString(const FILETIME& v) {
    RTIME rTime;
    FileTimeToRTime(v, &rTime);
    char buf[30];
    sprintf(buf,
            "%04hd-%02hd-%02hd %02hd:%02hd:%02hd.%03hd",
            rTime.year,
            rTime.mon,
            rTime.day,
            rTime.hour,
            rTime.min,
            rTime.sec,
            rTime.msec);
    return std::string(buf);
}
inline std::string DateToFormattedString(const int64_t& v) {
    return DateToFormattedString(*reinterpret_cast<FILETIME const*>(&v));
}

inline std::string DateToFormattedString(const OpcUa_DateTime& v) {
    return DateToFormattedString(*(FILETIME*)(&v));
}

inline std::string OpcUa_DateTime_GetCurrentFormattedString() {
    RTIME rTime;
    RGetDateTimeInternal(&rTime);

    char buf[30];
    sprintf(buf,
            "%04hd-%02hd-%02hd %02hd:%02hd:%02hd.%03hd",
            rTime.year,
            rTime.mon,
            rTime.day,
            rTime.hour,
            rTime.min,
            rTime.sec,
            rTime.msec);
    return std::string(buf);
}

inline void DateTimeToTimetString(const OpcUa_DateTime& v, char* value) {
    int64_t rawtime = filetime_to_timetms(OpcUa_DateTime_ToInt64(v));
    sprintf(value, "%lld", rawtime);
}

#define OpcUa_DateTime_From_FILETIME(xFT) *((OpcUa_DateTime*)&xFT)
#define OpcUa_FILETIME_From_DateTime(xFT) *((FILETIME*)&xFT)

inline OpcUa_DateTime OpcUa_DateTime_GetCurrent() {
    FILETIME ft = getFileTime();
    return OpcUa_DateTime_From_FILETIME(ft);
}

#define TEST_CRIT_SEC

class CCriticalSection {
public:
    CCriticalSection(LPCSTR name) {
        RInitCriticalSection(&_sec, name);
#ifdef TEST_CRIT_SEC
        _locked = false;
#endif
        _name = name;
    }

    ~CCriticalSection() {
        RDoneCriticalSection(&_sec, _name.c_str());
    }
    int Lock() {
        RLockCriticalSection(&_sec);
#ifdef TEST_CRIT_SEC
        if (_locked) {
            PRINTLN("!!!Recursive lock : %s", _name.c_str());
            while (true)
                RSleep_ms(100);
        }
        _locked = true;
#endif
        return S_OK;
    }
    int Unlock() {
#ifdef TEST_CRIT_SEC
        if (!_locked) {
            PRINTLN("!!!Not locked : %s", _name.c_str());
        }
        _locked = false;
#endif
        RUnlockCriticalSection(&_sec);
        return S_OK;
    }

private:
    R_CRITICAL_SECTION _sec;
#ifdef TEST_CRIT_SEC
    bool _locked;
#endif
    std::string _name;
};

template<class T>
class CLockSection {
public:
    CLockSection(T* pObject, bool bInitiallyOwn = true): m_pObject(pObject) {
        if (bInitiallyOwn) {
            m_pObject->Lock();
            m_bOwn = true;
        } else
            m_bOwn = false;
    }
    ~CLockSection() {
        if (m_bOwn)
            Unlock();
    }
    void Lock() {
        if (!m_bOwn)
            m_pObject->Lock();
        m_bOwn = true;
    }
    void Unlock() {
        if (m_bOwn)
            m_pObject->Unlock();
        m_bOwn = false;
    }
    T* m_pObject;

private:
    bool m_bOwn;
};

typedef CLockSection<CCriticalSection> CLockCriticalSection;

class CSemaphore {
public:
    MPLCSHARE_API CSemaphore();
    MPLCSHARE_API ~CSemaphore();
    MPLCSHARE_API int Init(int initialCount, int maxCount);

    // Ожидание освобождения count раз
    MPLCSHARE_API bool Wait(int count, OpcUa_Int64 timeout = -1);
    MPLCSHARE_API void Post();

private:
#ifdef WIN32
    HANDLE sem;
#else
    sem_t sem;
    bool inited;
#endif
}
#ifndef WIN32
__attribute__((aligned(8)))
#endif
;
