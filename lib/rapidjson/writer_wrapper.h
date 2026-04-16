#ifndef RAPIDJSON_EASY_WRITER_H_
#define RAPIDJSON_EASY_WRITER_H_

#include "writer.h"
#include <list>
#include <map>
#include <vector>
#include <mplc/libs/smart_ptr.hpp>
#include <mplc/libs/string_view.hpp>
RAPIDJSON_NAMESPACE_BEGIN

template<class Writer>
class WriterWrapper : public Writer {
    typedef Writer BaseWriter;
    typedef typename BaseWriter::Ch Ch;
    typedef typename BaseWriter::OS OutputStream;
    bool is_ok;

public:
    explicit WriterWrapper(OutputStream& os): BaseWriter(os), is_ok(true) {}

    explicit WriterWrapper(): is_ok(true) {}

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    WriterWrapper(WriterWrapper&& rhs) RAPIDJSON_NOEXCEPT : BaseWriter(rhs), is_ok(rhs.is_ok) {}
#endif

    WriterWrapper& StartObject() {
        if (is_ok)
            is_ok = BaseWriter::StartObject();
        return *this;
    }

    WriterWrapper& Null() {
        if (is_ok)
            is_ok = BaseWriter::Null();
        return *this;
    }
    WriterWrapper& Bool(bool b) {
        if (is_ok)
            is_ok = BaseWriter::Bool(b);
        return *this;
    }
    WriterWrapper& Int(int i) {
        if (is_ok)
            is_ok = BaseWriter::Int(i);
        return *this;
    }
    WriterWrapper& Uint(unsigned u) {
        if (is_ok)
            is_ok = BaseWriter::Uint(u);
        return *this;
    }
    WriterWrapper& Int64(int64_t i64) {
        if (is_ok)
            is_ok = BaseWriter::Int64(i64);
        return *this;
    }
    WriterWrapper& Uint64(uint64_t u64) {
        if (is_ok)
            is_ok = BaseWriter::Uint64(u64);
        return *this;
    }

    //! Writes the given \c double value to the stream
    /*!
        \param d The value to be written.
        \return Whether it is succeed.
    */
    WriterWrapper& Double(double d) {
        if (is_ok)
            is_ok = BaseWriter::Double(d);
        return *this;
    }

    WriterWrapper& RawNumber(const Ch* str, SizeType length, bool copy = false) {
        if (is_ok)
            is_ok = BaseWriter::RawNumber(str, length, copy);
        return *this;
    }

    template<SizeType N>
    WriterWrapper& String(const char (&str)[N]) {
        if (is_ok)
            is_ok = BaseWriter::String(str, N - 1, false);
        return *this;
    }

    WriterWrapper& String(const Ch* str, SizeType length, bool copy = false) {
        if (is_ok)
            is_ok = BaseWriter::String(str, length, copy);
        return *this;
    }

#if RAPIDJSON_HAS_STDSTRING
    /*WriterWrapper& String(const std::basic_string<Ch>& str) {
        if (is_ok)
            is_ok = BaseWriter::String(str);
        return *this;
    }*/
    WriterWrapper& String(const mplc::lib::string_view& str) {
        if (is_ok)
            is_ok = BaseWriter::String(str.data(), str.size());
        return *this;
    }
#endif

    template<SizeType N>
    WriterWrapper& Key(const char (&str)[N]) {  //-V1071
        if (is_ok)
            is_ok = BaseWriter::Key(str, N - 1, false);
        return *this;
    }

    WriterWrapper& Key(const Ch* str, SizeType length, bool copy = false) {
        if (is_ok)
            is_ok = BaseWriter::Key(str, length, copy);
        return *this;
    }

#if RAPIDJSON_HAS_STDSTRING
    WriterWrapper& Key(const std::basic_string<Ch>& str) {
        if (is_ok)
            is_ok = BaseWriter::Key(str);
        return *this;
    }
#endif

    WriterWrapper& EndObject(SizeType memberCount = 0) {
        if (is_ok)
            is_ok = BaseWriter::EndObject(memberCount);
        return *this;
    }

    WriterWrapper& StartArray() {
        if (is_ok)
            is_ok = BaseWriter::StartArray();
        return *this;
    }

    WriterWrapper& EndArray(SizeType elementCount = 0) {
        if (is_ok)
            is_ok = BaseWriter::EndArray(elementCount);
        return *this;
    }
    //@}

    /*! @name Convenience extensions */
    //@{

    //! Simpler but slower overload.
    WriterWrapper& String_(const Ch* str) {
        if (is_ok)
            is_ok = BaseWriter::String(str);
        return *this;
    }
    WriterWrapper& Key_(const Ch* str) {
        if (is_ok)
            is_ok = BaseWriter::Key(str);
        return *this;
    }

    //@}

    //! Write a raw JSON value.
    /*!
        For user to write a stringified JSON as a value.

        \param json A well-formed JSON value. It should not contain null character within [0, length
       - 1] range. \param length Length of the json. \param type Type of the root of json.
    */
    WriterWrapper& RawValue(const Ch* json, size_t length, Type type) {
        if (is_ok)
            is_ok = BaseWriter::RawValue(json, length, type);
        return *this;
    }

    WriterWrapper& operator<<(int32_t v) {
        return Int(v);
    }
    WriterWrapper& operator<<(uint32_t v) {
        return Uint(v);
    }
    WriterWrapper& operator<<(int64_t v) {
        return Int64(v);
    }
    WriterWrapper& operator<<(uint64_t v) {
        return Uint64(v);
    }
    WriterWrapper& operator<<(double v) {
        return Double(v);
    }
    WriterWrapper& operator<<(bool v) {
        return Bool(v);
    }

    WriterWrapper& operator<<(const std::string& v) {
        return String(v);
    }
    template<size_t N>
    WriterWrapper& operator<<(const char (&v)[N]) {
        return String(v, N - 1);
    }
    // WriterWrapper& operator<<(const char* v) { return String(v ? v : ""); }
    /*template<class T>
    WriterWrapper& operator<<(T* v) {
        return v ? *this << *v : Null();
    }*/

    template<class T>
    WriterWrapper& operator<<(const std::vector<T>& v) {
        StartArray();
        typename std::vector<T>::const_iterator it = v.begin();
        for (; it != v.end(); ++it) {
            *this << *it;
        }
        EndArray();
        return *this;
    }
    template<class T>
    WriterWrapper& operator<<(const std::list<T>& v) {
        StartArray();
        typename std::list<T>::const_iterator it = v.begin();
        for (; it != v.end(); ++it) {
            *this << *it;
        }
        EndArray();
        return *this;
    }
    template<class K, class V>
    WriterWrapper& operator<<(const std::map<K, V>& v) {
        StartObject();
        typename std::map<K, V>::const_iterator it = v.begin();
        for (; it != v.end(); ++it) {
            Key(it->first);
            *this << it->second;
        }
        EndObject();
        return *this;
    }

    operator bool() const {
        return is_ok;
    }
};
template<class WT, class T>
WriterWrapper<WT>& operator<<(WriterWrapper<WT>& os, const mplc::lib::shared_ptr<T>& v) {
    if (v) {
        os << *v;
    } else {
        os.Null();
    }
    return os;
}
//
//
// template<typename Encoding, typename Allocator>
// template<typename OS, typename SE, typename TE, typename SA, unsigned wf>
// bool GenericValue<Encoding, Allocator>::Accept(WriterWrapper<OS, SE, TE, SA, wf>& handler) const
// {
//    switch(GetType()) {
//    case kNullType:
//        return handler.Null();
//    case kFalseType:
//        return handler.Bool(false);
//    case kTrueType:
//        return handler.Bool(true);
//
//    case kObjectType:
//        if(RAPIDJSON_UNLIKELY(!handler.StartObject())) return false;
//        for(ConstMemberIterator m = MemberBegin(); m != MemberEnd(); ++m) {
//            RAPIDJSON_ASSERT(
//                m->name.IsString());  // User may change the type of name by MemberIterator.
//            if(RAPIDJSON_UNLIKELY(!handler.Key(m->name.GetString(),
//                                               m->name.GetStringLength(),
//                                               (m->name.data_.f.flags & kCopyFlag) != 0)))
//                return false;
//            if(RAPIDJSON_UNLIKELY(!m->value.Accept(handler))) return false;
//        }
//        return handler.EndObject(data_.o.size);
//
//    case kArrayType:
//        if(RAPIDJSON_UNLIKELY(!handler.StartArray())) return false;
//        for(const GenericValue* v = Begin(); v != End(); ++v)
//            if(RAPIDJSON_UNLIKELY(!v->Accept(handler))) return false;
//        return handler.EndArray(data_.a.size);
//
//    case kStringType:
//        return handler.String(GetString(), GetStringLength(), (data_.f.flags & kCopyFlag) != 0);
//
//    default:
//        RAPIDJSON_ASSERT(GetType() == kNumberType);
//        if(IsDouble())
//            return handler.Double(data_.n.d);
//        else if(IsInt())
//            return handler.Int(data_.n.i.i);
//        else if(IsUint())
//            return handler.Uint(data_.n.u.u);
//        else if(IsInt64())
//            return handler.Int64(data_.n.i64);
//        else
//            return handler.Uint64(data_.n.u64);
//    }
//}
RAPIDJSON_NAMESPACE_END

#endif
